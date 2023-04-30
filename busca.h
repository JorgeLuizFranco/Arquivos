#ifndef BUSCA_H
    #define BUSCA_H

    #include "utils.h"
    #include "registros.h"
    #include <string.h>

    typedef struct Campo_busca campo_busca_t;
    struct Campo_busca {
        char campo_busca[20];
        int chaveBuscaInt;
        char chaveBuscaStr[100];
    };

    campo_busca_t** le_campos_busca(int num_campos);
    int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos);

#endif 
