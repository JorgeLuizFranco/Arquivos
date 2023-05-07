#include "busca.h"

campo_busca_t* le_campo_busca() {
    campo_busca_t* campo_atual = (campo_busca_t*) malloc(sizeof(campo_busca_t));
    if (campo_atual == NULL) return NULL;

    scanf("%s", campo_atual->campo_busca);

    if (strcmp("idCrime", campo_atual->campo_busca) == 0 || strcmp("numeroArtigo", campo_atual->campo_busca) == 0) {
        scanf("%d", &(campo_atual->chaveBuscaInt));
    } else {
        scan_quote_string(campo_atual->chaveBuscaStr);
    }
    return campo_atual;
}

campo_busca_t** le_campos_busca(int num_campos) {

    campo_busca_t** campos_buscados = (campo_busca_t**) malloc(num_campos * sizeof(campo_busca_t*));
    if (campos_buscados == NULL) return NULL;

    for (int i = 0; i < num_campos; i++) {
        campos_buscados[i] = le_campo_busca();
        if (campos_buscados[i] == NULL) {
            libera_vetor_ate_pos((void**)campos_buscados, i-1);
            return NULL;
        }
    }

    return campos_buscados;
}

int satisfaz_campo(crime_t* crime_atual, campo_busca_t* campo_atual) {
    if (strcmp(campo_atual->campo_busca, "idCrime") == 0) {
        return crime_atual->idCrime == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "numeroArtigo") == 0) {
        return crime_atual->numeroArtigo == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "dataCrime") == 0) {
        return compara_string_limitada(crime_atual->dataCrime, campo_atual->chaveBuscaStr, 10, 1) == 0;
    } else if (strcmp(campo_atual->campo_busca, "marcaCelular") == 0) {
        return compara_string_limitada(crime_atual->marcaCelular, campo_atual->chaveBuscaStr, 12, 1) == 0;
    } else {
        char* string_dinamica_original;
        
        if (strcmp(campo_atual->campo_busca, "descricaoCrime") == 0) {
            string_dinamica_original = crime_atual->descricaoCrime;
        } else {
            string_dinamica_original = crime_atual->lugarCrime;
        }

        return strcmp(campo_atual->chaveBuscaStr, string_dinamica_original) == 0;
    }
    return -1;
}

int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos) {
    int resposta = 1; // satisfaz. Se eu encontrar evidencias do contrario, seto pra zero

    campo_busca_t* campo_atual;

    for (int i = 0; i < n_campos; i++) {
        campo_atual = query_atual[i];
        if (satisfaz_campo(crime_atual, campo_atual) == 0) {
            resposta = 0;
            break;
        }
    }

    return resposta;
}

void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var) {
    // quero que *low aponte pro menor para e *high aponte para o maior cara que satisfaçam a busca
    // fprintf(stderr, "Tipo var eh %d\n", tipo_var);
    *low_reg = -1;
    *high_reg = -2;

    int low = 0, high = num_regs - 1;
    int mid;
    int result=-1;

    void* reg_mid;

    while (low <= high) {

        mid = (low + high) / 2;
        
        reg_mid = ind_int[mid];

        if (compara_chave_busca(reg_mid, chaveBusca, 1, tipo_var)) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    if (result == -1 || compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var) == 0)
        return;
    *low_reg = result;

    low = 0, high = num_regs - 1;

    //fprintf(stderr, "==%d====%d====%d==\n", result, ((dados_int_t*)reg_mid)->chaveBusca, *((int*)chaveBusca));
    result = -1;
    while (low <= high) {
        mid = (low + high) / 2;

        reg_mid = ind_int[mid];
        //fprintf(stderr, "Estou comparando %d com %d (ind %d)\n", ((dados_int_t*)reg_mid)->chaveBusca, *((int*)chaveBusca), mid);

        if (compara_chave_busca(reg_mid, chaveBusca, -1, tipo_var) == 0) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }
    if (result-- <= 0) {
        result = -2;
    } else if (!compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var)) {
        result = -2;
    }
    *high_reg = result;
}

