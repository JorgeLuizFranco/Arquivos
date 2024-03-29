#ifndef INSERCAO_H
#define INSERCAO_H
// comentado no .c
#include "../arquivos/indice/escrita_bin_ind.h"
#include "../arquivos/indice/leitura_bin_ind.h"
#include "../arquivos/registros/escrita_bin.h"
#include "../arquivos/registros/leitura_bin.h"
#include "../extras/utils.h"
#include "../structs/registros.h"
#include "./busca.h"
#include <stdio.h>

int insere_crime_binario(FILE* arq_bin, cabecalho_t* cabecalho,
                         cabecalho_indice_t* cabecalho_indice, char* nome_campo, void*** dados,
                         int tipoVar, int* num_dados, crime_t* crime);
int insere_dado_ordenado(void*** dados, int tipoVar, int* num_dados, void* dado);

#endif
