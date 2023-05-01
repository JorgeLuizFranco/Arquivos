#include "escrita_bin_ind.h"

void escreve_cabecalho_ind(FILE* arq_indices, cabecalho_indice_t* dados_cabecalho) {
    fwrite(&(dados_cabecalho->status), sizeof(char), 1, arq_indices);
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

dados_int_t* pega_dado_int(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_int_t* dado_atual = NULL;

    switch (nome_campo[0]) {
        // idCrime
        case 'i':
            dado_atual = cria_dados_int(crime_atual->idCrime, byteOffset);
            break;
        // numeroArtigo
        case 'n':
            dado_atual = cria_dados_int(crime_atual->numeroArtigo, byteOffset);
            break;
    }

    return dado_atual;
}

dados_str_t* pega_dado_str(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_str_t* dado_atual = NULL;

    switch (nome_campo[2]) {
        // dataCrime
        case 't':
            dado_atual = cria_dados_str(crime_atual->dataCrime, byteOffset);
            break;
        // marcaCelular
        case 'r':
            dado_atual = cria_dados_str(crime_atual->marcaCelular, byteOffset);
            break;
        // lugarCrime
        case 'g':
            dado_atual = cria_dados_str(crime_atual->lugarCrime, byteOffset);
            break;
        // descricaoCrime
        case 's':
            dado_atual = cria_dados_str(crime_atual->descricaoCrime, byteOffset);
            break;
    }

    return dado_atual;
}

void escreve_arq_ind(FILE* arq_bin, FILE* arq_ind, char* nome_campo, char* tipo_campo, int nro_registros, long long int cab_byteOffset) {

    int copia_nro_reg = nro_registros;

    cabecalho_indice_t* cabecalho_ind = cria_cabecalho_indice('0');
    if (cabecalho_ind == NULL) {
        erro();
        return;
    }
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);

    long long int byteOffset = TAMANHO_CABECALHO;

    dados_int_t** vetor_dados_int = NULL;
    dados_str_t** vetor_dados_str = NULL;

    switch (tipo_campo[0]) {
        case 'i':
            vetor_dados_int = (dados_int_t**) malloc(nro_registros * sizeof(dados_int_t*));
            if (vetor_dados_int == NULL) {
                erro();
                free(cabecalho_ind);
                return;
            }
            break;
        case 's':
            vetor_dados_str = (dados_str_t**) malloc(nro_registros * sizeof(dados_str_t*));
            if (vetor_dados_str == NULL) {
                erro();
                free(cabecalho_ind);
                return;
            }
            break;
    }

    dados_int_t* dado_atual_int;
    dados_str_t* dado_atual_str;
    crime_t* crime_atual;

    int nro_reg_str = 0;

    while (byteOffset < cab_byteOffset) {
        crime_atual = le_crime_bin(arq_bin);
        if (crime_atual == NULL) {
            // em caso de erro de alocação
            free(cabecalho_ind);
            if (vetor_dados_int != NULL) libera_vetor_ate_pos((void**)vetor_dados_int, copia_nro_reg - nro_registros - 1);
            if (vetor_dados_str != NULL) libera_vetor_ate_pos((void**)vetor_dados_str, copia_nro_reg - nro_registros - 1);
            erro();
            return;
        }

        if (crime_atual->removido != '1') { 
            switch (tipo_campo[0]) {
                case 'i':
                    dado_atual_int = pega_dado_int(crime_atual, nome_campo, byteOffset);
                    if (dado_atual_int == NULL) {
                        libera_crime(crime_atual);
                        free(cabecalho_ind);
                        libera_vetor_ate_pos((void**)vetor_dados_int, copia_nro_reg - nro_registros - 1);
                    }
                    vetor_dados_int[copia_nro_reg - nro_registros] = dado_atual_int;
                    break;
                case 's':
                    dado_atual_str = pega_dado_str(crime_atual, nome_campo, byteOffset);
                    if (dado_atual_str == NULL) {
                        libera_crime(crime_atual);
                        free(cabecalho_ind);
                        libera_vetor_ate_pos((void**)vetor_dados_str, nro_reg_str-1);
                    }
                    if (dado_atual_str->chaveBusca[0] == '$') {
                        free(dado_atual_str);
                        byteOffset += tamanho_crime(crime_atual);
                        libera_crime(crime_atual);
                        continue;
                    }
                    vetor_dados_str[nro_reg_str++] = dado_atual_str;
                    break;
            }

            nro_registros--;
        }
        
        byteOffset += tamanho_crime(crime_atual);
        libera_crime(crime_atual); // libera memória alocada
    }
    switch (tipo_campo[0]) {
        case 'i':
            ordena_dados_int(vetor_dados_int, copia_nro_reg);
            escreve_dados_int(arq_ind, vetor_dados_int, copia_nro_reg);
            break;
        case 's':
            ordena_dados_str(vetor_dados_str, nro_reg_str);
            escreve_dados_str(arq_ind, vetor_dados_str, nro_reg_str);
            break;
    }

    // volta ao início do binário
    fseek(arq_ind, 0, SEEK_SET);
    cabecalho_ind->status = '1'; // indica que terminou de escrever
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);
    free(cabecalho_ind);
    if (vetor_dados_int != NULL) libera_vetor_ate_pos((void**)vetor_dados_int, copia_nro_reg - 1);
    if (vetor_dados_str != NULL) libera_vetor_ate_pos((void**)vetor_dados_str, copia_nro_reg - 1);
}
