#include "./arvoreb.h"

/**
 * Dado um nó, essa função retorna o menor i tal que no_atual->chaves[i].chaveBusca >=
 * chave->chaveBusca com uma busca binária. Se não houver esse 'i', retorna -1.
 * Note que isso é possível porque as chaves são armazenadas de maneira crescente
 *
 * @param no_atual nó em questão
 * @param chave chave em questão
 * @return -1 se todas as chaves do nó são estritamente menores que a chave buscada. Ou então um i
 * maior igual a 0 e menor que o número de chaves do nó atual; e tal que é o menor possível tal que
 * no_atual->chaves[i].chaveBusca >= chave->chaveBusca
 */
int busca_bin_no(no_t* no_atual, dados_int_t* chave) {

    // Algoritmo clássico de busca binária
    // Foi utilizada a função de comparação de chaves de busca, implementada no trabalho anterior
    int baixo = 0;
    int alto = no_atual->num_chaves - 1;
    int meio;
    int resultado = -1;

    while (baixo <= alto) {
        meio = (baixo + alto) / 2;
        if (compara_chave_busca(&(no_atual->chaves[meio]), &(chave->chaveBusca), 1, 0, 0) == 1) {
            alto = meio - 1;
            resultado = meio;
        } else {
            baixo = meio + 1;
        }
    }

    return resultado;
}

/**
 * Apenas função para auxiliar a debugar o código. Printa os dados de um nó
 *
 * @param no nó a ser printado
 */
void debuga_no(no_t* no) {
    fprintf(stderr, "O no atual tem nivel %d e %d chaves:\n", no->nivel, no->num_chaves);

    for (int i = 0; i < no->num_chaves; i++) {
        fprintf(stderr, "(%d) chave=%d ", no->ponteiro[i], no->chaves[i].chaveBusca);
    }
    fprintf(stderr, "(%d)\n", no->ponteiro[no->num_chaves]);
}

/**
 * Seta como -1 todos os byte offset e chaves de busca de um nó de todos os índices de ini ate end
 *
 * @param no nó para setar chaves como -1
 * @param ini índice inicial
 * @param fim índice final
 */
void setam1_chaves(no_t* no, int ini, int fim) {
    for (int i = ini; i <= fim; i++) {
        no->chaves[i].byteOffset = -1;
        no->chaves[i].chaveBusca = -1;
    }
}

/**
 * Seta como -1 todos os rrn de um nó de todos os índices de ini ate end
 *
 * @param no nó para setar os rrn (ponteiros) como -1
 * @param ini índice inicial
 * @param fim índice final
 */
void setam1_ponts(no_t* no, int ini, int fim) {
    for (int i = ini; i <= fim; i++) {
        no->ponteiro[i] = -1;
    }
}

/**
 * Cria (aloca) em RAM um novo nó. Esse nó começa com nível 1 e zero chaves
 * e todas as chaves e ponteiros da struct começam com -1
 *
 * @return ponteiro para no alocado. NULL se der ruim
 */
no_t* aloca_no() {
    no_t* no = (no_t*)malloc(sizeof(no_t));
    if (no == NULL) return NULL;

    no->nivel = 1;
    no->num_chaves = 0;

    setam1_chaves(no, 0, ORDEM_ARVORE - 1);
    setam1_ponts(no, 0, ORDEM_ARVORE);

    return no;
}
