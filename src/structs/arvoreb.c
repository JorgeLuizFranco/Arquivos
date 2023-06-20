#include "./arvoreb.h"

// acha o menor i tal que no_atual->chaves[i].chaveBusca >= chave->chaveBusca
int busca_bin_no(no_t* no_atual, dados_int_t* chave) {
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

void debuga_no(no_t* no) {
    fprintf(stderr, "O no atual tem nivel %d e %d chaves:\n", no->nivel, no->num_chaves);

    for (int i = 0; i < no->num_chaves; i++) {
        fprintf(stderr, "(%d) chave=%d ", no->ponteiro[i], no->chaves[i].chaveBusca);
    }
    fprintf(stderr, "(%d)\n", no->ponteiro[no->num_chaves]);
}

void setam1_chaves(no_t* no, int ini, int end) {
    for (int i = ini; i <= end; i++) {
        no->chaves[i].byteOffset = -1;
        no->chaves[i].chaveBusca = -1;
    }
}

void setam1_ponts(no_t* no, int ini, int end) {
    for (int i = ini; i <= end; i++) {
        no->ponteiro[i] = -1;
    }
}

no_t* aloca_no() {
    no_t* no = (no_t*)malloc(sizeof(no_t));
    if (no == NULL) return NULL;

    no->nivel = 1;
    no->num_chaves = 0;

    setam1_chaves(no, 0, ORDEM_ARVORE - 1);
    setam1_ponts(no, 0, ORDEM_ARVORE);

    return no;
}
