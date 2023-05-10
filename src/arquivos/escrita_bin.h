#ifndef ESCRITA_BIN_H
    #define ESCRITA_BIN_H

    #include <stdio.h>
    #include "../registros.h"
    #include "../extras/utils.h"

    void escreve_cabecalho(FILE* arq_bin, cabecalho_t* cabecalho);
    void escreve_registro_criminal(FILE* arq_bin, crime_t* crime);
    void remocao_logica(FILE* arq_bin, crime_t* crime, cabecalho_t* cabecalho, long long int byteOffset);
    
#endif
