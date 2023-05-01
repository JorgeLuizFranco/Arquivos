#include "funcionalidades.h"

/**
 * FUNCIONALIDADE 1
 * Lê o CSV contendo os crimes e transcreve sua informação para um arquivo binário.
 *
 * @param nome_arq_csv Nome do arquivo CSV 
 * @param nome_arq_bin Nome do arquivo binário
 */
void csv_para_bin(char* nome_arq_csv, char* nome_arq_bin) {
    
    FILE* arq_csv = fopen(nome_arq_csv, "r");
    if (arq_csv == NULL) {
        erro();
        return;
    }

    // Abre o arquivo binário de modo que ele seja limpo e que seja possível
    // ler e escrever nele
    FILE* arq_bin = fopen(nome_arq_bin, "wb+");
    if (arq_bin == NULL) {
        fclose(arq_csv);
        erro();
        return;
    }

    cabecalho_t* cabecalho = (cabecalho_t*) malloc(sizeof(cabecalho_t));
    if (cabecalho == NULL) {
        fclose(arq_csv);
        fclose(arq_bin);
        erro();
        return;
    }

    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->proxByteOffset = TAMANHO_CABECALHO; // quanto o registro cabeçalho ocupa
    cabecalho->status = '0'; // status 0 indica que ta escrevendo
    escreve_cabecalho(arq_bin, cabecalho); // vai colocando informações temporárias referentes ao cabeçalho no binário

    // pula a primeira linha do csv
    char pula_primeira_linha = 0;
    while (pula_primeira_linha != '\n')
        fscanf(arq_csv, "%c", &pula_primeira_linha);
    
    // vai lendo crime por crime do csv
    crime_t* crime_lido = NULL;
    while ((crime_lido = le_registro_criminal(arq_csv)) != NULL) {
        escreve_registro_criminal(arq_bin, crime_lido);
        cabecalho->nroRegArq++; // mais um registro
        cabecalho->proxByteOffset += tamanho_crime(crime_lido); // atualiza número de bytes do proxByteOffset
        libera_crime(crime_lido); // libera memória
    }

    // Fecha o csv
    fclose(arq_csv);
    if (crime_lido != NULL && crime_lido->lugarCrime == NULL) {
        libera_crime(crime_lido);
        free(cabecalho);
        fclose(arq_bin);
        erro();
        return;
    }
    
    // volta ao início do binário
    fseek(arq_bin, 0, SEEK_SET);
    cabecalho->status = '1'; // indica que terminou de escrever
    escreve_cabecalho(arq_bin, cabecalho); // escreve o cabeçalho atualizado

    free(cabecalho); // libera o cabeçalho

    fclose(arq_bin); // fecha o binário
    binarioNaTela(nome_arq_bin); // função pedida pra ser executada
}

/**
 * Lê o arquivo binário e imprime as informações
 * FUNCIONALIDADE 2
 *
 * @param nome_arq_bin Nome do arquivo binário.
 */
void mostra_crimes_tela(char* nome_arq_bin) {
    FILE* arq_bin = fopen(nome_arq_bin, "rb");
    if (arq_bin == NULL) {
        erro();
        return;
    }

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        fclose(arq_bin);
        erro();
        return;
    }
    int nro_registros = cabecalho->nroRegArq - cabecalho->nroRegRem;
    free(cabecalho);

    mostra_crimes_tela_arq(arq_bin, nro_registros);

    fclose(arq_bin);
}

