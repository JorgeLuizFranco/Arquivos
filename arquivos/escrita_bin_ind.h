#ifndef ESCRITA_BIN_IND_H
    #define ESCRITA_BIN_IND_H

    #include <stdio.h>
    #include "../indices.h"
    #include "../extras/utils.h"
    #include "leitura_bin.h"

    void escreve_arq_ind(FILE* arq_bin, FILE* arq_ind, char* nome_campo, char* tipo_campo, int nro_registros);

#endif
