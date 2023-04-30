#ifndef ESCRITA_BIN_H
    #define ESCRITA_BIN_H

    #include <stdio.h>
    #include "../registros.h"

    void escreve_cabecalho(FILE* arq_bin, cabecalho_t* cabecalho);
    void escreve_registro_criminal(FILE* arq_bin, crime_t* crime);

#endif
