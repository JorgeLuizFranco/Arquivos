#ifndef CAMPOS_H
    #define CAMPOS_H

    #include "registros.h"
    #include "utils_structs.h"
    #include <stdio.h>

    // mais detalhes no .c

    // define registro de campo de busca
    // util para quando usuario esta especificando esses campos de busca
    // na entrada
    typedef struct Campo_busca campo_busca_t;
    struct Campo_busca {
        char campo_busca[20]; // por ex "idCrime" ou "lugarCrime" (etc)
        int chaveBuscaInt; // se for um campo de int como "idCrime", armazena seu valor
        char chaveBuscaStr[100]; // se for campo de string, analogamente
    };

    campo_busca_t** le_campos_busca(int num_campos);
    int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos);
    int atualiza_registro(crime_t* crime_atual, campo_busca_t** atualizacoes, int n_campos);
    int checa_campo_procurado(campo_busca_t** campos, int n_campos, char* nome_campo);
    void le_campo_inteiro(int* inteiro);

#endif
