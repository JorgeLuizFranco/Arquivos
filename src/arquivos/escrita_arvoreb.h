#ifndef ESCRITA_ARVOREB_H
#define ESCRITA_ARVOREB_H

#include "../structs/arvoreb.h"
#include "escrita_bin_ind.h"
#include <stdio.h>

void escreve_cab_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb);
void escreve_no_arvb(FILE* arq_arvb, no_t* no, int nivel);

#endif