#ifndef LEITURA_BIN_IND_H
    #define LEITURA_BIN_IND_H

    #include "../indices.h"

    void le_arq_indices(FILE* arq_indices, char* tipo_campo, dados_int_t*** dados_int, dados_str_t*** dados_str, cabecalho_indice_t** cabecalho_indice);

#endif
