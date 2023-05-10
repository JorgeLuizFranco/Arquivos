#ifndef INSERCAO_H
    #define INSERCAO_H
    
    #include <stdio.h>
    #include "./structs/registros.h"
    #include "./extras/utils.h"
    #include "./arquivos/leitura_bin.h"
    #include "./arquivos/leitura_bin_ind.h"
    #include "./arquivos/escrita_bin.h"
    #include "./arquivos/escrita_bin_ind.h"
    #include "busca.h"

    int insere_crime_binario(FILE* arq_bin, char* nome_campo, void*** dados, int tipoVar, int* num_dados, long long int byteOffset, crime_t* crime);

#endif
