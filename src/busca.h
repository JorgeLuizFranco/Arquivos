#ifndef BUSCA_H
    #define BUSCA_H

    // comentado no .c

    #include "./extras/utils.h"
    #include "./arquivos/leitura_bin.h"
    #include "./arquivos/escrita_bin.h"
    #include "./arquivos/leitura_bin_ind.h"
    #include "./arquivos/escrita_bin_ind.h"
    #include "./io/io_crimes.h"
    #include "./structs/registros.h"
    #include "./structs/indices.h"
    #include "./structs/campos.h"
    #include <string.h>

    void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var, int flag_dinamica);
    int indice_procura_registro(crime_t* crime_atual, long long int offset_atual, void** dados, int num_dados, char* nome_campo, int tipoVar);

#endif 
