#ifndef ESCRITA_TELA_H
    #define ESCRITA_TELA_H

    #include <stdio.h>
    #include "../arquivos/leitura_bin.h"
    #include "../extras/utils.h"

    void mostra_crimes_tela_arq(FILE* arq_bin, int n_registros);
    void mostra_crime_tela(crime_t* crime_atual);
    
#endif
