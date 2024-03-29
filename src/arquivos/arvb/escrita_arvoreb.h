#ifndef ESCRITA_ARVOREB_H
#define ESCRITA_ARVOREB_H

// comentado no .c

#include "../../structs/arvoreb.h"
#include "../indice/escrita_bin_ind.h"
#include <stdio.h>

void escreve_cab_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb);
void escreve_no_arvb(FILE* arq_arvb, no_t* no);
void seta_consistencia_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb, char consistencia);
void escreve_no_rrn(FILE* arq_arvb, no_t* no, int rrn_no);

#endif