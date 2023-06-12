#ifndef LEITURA_BIN_IND_H
#define LEITURA_BIN_IND_H

#include "../../extras/utils.h"
#include "../../structs/indices.h"

int le_arq_indices(FILE* arq_indices, void*** dados, int tipoVar,
                   cabecalho_indice_t** cabecalho_indice, int* num_ind);
void le_dado_gen(FILE* arq_indices, void** dado_gen, int tipoVar);

#endif