int indice_procura_registro(crime_t* crime_atual, long long int offset_atual, void** dados, int num_dados, char* nome_campo, int tipoVar) {
    int low_reg, high_reg;
    void* dado_atual;
    void* dado_crime = pega_dado_generico(crime_atual, nome_campo, -1, tipoVar); // so serve pra pegar chave busca a partir do nome do campo
    if (dado_crime == NULL) return -2;


    busca_bin_campos(dados, num_dados, &low_reg, &high_reg, pega_chave_generico(dado_crime, tipoVar), tipoVar);
    free(dado_crime);

    if (low_reg < 0) return -1;

    dado_atual = dados + low_reg;
    while (low_reg <= high_reg) {
        
        if (offset_atual == pega_offset_generico(dado_atual, tipoVar)) {
            return low_reg;        
        }

        low_reg++;
        dado_atual++;
    }

    return -1;
}

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade) {

    // se esse arquivo de indice existir, armazeno ele na memoria
    // se não, digo que ele e NULL



    FILE* arq_bin = fopen(nome_arq_bin, "rb+");
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

    FILE* arq_idx = fopen(nome_arq_idx, "rb+");

    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    cabecalho_indice_t* cabecalho_indice = NULL;
    crime_t* crime_atual = NULL;
    int num_indices;

    fprintf(stderr, "a\n");
    le_arq_indices(arq_idx, &dados, tipoVar, &cabecalho_indice, &num_indices);
    fprintf(stderr, "a\n");

    for (int i = 0; i < num_consultas; i++) {

        int regs_mostrados = 0;
        if (funcionalidade == 4)
            printf("Resposta para a busca %d\n", i+1);

        int num_campos;
        if (i != 0) desloca_offset(arq_bin, TAMANHO_CABECALHO);
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            free(cabecalho);
            fclose(arq_bin);
            fclose(arq_idx);
            erro();
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
                    if (cabecalho_indice != NULL) {
                        fclose(arq_idx);
                        free(cabecalho_indice);
                        if (dados != NULL) libera_vetor_ate_pos(dados, num_indices-1);
                    }
                    free(cabecalho);
                    fclose(arq_bin);
                    libera_vetor_ate_pos((void**)campos, num_campos-1);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        // remover esse crime do arquivo binario marcando a flag removido como 1
                        desloca_offset(arq_bin, byteOffset);
                        crime_atual->removido = '1';
                        cabecalho->nroRegRem++;
                        escreve_registro_criminal(arq_bin, crime_atual);
                        // procurar no arquivo de índices o registro atual e remove-lo por shiftacao
                        int ind_arquivo = indice_procura_registro(crime_atual, byteOffset, dados, num_indices, nome_campo, tipoVar);
                        if (ind_arquivo >= 0) {
                            // se estiver presente no arquivo de indices, remove da lista de dados
                            remove_dado(&dados, tipoVar, &num_indices, ind_arquivo);
                            cabecalho_indice->nro_reg--;
                            fprintf(stderr, "Agora eh %d\n", cabecalho_indice->nro_reg);
                        }
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
            fprintf(stderr, "a\n");
            busca_bin_campos(dados, num_indices, &low, &high, chaveBusca, tipoVar);
            fprintf(stderr, "a\n");
            while (low <= high) {
                
                void* dado_atual = dados[low];
                byteOffset = pega_offset_generico(dado_atual, tipoVar);
                crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
                fprintf(stderr, "a\n");

                if (crime_atual == NULL) {
                    fclose(arq_idx);
                    free(cabecalho_indice);
                    if (dados != NULL) libera_vetor_ate_pos(dados, num_indices-1);
                    
                    free(cabecalho);
                    fclose(arq_bin);
                    libera_vetor_ate_pos((void**)campos, num_campos-1);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        fprintf(stderr, "a\n");
                        desloca_offset(arq_bin, byteOffset);
                        crime_atual->removido = '1';
                        cabecalho->nroRegRem++;
                        cabecalho_indice->nro_reg--;
                        escreve_registro_criminal(arq_bin, crime_atual);
                        fprintf(stderr, "a %d a\n", cabecalho_indice->nro_reg);
                        remove_dado(&dados, tipoVar, &num_indices, low);

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
            desloca_offset(arq_idx, 0);
            escreve_cabecalho_ind(arq_idx, cabecalho_indice);
            escreve_dados_gen(arq_idx, dados, tipoVar, num_indices);
        }
    }

    free(cabecalho);

    fclose(arq_bin);
    fclose(arq_idx);

    if (cabecalho_indice != NULL) free(cabecalho_indice);
    libera_vetor_ate_pos(dados, num_indices-1);

    if (funcionalidade == 5) {
        binarioNaTela(nome_arq_bin);
        binarioNaTela(nome_arq_idx);
    }
}