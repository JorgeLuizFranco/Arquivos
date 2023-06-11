#include "escrita_arvoreb.h"

void escreve_cab_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb) {
    fwrite(&(cab_arvb->status), sizeof(char), 1, arq_arvb);
    fwrite(&(cab_arvb->proxRRN), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->nroNiveis), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->noRaiz), sizeof(int), 1, arq_arvb);
    fwrite(&(cab_arvb->nroChaves), sizeof(int), 1, arq_arvb);

    char lixo = '$';
    for (int i = 0; i < LIXO_CABECALHO; i++) {
        fwrite(&lixo, sizeof(char), 1, arq_arvb);
    }
}

void escreve_no_arvb(FILE* arq_arvb, no_t* no, int nivel) {
    fwrite(&nivel, sizeof(int), 1, arq_arvb);
    fwrite(&(no->num_chaves), sizeof(int), 1, arq_arvb);

    for (int i = 0; i < ORDEM_ARVORE - 1; i++) {
        fwrite(&(no->ponteiro[i]), sizeof(int), 1, arq_arvb);
        fwrite(&((no->chaves[i]).chaveBusca), sizeof(int), 1, arq_arvb);
        fwrite(&((no->chaves[i]).byteOffset), sizeof(long long int), 1, arq_arvb);
    }

    fwrite(&(no->ponteiro[ORDEM_ARVORE - 1]), sizeof(int), 1, arq_arvb);
}