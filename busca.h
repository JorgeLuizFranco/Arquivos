#ifndef BUSCA_H
    #define BUSCA_H

    #include "extras/utils.h"
    #include "arquivos/leitura_bin.h"
    #include "arquivos/escrita_bin.h"
    #include "arquivos/leitura_bin_ind.h"
    #include "arquivos/escrita_bin_ind.h"
    #include "registros.h"
    #include "indices.h"
    #include <string.h>

    typedef struct Campo_busca campo_busca_t;
    struct Campo_busca {
        char campo_busca[20];
        int chaveBuscaInt;
        char chaveBuscaStr[100];
    };

    campo_busca_t** le_campos_busca(int num_campos);
    int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos);
    void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var);
    void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade);

#endif 
