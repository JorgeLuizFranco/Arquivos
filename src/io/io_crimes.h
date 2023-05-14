#ifndef IO_CRIMES_H
    #define IO_CRIMES_H

    // comentado no .c

    #include <stdio.h>
    #include "../arquivos/leitura_bin.h"
    #include "../extras/utils.h"

    void mostra_crimes_tela_arq(FILE* arq_bin, int n_registros);
    void mostra_crime_tela(crime_t* crime);
    crime_t* le_crime_tela();
    
#endif
