#ifndef REGISTROS_H
    #define REGISTROS_H

    #include <stdlib.h>
    #include <string.h>
    #include "./utils_structs.h"

    // Documentação em detalhes no .c, que também possui outras funções
    // que servem de base para as colocadas aqui

    // Cada registro a ser analisado
    typedef struct Crime crime_t;
    struct Crime {
        // Campos fixos
        int idCrime;
        char dataCrime[10];
        int numeroArtigo;
        char marcaCelular[12];
        char removido;

        // Campos variáveis
        char* lugarCrime;
        char* descricaoCrime;

        // Campo auxiliar
        int tamanho_real;
    };

    // registro header
    typedef struct Cabecalho cabecalho_t;
    struct Cabecalho {
        char status;
        long long int proxByteOffset;
        int nroRegArq;
        int nroRegRem;
    };
    static const int TAMANHO_CABECALHO = 4+4+8+1;

    void libera_crime(crime_t* crime_atual);
    int tamanho_crime(crime_t* crime);
    crime_t* copia_crime(crime_t* original);

#endif
