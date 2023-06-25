#ifndef ARVOREB_H
#define ARVOREB_H

#include "./indices.h"
#include <stdlib.h>

// algumas funções comentadas no .c

#define ORDEM_ARVORE 5

static const int LIXO_CABECALHO = 59; // 59 bytes de lixo $ são armazenados no cabeçalho

// cabeçalho incluindo lixo ocupa 76 bytes. cada nó também.
static const int TAMANHO_PAGINA_ARVB = 76;

typedef struct No no_t; // struct nó que fica em RAM
struct No {
    // essa struct tem 6 ponteiros (rrns) e 5 chaves
    // mas isso é só para auxiliar na hora de implementar a rotina de inserção
    // porque durante a inserção, pode ser que algum nó fique temporariamente com 5 chaves

    int ponteiro[ORDEM_ARVORE + 1]; // +1 pra auxiliar

    // usei struct implementada no trabalho passado
    // essa struct guarda um int (chave) e um long long int (byteOffset)
    dados_int_t chaves[(ORDEM_ARVORE - 1) + 1]; // +1 arqui também

    int num_chaves;
    int nivel;
};

// struct cabeçalho. não precisa guardar os $ em RAM
typedef struct Cabecalho_Arvoreb cab_arvb_t;
struct Cabecalho_Arvoreb {
    char status;
    int noRaiz;
    int proxRRN;
    int nroNiveis;
    int nroChaves;
};

// funções explicadas no .c
int busca_bin_no(no_t* no_atual, dados_int_t* chave);
void debuga_no(no_t* no_atual);

no_t* aloca_no();
void setam1_chaves(no_t* no, int ini, int end);
void setam1_ponts(no_t* no, int ini, int end);

#endif
