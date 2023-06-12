#ifndef ARVB_BUSCA_H
#define ARVB_BUSCA_H

#include "../arquivos/arvb/leitura_arvoreb.h"
#include "../io/io_crimes.h"
#include "../structs/arvoreb.h"

int busca_arvb(FILE* arq_bin, FILE* arq_arv, cab_arvb_t* cab_arvb, cabecalho_t* cab_arq_bin,
               campo_busca_t** campos, int num_campos, char* nome_campo);

#endif