/**
 * FUNCIONALIDADE 3
 * Cria um arquivo de índices dado o nome do arquivo binário de registros, o nome do campo,
 * atributo do campo e o arquivo binário de destino
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
*/
void cria_arq_indices(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_ind) {

    FILE* arq_bin = fopen(nome_arq_bin, "rb"); // arquivo de registros
    if (arq_bin == NULL) {
        erro();
        return;
    }

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        fclose(arq_bin);
        erro();
        return;
    }

    int n_registros = cabecalho->nroRegArq - cabecalho->nroRegRem;

    FILE* arq_ind = fopen(nome_arq_ind, "wb");
    if (arq_ind == NULL) {
        fclose(arq_bin);
        free(cabecalho);
        erro();
        return;
    }

    escreve_arq_ind(arq_bin, arq_ind, nome_campo, tipo_campo, n_registros, cabecalho->proxByteOffset);

    free(cabecalho);
    fclose(arq_bin);
    fclose(arq_ind);
    binarioNaTela(nome_arq_ind);
}

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {

    // se esse arquivo de indice existir, armazeno ele na memoria
    // se não, digo que ele e NULL



    FILE* arq_bin = fopen(nome_arq_bin, "rb");
    if (arq_bin == NULL) {
        erro();
        return;
    }
    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL) {
        fclose(arq_bin);
        erro();
        return;
    }

    FILE* arq_idx = fopen(nome_arq_idx, "rb");

    dados_int_t** dados_int = NULL;
    dados_str_t** dados_str = NULL;
    cabecalho_indice_t* cabecalho_indice = NULL;
    crime_t* crime_atual = NULL;
    int num_indices;

    le_arq_indices(arq_idx, tipo_campo, &dados_int, &dados_str, &cabecalho_indice, &num_indices);

    for (int i = 0; i < num_consultas; i++) {

        int regs_mostrados = 0;
        printf("Resposta para a busca %d\n", i+1);

        int num_campos;
        if (i != 0) desloca_offset(arq_bin, TAMANHO_CABECALHO);
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            erro();
            return;
        }

        // sera que tem o campo do arquivo de indice?
        int flag_campo_procurado = 0;
        campo_busca_t* campo_ind = NULL;
        for (int j = 0; j < num_campos; j++) {
            if (strcmp(campos[j]->campo_busca, nome_campo) == 0) {
                flag_campo_procurado = 1;
                campo_ind = campos[j];
                break;
            }
        }
        // se nao tiver ou nem existir indice, fazer uma busca linear no arquivo binario

        if (cabecalho_indice == NULL || (cabecalho_indice != NULL && flag_campo_procurado == 0)) {
            long long int byteOffset = TAMANHO_CABECALHO;
            while (byteOffset < cabecalho->proxByteOffset) {
                crime_atual = le_crime_bin(arq_bin);
                if (crime_atual == NULL) {
                    if (cabecalho_indice != NULL) {
                        fclose(arq_idx);
                        free(cabecalho_indice);
                        if (dados_int != NULL) libera_vetor_ate_pos((void**)dados_int, num_indices-1);
                        if (dados_str != NULL) libera_vetor_ate_pos((void**)dados_str, num_indices-1);
                    }
                    free(cabecalho);
                    fclose(arq_bin);
                    libera_vetor_ate_pos((void**)campos, num_campos-1);
                    return;
                }

                if (satisfaz_query(crime_atual, campos, num_campos)) {
                    mostra_crime_tela(crime_atual);
                    regs_mostrados++;
                }

                byteOffset += tamanho_crime(crime_atual);
                libera_crime(crime_atual);
            }
        } else {

            // se tiver (e existir), ver todos os caras com o indice que quero no arquivo de indices com busca binaria
            // percorrer esses caras, e pra cada um deles eu vou pra cada byteoffset do arquivo que to lendo e vejo se obedece
            // os outros requisitos

            int low;
            int high;
            void** indices = dados_int == NULL ? (void**) dados_str : (void**) dados_int;
            void* chaveBusca = dados_int == NULL ? (void*) campo_ind->chaveBuscaStr : (void*) (&(campo_ind->chaveBuscaInt));

            busca_bin_campos(indices, num_indices, &low, &high, chaveBusca, dados_int != NULL ? 0 : 1);

            while (low <= high) {
                
                if (dados_int != NULL) {
                    //fprintf(stderr, "a\n");
                    dados_int_t* dado_atual = dados_int[low];
                    //fprintf(stderr, "b %lld\n", dado_atual->byteOffset);
                    crime_atual = le_crime_bin_offset(arq_bin, dado_atual->byteOffset);
                    //fprintf(stderr, "c\n");
                } else {
                    dados_str_t* dado_atual = dados_str[low];
                    crime_atual = le_crime_bin_offset(arq_bin, dado_atual->byteOffset);
                }

                if (crime_atual == NULL) {
                    fclose(arq_idx);
                    free(cabecalho_indice);
                    if (dados_int != NULL) libera_vetor_ate_pos((void**)dados_int, num_indices-1);
                    if (dados_str != NULL) libera_vetor_ate_pos((void**)dados_str, num_indices-1);
                    
                    free(cabecalho);
                    fclose(arq_bin);
                    libera_vetor_ate_pos((void**)campos, num_campos-1);
                    return;
                }

                if (satisfaz_query(crime_atual, campos, num_campos)) {
                    mostra_crime_tela(crime_atual);
                    regs_mostrados++;
                }

                libera_crime(crime_atual);
                low++;
            }
        }

        if (regs_mostrados == 0) {
            printf("Registro inexistente.\n");
        }



        free(campos);
    }
}