#ifndef LEITURA_BIN_H
    #define LEITURA_BIN_H

    #include <stdio.h>
    #include "../registros.h"

    cabecalho_t* le_cabecalho_bin(FILE* arq_bin);
    crime_t* le_crime_bin(FILE* arq_bin);
    crime_t* le_crime_bin_offset(FILE* arq_bin, long long int byteOffset);

#endif