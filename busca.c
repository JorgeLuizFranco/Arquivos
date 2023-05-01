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

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, void (*funcao)(crime_t* crime_atual, FILE* arq_bin, long long int offset)) {

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

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    funcao(crime_atual, arq_bin, byteOffset);
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
            long long int byteOffset;
            void** indices = dados_int == NULL ? (void**) dados_str : (void**) dados_int;
            void* chaveBusca = dados_int == NULL ? (void*) campo_ind->chaveBuscaStr : (void*) (&(campo_ind->chaveBuscaInt));

            busca_bin_campos(indices, num_indices, &low, &high, chaveBusca, dados_int != NULL ? 0 : 1);
            
            while (low <= high) {
                
                if (dados_int != NULL) {
                    //fprintf(stderr, "a\n");
                    dados_int_t* dado_atual = dados_int[low];
                    byteOffset = dado_atual->byteOffset;
                    //fprintf(stderr, "b %lld\n", dado_atual->byteOffset);
                    crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
                    //fprintf(stderr, "c\n");
                    
                } else {
                    dados_str_t* dado_atual = dados_str[low];
                    byteOffset = dado_atual->byteOffset;
                    crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
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

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    funcao(crime_atual, arq_bin, byteOffset);
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