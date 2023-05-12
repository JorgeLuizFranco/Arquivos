#ifndef LEITURA_BIN_IND_H
    #define LEITURA_BIN_IND_H

    #include "../structs/indices.h"
    #include "../extras/utils.h"

    int le_arq_indices(FILE* arq_indices, void*** dados, int tipoVar, cabecalho_indice_t** cabecalho_indice, int* num_ind);

#endif
