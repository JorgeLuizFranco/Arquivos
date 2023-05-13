#ifndef ESCRITA_BIN_H
    #define ESCRITA_BIN_H

    #include <stdio.h>
    #include "../structs/registros.h"
    #include "../extras/utils.h"

    void escreve_cabecalho(FILE* arq_bin, cabecalho_t* cabecalho);
    void escreve_registro_criminal(FILE* arq_bin, crime_t* crime);
    void remocao_logica(FILE* arq_bin, crime_t* crime, cabecalho_t* cabecalho, long long int byteOffset);
    void sobrescreve_crime(FILE* arq_bin, long long int byteOffset, int tamanho_crime_antigo, crime_t* crime_novo);
    
#endif
