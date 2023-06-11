#include "insercao.h"

/**
 * Dado um void* dado (registro de índice genérico), insere ele numa lista de dados
 * genéricos mantendo a ordenação
 * @param dados lista de dados genericos
 * @param tipoVar tipo desses dados
 * @param num_dados (ponteiro para o) número de dados desse tipo
 * @param dado dado genérico a ser inserido
 * @return retorna 0 se houve erro de alocação ou 1 se houve sucesso
 */
int insere_dado_ordenado(void*** dados, int tipoVar, int* num_dados, void* dado) {
    // dá realloc na lista enquanto incrementa *num_dados
    void** dados_realloc = (void**)realloc(*dados, (++(*num_dados)) * get_tam_var(tipoVar));
    if (dados_realloc == NULL) return 0;
    *dados = dados_realloc;

    for (int i = (*num_dados) - 2; i >= 0; i--) {
        // eh para o dado estar nessa posicao?
        if (compara_dados_gen(dado, (*dados)[i], tipoVar) > 0) {
            (*dados)[i + 1] = dado;
            return 1;
        }
        (*dados)[i + 1] = (*dados)[i];
    }

    (*dados)[0] = dado;

    return 1;
}

/**
 *
 * Núcleo da funcionalidade 6. Insere um registro criminal no arquivo binário e atualiza o arquivo
 * de índices
 * @param arq_bin arquivo binario de registros
 * @param cabecalho cabecalho desse arquivo
 * @param cabecalho_indice cabecalho do arquivo de indices
 * @param nome_campo nome do campo que o arquivo de índices usou
 * @param dados lista de dados genericos
 * @param tipoVar tipo de variável
 * @param num_dados ponteiro para o número de registros
 * @param crime registro criminal
 * @return 0 se ocorreu algum erro de alocação; 1 se deu tudo certo
 */
int insere_crime_binario(FILE* arq_bin, cabecalho_t* cabecalho,
                         cabecalho_indice_t* cabecalho_indice, char* nome_campo, void*** dados,
                         int tipoVar, int* num_dados, crime_t* crime) {

    // cria um dado para o arquivo de indices com a chaveBusca especificada pelo campo "nome_campo"
    // do registro crime e byteOffset com o byteoffset que sera inserido
    void* dado = pega_dado_generico(crime, nome_campo, cabecalho->proxByteOffset, tipoVar);
    if (dado == NULL) return 0;

    // se o campo nome_campo do crime for NULL, não preciso pôr ele no arquivo de índices
    if (checa_dado_nulo(dado, tipoVar)) {
        // basta escrever no arquivo binário
        escreve_registro_criminal(arq_bin, crime);
        cabecalho->nroRegArq++;
        cabecalho->proxByteOffset += tamanho_crime(crime);
        return 1;
    }

    // preciso inserir o dado genérico na lista de dados genéricos
    if (insere_dado_ordenado(dados, tipoVar, num_dados, dado) == 0) {
        free(dado);
        return 0;
    }
    cabecalho_indice->nro_reg++;

    // depois de tudo, escreve no arquivo binário o crime
    escreve_registro_criminal(arq_bin, crime);
    cabecalho->nroRegArq++;
    cabecalho->proxByteOffset += tamanho_crime(crime);

    return 1;
}