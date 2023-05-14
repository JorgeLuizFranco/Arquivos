#ifndef INSERCAO_H
    #define INSERCAO_H
    // comentado no .c
    #include <stdio.h>
    #include "./structs/registros.h"
    #include "./extras/utils.h"
    #include "./arquivos/leitura_bin.h"
    #include "./arquivos/leitura_bin_ind.h"
    #include "./arquivos/escrita_bin.h"
    #include "./arquivos/escrita_bin_ind.h"
    #include "busca.h"

    int insere_crime_binario(FILE* arq_bin, cabecalho_t* cabecalho, cabecalho_indice_t* cabecalho_indice, char* nome_campo, void*** dados, int tipoVar, int* num_dados, crime_t* crime);
    int insere_dado_ordenado(void*** dados, int tipoVar, int* num_dados, void* dado);
    
#endif
