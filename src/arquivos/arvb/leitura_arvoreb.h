#ifndef LEITURA_ARVOREB_H
#define LEITURA_ARVOREB_H

#include "../../structs/arvoreb.h"
#include "../indice/leitura_bin_ind.h"

// comentario no .c

no_t* le_no_arvb(FILE* arq_arvb);
cab_arvb_t* le_cab_arvb(FILE* arq_arvb);
long long int byteoffset_no(int rrn_no);
no_t* pega_no_rrn(FILE* arq_arvb, int rrn_no);

#endif
