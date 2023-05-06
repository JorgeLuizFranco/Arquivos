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

    int copia_nro_reg = nro_registros;

    cabecalho_indice_t* cabecalho_ind = cria_cabecalho_indice('0', 0);
    if (cabecalho_ind == NULL) {
        erro();
        return;
    }
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);

    long long int byteOffset = TAMANHO_CABECALHO;

    int tipoVar = (tipo_campo[0] == 'i') ? 0 : 1;
    void** vetor_dados = NULL;
    vetor_dados = (void**) malloc(nro_registros * (tipoVar == 0 ? sizeof(dados_int_t*) : sizeof(dados_str_t*)));
    if (vetor_dados == NULL) {
        erro();
        free(cabecalho_ind);
        return;
    }

    void* dado_atual;
    crime_t* crime_atual;

    int nro_reg_str = 0;

    while (byteOffset < cab_byteOffset) {
        crime_atual = le_crime_bin(arq_bin);
        if (crime_atual == NULL) {
            // em caso de erro de alocação
            free(cabecalho_ind);
            if (vetor_dados != NULL) libera_vetor_ate_pos(vetor_dados, copia_nro_reg - nro_registros - 1);
            erro();
            return;
        }

        if (crime_atual->removido != '1') { 
            cabecalho_ind->nro_reg++;
            dado_atual = pega_dado_generico(crime_atual, nome_campo, byteOffset, tipoVar);
            if (dado_atual == NULL) {
                libera_crime(crime_atual);
                free(cabecalho_ind);
                libera_vetor_ate_pos(vetor_dados, copia_nro_reg - nro_registros - 1);
            }
            if (tipoVar == 1) {
                if (((char*)pega_chave_generico(dado_atual, 1))[0] == '$') {
                    free(dado_atual);
                    byteOffset += tamanho_crime(crime_atual);
                    libera_crime(crime_atual);
                }
                vetor_dados[nro_reg_str++] = dado_atual;
            } else {
                vetor_dados[copia_nro_reg - nro_registros] = dado_atual;
            }
            vetor_dados[copia_nro_reg - nro_registros] = dado_atual;
            nro_registros--;
        }
        
        byteOffset += tamanho_crime(crime_atual);
        libera_crime(crime_atual); // libera memória alocada
    }

    ordena_dados_gen(vetor_dados, tipoVar, tipoVar == 0 ? copia_nro_reg : nro_reg_str);
    escreve_dados_gen(arq_ind, vetor_dados, tipoVar, tipoVar == 0 ? copia_nro_reg : nro_reg_str);

    // volta ao início do binário
    fseek(arq_ind, 0, SEEK_SET);
    cabecalho_ind->status = '1'; // indica que terminou de escrever
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);
    free(cabecalho_ind);
    libera_vetor_ate_pos(vetor_dados, copia_nro_reg-1);
}
