#ifndef INDICES_H
    #define INDICES_H

    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include "utils.h"

    typedef struct Cabecalho_indice cabecalho_indice_t;
    struct Cabecalho_indice {
        char status;
    };

    cabecalho_indice_t* cria_cabecalho_indice(char status);

    typedef struct Dados_inteiro dados_int_t;
    struct Dados_inteiro {
        int chaveBusca;
        long long int byteOffset;
    };

    void ordena_dados_int(dados_int_t** vetor_dados_int, int tamanho_vetor);
    dados_int_t* cria_dados_int(int chaveBusca, long long int byteOffset);

    typedef struct Dados_string dados_str_t;
    struct Dados_string {
        char chaveBusca[12];
        long long int byteOffset;
    };

    void ordena_dados_str(dados_str_t** vetor_dados_str, int tamanho_vetor);
    dados_str_t* cria_dados_str(char* chaveBusca, long long int byteOffset);

#endif
