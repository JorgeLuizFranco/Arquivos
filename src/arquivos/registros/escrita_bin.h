#ifndef ESCRITA_BIN_H
#define ESCRITA_BIN_H

// explicacao no .c

#include "../../extras/utils.h"
#include "../../structs/registros.h"
#include <stdio.h>

void escreve_cabecalho(FILE* arq_bin, cabecalho_t* cabecalho);
void escreve_registro_criminal(FILE* arq_bin, crime_t* crime);
void remocao_logica(FILE* arq_bin, crime_t* crime, cabecalho_t* cabecalho,
                    long long int byteOffset);
void sobrescreve_crime(FILE* arq_bin, long long int byteOffset, int tamanho_crime_antigo,
                       crime_t* crime_novo);
void insere_no_final(FILE* arq_bin, cabecalho_t* cabecalho, crime_t* novo_crime);
void seta_consistencia_bin(FILE* arq_bin, cabecalho_t* cabecalho, char consistencia);

#endif
