#ifndef ARVB_BUSCA_H
#define ARVB_BUSCA_H

#include "../arquivos/arvb/leitura_arvoreb.h"
#include "../io/io_crimes.h"
#include "../structs/arvoreb.h"

// Funções relacionadas a operações de busca na árvore B*
// comentadas no .c

int busca_chave(FILE* arq_arv, cab_arvb_t* cab_arvb, dados_int_t* chave_procurada, no_t** no,
                int* pos_chave, int* flag_presente);

int busca_arvb(FILE* arq_bin, FILE* arq_arv, cab_arvb_t* cab_arvb, cabecalho_t* cab_arq_bin,
               campo_busca_t** campos, int num_campos, char* nome_campo);

#endif
