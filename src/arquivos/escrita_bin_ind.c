#include "escrita_bin_ind.h"

void escreve_cabecalho_ind(FILE* arq_indices, cabecalho_indice_t* dados_cabecalho) {
    fwrite(&(dados_cabecalho->status), sizeof(char), 1, arq_indices);
    fwrite(&(dados_cabecalho->nro_reg), sizeof(int), 1, arq_indices);
}

void escreve_dado_int(FILE* arq_indices, dados_int_t* dado_int) {
    fwrite(&(dado_int->chaveBusca), sizeof(int), 1, arq_indices);
    fwrite(&(dado_int->byteOffset), sizeof(long long int), 1, arq_indices);
}

void escreve_dado_str(FILE* arq_indices, dados_str_t* dado_str) {
    fwrite(dado_str->chaveBusca, sizeof(char), 12, arq_indices);
    fwrite(&(dado_str->byteOffset), sizeof(long long int), 1, arq_indices);
}

void escreve_dados_int(FILE* arq_indices, dados_int_t** vetor_dados_int, int tamanho_vetor) {
    for (int i = 0; i < tamanho_vetor; i++)
        escreve_dado_int(arq_indices, vetor_dados_int[i]);
}

void escreve_dados_str(FILE* arq_indices, dados_str_t** vetor_dados_str, int tamanho_vetor) {
    for (int i = 0; i < tamanho_vetor; i++)
        escreve_dado_str(arq_indices, vetor_dados_str[i]);
}

void escreve_dados_gen(FILE* arq_indices, void** vetor_dados, int tipoVar, int tamanho_vetor) {
    if (tipoVar == 0)
        escreve_dados_int(arq_indices, (dados_int_t**)vetor_dados, tamanho_vetor);
    else
        escreve_dados_str(arq_indices, (dados_str_t**)vetor_dados, tamanho_vetor);
}

void escreve_arq_ind(FILE* arq_bin, FILE* arq_ind, char* nome_campo, char* tipo_campo, int nro_registros, long long int cab_byteOffset) {

    cabecalho_indice_t* cabecalho_ind = cria_cabecalho_indice('0', 0);
    if (cabecalho_ind == NULL) {
        erro();
        return;
    }
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);

    long long int byteOffset = TAMANHO_CABECALHO;

    int tipoVar = get_tipo_var(tipo_campo);
    void** vetor_dados = NULL;
    vetor_dados = (void**) malloc(nro_registros * get_tam_var(tipoVar));
    if (vetor_dados == NULL) {
        erro();
        free(cabecalho_ind);
        return;
    }

    void* dado_atual;
    crime_t* crime_atual;

    while (byteOffset < cab_byteOffset) {
        crime_atual = le_crime_bin(arq_bin);

        if (crime_atual == NULL) {
            // em caso de erro de alocação
            free(cabecalho_ind);
            if (vetor_dados != NULL) libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg-1);
            erro();
            return;
        }

        if (crime_atual->removido != '1') { 
            dado_atual = pega_dado_generico(crime_atual, nome_campo, byteOffset, tipoVar);
            if (dado_atual == NULL) {
                libera_crime(crime_atual);
                free(cabecalho_ind);
                libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg-1);
                erro();
                return;
            }
            if (checa_dado_nulo(dado_atual, tipoVar)) {
                free(dado_atual);
                byteOffset += crime_atual->tamanho_real;
                libera_crime(crime_atual);
                continue;
            }
            vetor_dados[cabecalho_ind->nro_reg++] = dado_atual;
        }
        
        byteOffset += crime_atual->tamanho_real;
        libera_crime(crime_atual); // libera memória alocada
    }

    ordena_dados_gen(vetor_dados, tipoVar, cabecalho_ind->nro_reg);
    escreve_dados_gen(arq_ind, vetor_dados, tipoVar, cabecalho_ind->nro_reg);
    libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg-1);

    // volta ao início do binário
    desloca_offset(arq_ind, 0);
    cabecalho_ind->status = '1'; // indica que terminou de escrever
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);
    free(cabecalho_ind);
    
}

void remove_com_shift(void*** dados, int tipoVar, int* num_indices, int ind_removido, cabecalho_indice_t* cabecalho_indice) {
    if (ind_removido >= 0) {
        cabecalho_indice->nro_reg--;
        remove_dado(dados, tipoVar, num_indices, ind_removido);
    }
}