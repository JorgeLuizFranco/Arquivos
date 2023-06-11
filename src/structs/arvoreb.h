#ifndef ARVOREB_H
#define ARVOREB_H

#include "indices.h"
#include <stdlib.h>

static const int ORDEM_ARVORE = 5;
static const int LIXO_CABECALHO = 59;
static const int TAMANHO_PAGINA_ARVB = 76;

typedef struct No no_t;
struct No {
    int ponteiro[ORDEM_ARVORE];
    dados_int_t chaves[ORDEM_ARVORE - 1];
    int num_chaves;
    int nivel;
};

typedef struct Cabecalho_Arvoreb cab_arvb_t;
struct Cabecalho_Arvoreb {
    char status;
    int noRaiz;
    int proxRRN;
    int nroNiveis;
    int nroChaves;
};

#endif
