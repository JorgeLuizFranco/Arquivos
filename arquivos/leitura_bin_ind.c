#include "leitura_bin_ind.h"

void le_cabecalho_indice(FILE* arq_indices, cabecalho_indice_t** cabecalho_indice) {
    char status;
    fread(&status, sizeof(char), 1, arq_indices);
    *cabecalho_indice = cria_cabecalho_indice(status);
}

void le_dado_int(FILE* arq_indices, dados_int_t** dado_int) {
    int chaveBusca;
    long long int byteOffset;
    fread(&chaveBusca, sizeof(int), 1, arq_indices);
    fread(&byteOffset, sizeof(long long int), 1, arq_indices);
    *dado_int = cria_dados_int(chaveBusca, byteOffset);
}

void le_dado_str(FILE* arq_indices, dados_str_t** dado_str) {
    char chaveBusca[12];
    long long int byteOffset;
    fread(chaveBusca, sizeof(char), 12, arq_indices);
    fread(&byteOffset, sizeof(long long int), 1, arq_indices);
    *dado_str = cria_dados_str(chaveBusca, byteOffset);
}

void le_arq_indices(FILE* arq_indices, char* tipo_campo, dados_int_t*** dados_int, dados_str_t*** dados_str, cabecalho_indice_t** cabecalho_indice, int* num_ind) {
    *num_ind = 0;
    if (arq_indices == NULL) return;

    le_cabecalho_indice(arq_indices, cabecalho_indice);

    if (*cabecalho_indice == NULL || (*cabecalho_indice)->status == '0') {
        if (*cabecalho_indice != NULL) free(*cabecalho_indice);
        *cabecalho_indice = NULL;
        return;
    }

    int num_dados = 0;
    while (feof(arq_indices) == 0) {
        (*num_ind)++;
        switch (tipo_campo[0]) {
            case 'i':{
                dados_int_t** dados_int_realloc = (dados_int_t**) realloc(*dados_int, (++num_dados)*sizeof(dados_int_t*));
                if (dados_int_realloc == NULL) {
                    free(*cabecalho_indice);
                    *cabecalho_indice = NULL;
                    libera_vetor_ate_pos((void**)(*dados_int), num_dados-2);
                    return;
                }
                *dados_int = dados_int_realloc;

                le_dado_int(arq_indices, &((*dados_int)[num_dados-1]));
                if ((*dados_int)[num_dados-1] == NULL) {
                    free(*cabecalho_indice);
                    *cabecalho_indice = NULL;
                    libera_vetor_ate_pos((void**)(*dados_int), num_dados-1);
                }
                break;
            }
            case 's':{
                dados_str_t** dados_str_realloc = (dados_str_t**) realloc(*dados_str, (++num_dados) * sizeof(dados_str_t*));
                if (dados_str_realloc == NULL) {
                    free(*cabecalho_indice);
                    *cabecalho_indice = NULL;
                    libera_vetor_ate_pos((void**)(*dados_str), num_dados-2);
                }
                *dados_str = dados_str_realloc;

                le_dado_str(arq_indices, &((*dados_str)[num_dados-1]));
                if ((*dados_str)[num_dados-1] == NULL) {
                    free(*cabecalho_indice);
                    *cabecalho_indice = NULL;
                    libera_vetor_ate_pos((void**)(*dados_str), num_dados-1);
                }
                break;
            }
        }
    }
}