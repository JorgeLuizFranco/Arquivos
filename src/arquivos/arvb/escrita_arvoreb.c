#include "./escrita_arvoreb.h"

/**
 * escreve cabeçalho no arquivo de índice arvore B*
 *
 * @param arq_arvb ponteiro para o arquivo
 * @param cab_arvb ponteiro cabeçalho a ser escrito
 *
 */
void escreve_cab_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb) {
    fwrite(&(cab_arvb->status), sizeof(char), 1, arq_arvb);
    fwrite(&(cab_arvb->noRaiz), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->proxRRN), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->nroNiveis), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->nroChaves), sizeof(int), 1, arq_arvb);

    char lixo = '$';
    for (int i = 0; i < LIXO_CABECALHO; i++) {
        fwrite(&lixo, sizeof(char), 1, arq_arvb);
    }
}

/**
 * escreve um no no arquivo de índice árvore B*
 *
 * @param arq_arvb ponteiro para o arquivo
 * @param no ponteiro para o no a ser escrito
 */
void escreve_no_arvb(FILE* arq_arvb, no_t* no) {
    fwrite(&(no->nivel), sizeof(int), 1, arq_arvb);
    fwrite(&(no->num_chaves), sizeof(int), 1, arq_arvb);

    for (int i = 0; i < ORDEM_ARVORE - 1; i++) {
        fwrite(&(no->ponteiro[i]), sizeof(int), 1, arq_arvb);
        fwrite(&((no->chaves[i]).chaveBusca), sizeof(int), 1, arq_arvb);
        fwrite(&((no->chaves[i]).byteOffset), sizeof(long long int), 1, arq_arvb);
    }

    fwrite(&(no->ponteiro[ORDEM_ARVORE - 1]), sizeof(int), 1, arq_arvb);
}

/**
 * Marca como inconsistente ou consistente
 *
 * @param arq_arvb arquivo de indice de arvore B*
 * @param cab_arvb cabeçalho desse arquivo
 * @param consistencia sempre '0' ou '1'
 */
void seta_consistencia_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb, char consistencia) {
    desloca_offset(arq_arvb, 0);
    cab_arvb->status = consistencia;
    escreve_cab_arvb(arq_arvb, cab_arvb);
}

/**
 *
 * Escreve um nó em um rrn especificado
 *
 * @param arq_arvb arquivo de indice de arvore B*
 * @param no ponteiro para nó em RAM a ser escrito
 * @param rrn_no rrn que vai escrever esse nó
 */
void escreve_no_rrn(FILE* arq_arvb, no_t* no, int rrn_no) {
    desloca_offset(arq_arvb, 1LL * TAMANHO_PAGINA_ARVB * (rrn_no + 1));
    escreve_no_arvb(arq_arvb, no);
}
