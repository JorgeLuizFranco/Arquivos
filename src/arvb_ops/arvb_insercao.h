#ifndef ARVB_INSERCAO_H
#define ARVB_INSERCAO_H

#include "../arquivos/arvb/escrita_arvoreb.h"
#include "../arquivos/arvb/leitura_arvoreb.h"
#include "../structs/arvoreb.h"
#include "./arvb_busca.h"

int insere_crime_arvb(FILE* arq_arv_b, cab_arvb_t* cab_arvb, char* nome_campo, crime_t* crime,
                      long long int byteOffset);

#endif
