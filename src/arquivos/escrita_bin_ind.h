#ifndef ESCRITA_BIN_IND_H
    #define ESCRITA_BIN_IND_H

    // escreve .c

    #include <stdio.h>
    #include "../structs/indices.h"
    #include "../extras/utils.h"
    #include "leitura_bin.h"

    void escreve_arq_ind(FILE* arq_bin, FILE* arq_ind, char* nome_campo, char* tipo_campo, int nro_registros, long long int byteOffset_reg);
    void escreve_dados_gen(FILE* arq_indices, void** vetor_dados, int tipoVar, int tamanho_vetor);
    void escreve_cabecalho_ind(FILE* arq_indices, cabecalho_indice_t* dados_cabecalho);
    void remove_com_shift(void*** dados, int tipoVar, int* num_indices, int ind_removido, cabecalho_indice_t* cabecalho_indice);
    void seta_consistencia_ind(FILE* arq_ind, cabecalho_indice_t* cabecalho_indice, char consistencia);
    
#endif
