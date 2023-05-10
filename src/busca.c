#include "busca.h"

void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var, int flag_dinamica) {
    // quero que *low aponte pro menor para e *high aponte para o maior cara que satisfaçam a busca
    *low_reg = -1;
    *high_reg = -2;

    int low = 0, high = num_regs - 1;
    int mid;
    int result=-1;

    void* reg_mid;

    while (low <= high) {

        mid = (low + high) / 2;
        
        reg_mid = ind_int[mid];

        if (compara_chave_busca(reg_mid, chaveBusca, 1, tipo_var, flag_dinamica)) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    if (result == -1 || compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var, flag_dinamica) == 0)
        return;
    *low_reg = result;

    low = 0, high = num_regs - 1;

    result = -1;
    while (low <= high) {
        mid = (low + high) / 2;

        reg_mid = ind_int[mid];
        //reg_mid > chaveBusca?
        if (compara_chave_busca(reg_mid, chaveBusca, -1, tipo_var, flag_dinamica) == 0) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    // se result = -1, n tem ngm maior do que chaveBusca
    // sei que o low é o menor cara = chaveBusca
    // entao se result = -1, qr dizer que do low ate num_regs-1 e todo mundo igual

    *high_reg = result != -1 ? result-1 : num_regs-1;
}

int indice_procura_registro(crime_t* crime_atual, long long int offset_atual, void** dados, int num_dados, char* nome_campo, int tipoVar) {
    if (dados == NULL) return -1;
    
    int low_reg, high_reg;
    void* dado_atual;
    void* dado_crime = pega_dado_generico(crime_atual, nome_campo, -1, tipoVar); // so serve pra pegar chave busca a partir do nome do campo
    if (dado_crime == NULL) return -2;

    busca_bin_campos(dados, num_dados, &low_reg, &high_reg, pega_chave_generico(dado_crime, tipoVar), tipoVar, 0);
    free(dado_crime);

    if (low_reg < 0) return -1;

    int mid_reg;
    long long int offset_dado_atual;

    while (low_reg <= high_reg) {

        mid_reg = (low_reg + high_reg) / 2;
        dado_atual = dados[mid_reg];
        offset_dado_atual = pega_offset_generico(dado_atual, tipoVar);

        if (offset_atual > offset_dado_atual) {
            low_reg = mid_reg + 1;
        } else if (offset_atual < offset_dado_atual) {
            high_reg = mid_reg - 1;
        } else {
            return mid_reg;
        }
    }

    return -1;
}

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade) {

    // se esse arquivo de indice existir, armazeno ele na memoria
    // se não, digo que ele e NULL



    FILE* arq_bin = fopen(nome_arq_bin, "rb+");
    if (arq_bin == NULL) {
        libera_memo_consultas(1, NULL, NULL, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }
    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL) {
        libera_memo_consultas(1, arq_bin, NULL, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }

    FILE* arq_idx = fopen(nome_arq_idx, "rb+");

    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    cabecalho_indice_t* cabecalho_indice = NULL;
    crime_t* crime_atual = NULL;
    int num_indices;

    le_arq_indices(arq_idx, &dados, tipoVar, &cabecalho_indice, &num_indices);
    if (dados == NULL) {
        libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, NULL, -1);
        return;
    }

    for (int i = 0; i < num_consultas; i++) {

        int regs_mostrados = 0;
        if (funcionalidade == 4)
            printf("Resposta para a busca %d\n", i+1);

        int num_campos;
        if (i != 0) desloca_offset(arq_bin, TAMANHO_CABECALHO);
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);
            return;
        }

        // sera que tem o campo do arquivo de indice?
        int flag_campo_procurado = 0;
        for (int j = 0; j < num_campos; j++) {
            if (strcmp(campos[j]->campo_busca, nome_campo) == 0) {
                flag_campo_procurado = j+1;
                break;
            }
        }
        // se nao tiver ou nem existir indice, fazer uma busca linear no arquivo binario

        if (cabecalho_indice == NULL || (cabecalho_indice != NULL && flag_campo_procurado == 0)) {
            long long int byteOffset = TAMANHO_CABECALHO;
            while (byteOffset < cabecalho->proxByteOffset) {
                crime_atual = le_crime_bin(arq_bin);
                if (crime_atual == NULL) {
                    libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        // remover esse crime do arquivo binario marcando a flag removido como 1
                        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
                        // procurar no arquivo de índices o registro atual e remove-lo por shiftacao
                        int ind_arquivo = indice_procura_registro(crime_atual, byteOffset, dados, num_indices, nome_campo, tipoVar);
                        remove_com_shift(&dados, tipoVar, &num_indices, ind_arquivo, cabecalho_indice);
                    }
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
            long long int byteOffset;
            void* chaveBusca = tipoVar == 0 ? (void*)&(campos[flag_campo_procurado-1]->chaveBuscaInt) : (void*)(campos[flag_campo_procurado-1]->chaveBuscaStr);
            busca_bin_campos(dados, num_indices, &low, &high, chaveBusca, tipoVar, 1);
            while (low <= high) {
                
                void* dado_atual = dados[low];
                byteOffset = pega_offset_generico(dado_atual, tipoVar);
                crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
            
                if (crime_atual == NULL) {
                    libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
                        remove_com_shift(&dados, tipoVar, &num_indices, low, cabecalho_indice);
                        low--;
                        high--;
                    }
                }

                libera_crime(crime_atual);
                low++;
            }
        }

        if (regs_mostrados == 0 && funcionalidade == 4) {
            printf("Registro inexistente.\n");
        }

        libera_vetor_ate_pos((void**)campos, num_campos-1);

    }

    if (funcionalidade == 5) {
        desloca_offset(arq_bin, 0);
        escreve_cabecalho(arq_bin, cabecalho);
        if (arq_idx != NULL) {
            fclose(arq_idx);
            arq_idx = fopen(nome_arq_idx, "wb");
            escreve_cabecalho_ind(arq_idx, cabecalho_indice);
            escreve_dados_gen(arq_idx, dados, tipoVar, num_indices);
        }
    }

   libera_memo_consultas(0, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);

    if (funcionalidade == 5) {
        binarioNaTela(nome_arq_bin);
        binarioNaTela(nome_arq_idx);
    }
}