#ifndef LEITURA_BIN_H
    #define LEITURA_BIN_H

    // explicacao no .c

    #include <stdio.h>
    #include "../structs/registros.h"
    #include "../extras/utils.h"

    cabecalho_t* le_cabecalho_bin(FILE* arq_bin);
    crime_t* le_crime_bin(FILE* arq_bin);
    crime_t* le_crime_bin_offset(FILE* arq_bin, long long int byteOffset);

#endif