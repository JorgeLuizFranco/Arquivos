#include "./escrita_bin_ind.h"

/**
 * escreve cabeçalho do arquivo de índices
 *
 * @param arq_indices ponteiro para o arquivo de índices
 * @param arq_ind ponteiro para a struct de cabeçalho
 */
void escreve_cabecalho_ind(FILE* arq_indices, cabecalho_indice_t* dados_cabecalho) {
    fwrite(&(dados_cabecalho->status), sizeof(char), 1, arq_indices);
    fwrite(&(dados_cabecalho->nro_reg), sizeof(int), 1, arq_indices);
}

/**
 * escreve registro de indices do tipo inteiro em arquivo de indices
 *
 * @param arq_indices
 * @param dado_int
 */
void escreve_dado_int(FILE* arq_indices, dados_int_t* dado_int) {
    fwrite(&(dado_int->chaveBusca), sizeof(int), 1, arq_indices);
    fwrite(&(dado_int->byteOffset), sizeof(long long int), 1, arq_indices);
}

/**
 * escreve registro de indices do tipo string em arquivos de indices
 *
 * @param arq_indices
 * @param dado_str
 */
void escreve_dado_str(FILE* arq_indices, dados_str_t* dado_str) {
    fwrite(dado_str->chaveBusca, sizeof(char), 12, arq_indices);
    fwrite(&(dado_str->byteOffset), sizeof(long long int), 1, arq_indices);
}

/**
 * escreve array de indices do tipo int em arquivo de indices
 *
 * @param arq_indices
 * @param vetor_dados_int
 * @param tamanho_vetor
 */
void escreve_dados_int(FILE* arq_indices, dados_int_t** vetor_dados_int, int tamanho_vetor) {
    for (int i = 0; i < tamanho_vetor; i++)
        escreve_dado_int(arq_indices, vetor_dados_int[i]);
}

/**
 *
 * escreve array de indices do tipo string em arquivo de indices
 *
 * @param arq_indices
 * @param vetor_dados_str
 * @param tamanho_vetor
 */
void escreve_dados_str(FILE* arq_indices, dados_str_t** vetor_dados_str, int tamanho_vetor) {
    for (int i = 0; i < tamanho_vetor; i++)
        escreve_dado_str(arq_indices, vetor_dados_str[i]);
}

/**
 * escreve array de dados genericos em arquivo de indices
 *
 * @param arq_indices
 * @param vetor_dados
 * @param tipoVar
 * @param tamanho_vetor
 */
void escreve_dados_gen(FILE* arq_indices, void** vetor_dados, int tipoVar, int tamanho_vetor) {
    if (tipoVar == 0)
        escreve_dados_int(arq_indices, (dados_int_t**)vetor_dados, tamanho_vetor);
    else
        escreve_dados_str(arq_indices, (dados_str_t**)vetor_dados, tamanho_vetor);
}

/**
 * Escreve o arquivo de índices
 *
 * @param arq_bin ponteiro para o arquivo de dados
 * @param arq_ind ponteiro para o arquivo de índices
 * @param nome_campo string que define nome do campo ( ex: idCrime)
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
 * @param nro_registros numero de registros
 * @param byteOffset_reg byteOffset do arquivo
 */

void escreve_arq_ind(FILE* arq_bin, FILE* arq_ind, char* nome_campo, char* tipo_campo,
                     int nro_registros, long long int byteOffset_reg) {

    // cria cabeçalho com status '0' e 0 registros
    cabecalho_indice_t* cabecalho_ind = cria_cabecalho_indice('0', 0);
    if (cabecalho_ind == NULL) {
        erro();
        return;
    }
    escreve_cabecalho_ind(arq_ind, cabecalho_ind);

    long long int byteOffset = TAMANHO_CABECALHO;

    int tipoVar = get_tipo_var(tipo_campo); // pega o inteiro correspondente ao tipo
    void** vetor_dados = NULL;
    vetor_dados =
        (void**)malloc(nro_registros * get_tam_var(tipoVar)); // cria vetor de dados baseado no tipo
    if (vetor_dados == NULL) {
        erro();
        free(cabecalho_ind);
        return;
    }

    void* dado_atual;
    crime_t* crime_atual;
    while (byteOffset <
           byteOffset_reg) { // vai lendo crime por crime ate chegar no offset do arquivo
        crime_atual = le_crime_bin(arq_bin);
        if (crime_atual == NULL) {
            // em caso de erro de alocação
            free(cabecalho_ind);
            if (vetor_dados != NULL) libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg - 1);
            erro();
            return;
        }

        // se crime nao tiver marcado como removido
        if (crime_atual->removido != '1') {
            // pega o registro de indice correspondente
            dado_atual = pega_dado_generico(crime_atual, nome_campo, byteOffset, tipoVar);
            if (dado_atual == NULL) {
                // erro de alocação
                libera_crime(crime_atual);
                free(cabecalho_ind);
                libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg - 1);
                erro();
                return;
            }
            if (checa_dado_nulo(dado_atual, tipoVar)) {
                // se for nulo, ignora esse
                free(dado_atual);
                byteOffset += crime_atual->tamanho_real;
                libera_crime(crime_atual);
                continue;
            }
            // se não for nulo, coloca no vetor
            vetor_dados[cabecalho_ind->nro_reg++] = dado_atual;
        }

        // vai pro próximo
        byteOffset += crime_atual->tamanho_real;
        libera_crime(crime_atual); // libera memória alocada
    }

    // ordena vetor de dados, escreve-o no arquivo
    ordena_dados_gen(vetor_dados, tipoVar, cabecalho_ind->nro_reg);
    escreve_dados_gen(arq_ind, vetor_dados, tipoVar, cabecalho_ind->nro_reg);

    // libera vetor de dados
    libera_vetor_ate_pos(vetor_dados, cabecalho_ind->nro_reg - 1);

    // volta ao início do arquivo de índices e seta consistencia e conserta
    // numero de registros
    seta_consistencia_ind(arq_ind, cabecalho_ind, '1');

    free(cabecalho_ind);
}

/**
 * Remove indice de vetor de dados se >= 0
 *
 * @param dados
 * @param tipoVar
 * @param num_indices
 * @param ind_removido
 * @param cabecalho_indice
 */
void remove_com_shift(void*** dados, int tipoVar, int* num_indices, int ind_removido,
                      cabecalho_indice_t* cabecalho_indice) {
    if (ind_removido >= 0) {
        cabecalho_indice->nro_reg--;
        remove_dado(dados, tipoVar, num_indices, ind_removido);
    }
}

/**
 * Seta consistencia do cabecalho
 *
 * @param arq_bin
 * @param cabecalho
 * @param consistencia
 */
void seta_consistencia_ind(FILE* arq_ind, cabecalho_indice_t* cabecalho_indice, char consistencia) {
    desloca_offset(arq_ind, 0);
    cabecalho_indice->status = consistencia;
    escreve_cabecalho_ind(arq_ind, cabecalho_indice);
}