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

    binarioNaTela(arq_bin); // função pedida pra ser executada
    fclose(arq_bin); // fecha o binário
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
    free(cabecalho);

    FILE* arq_ind = fopen(nome_arq_ind, "wb");
    if (arq_ind == NULL) {
        fclose(arq_bin);
        erro();
        return;
    }

    escreve_arq_ind(arq_bin, arq_ind, nome_campo, tipo_campo, n_registros);

    fclose(arq_bin);
    binarioNaTela(arq_ind);
    fclose(arq_ind);
}

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {

    // se esse arquivo de indice existir, armazeno ele na memoria
    // se não, digo que ele e NULL



    FILE* arq_bin = fopen(nome_arq_bin, "rb");
    if (arq_bin == NULL) {
        erro();
        return;
    }

    FILE* arq_idx = fopen(nome_arq_idx, "rb");

    dados_int_t** dados_int = NULL;
    dados_str_t** dados_str = NULL;
    cabecalho_indice_t* cabecalho_indice = NULL;

    le_arq_indices(arq_idx, tipo_campo, &dados_int, &dados_str, &cabecalho_indice);

    for (int i = 0; i < num_consultas; i++) {
        int num_campos;
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            erro();
            return;
        }

        // sera que tem o campo do arquivo de indice?
        int flag_campo_procurado = 0;
        for (int j = 0; j < num_campos; j++) {
            if (strcmp(campos[j]->campo_busca, nome_campo) == 0) {
                flag_campo_procurado = 1;
                break;
            }
        }
        // se nao tiver ou nem existir indice, fazer uma busca linear no arquivo binario

        if (cabecalho_indice == NULL || (cabecalho_indice != NULL && flag_campo_procurado)) {
            
        }

        // se tiver (e existir), ver todos os caras com o indice que quero no arquivo de indices com busca binaria
        // percorrer esses caras, e pra cada um deles eu vou pra cada byteoffset do arquivo que to lendo e vejo se obedece
        // os outros requisitos



        free(campos);
    }
}