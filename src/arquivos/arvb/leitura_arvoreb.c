#include "./leitura_arvoreb.h"

cab_arvb_t* le_cab_arvb(FILE* arq_arvb) {
    cab_arvb_t* cab_arvb = (cab_arvb_t*)malloc(sizeof(cab_arvb));
    if (cab_arvb == NULL) return NULL;

    fread(&(cab_arvb->status), sizeof(char), 1, arq_arvb);
    fread(&(cab_arvb->noRaiz), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->proxRRN), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->nroNiveis), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->nroChaves), sizeof(int), 1, arq_arvb);

    char lixo;
    for (int i = 0; i < LIXO_CABECALHO; i++) {
        fread(&lixo, sizeof(char), 1, arq_arvb);
    }

    return cab_arvb;
}

no_t* le_no_arvb(FILE* arq_arvb) {
    no_t* no = (no_t*)malloc(sizeof(no_t));
    if (no == NULL) return NULL;

    fread(&(no->nivel), sizeof(int), 1, arq_arvb);
    fread(&(no->num_chaves), sizeof(int), 1, arq_arvb);

    for (int i = 0; i < ORDEM_ARVORE - 1; i++) {
        fread(&(no->ponteiro[i]), sizeof(int), 1, arq_arvb);
        fread(&((no->chaves[i]).chaveBusca), sizeof(int), 1, arq_arvb);
        fread(&((no->chaves[i]).byteOffset), sizeof(long long int), 1, arq_arvb);
    }

    fread(&(no->ponteiro[ORDEM_ARVORE - 1]), sizeof(int), 1, arq_arvb);
    setam1_chaves(no, ORDEM_ARVORE - 1, ORDEM_ARVORE - 1);
    setam1_ponts(no, ORDEM_ARVORE, ORDEM_ARVORE);

    return no;
}

long long int byteoffset_no(int rrn_no) { return 1LL * TAMANHO_PAGINA_ARVB * (rrn_no + 1); }

no_t* pega_no_rrn(FILE* arq_arvb, int rrn_no) {
    desloca_offset(arq_arvb, byteoffset_no(rrn_no));
    return le_no_arvb(arq_arvb);
}
