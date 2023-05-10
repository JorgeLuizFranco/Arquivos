#ifndef BUSCA_H
    #define BUSCA_H

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
    void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade);

#endif 
