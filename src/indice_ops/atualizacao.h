#ifndef ATUALIZACAO_H
#define ATUALIZACAO_H

// comentado no .c

#include "../arquivos/indice/escrita_bin_ind.h"
#include "../arquivos/indice/leitura_bin_ind.h"
#include "../arquivos/registros/escrita_bin.h"
#include "../arquivos/registros/leitura_bin.h"
#include "../extras/utils.h"
#include "../io/io_crimes.h"
#include "../structs/campos.h"
#include "../structs/indices.h"
#include "../structs/registros.h"
#include "./busca.h"
#include "./insercao.h"

int atualizar(FILE* arq_bin, cabecalho_t* cabecalho, crime_t* crime_atual, long long int byteOffset,
              FILE* arq_idx, cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos_atualizar,
              int num_campos_atualizar, int flag_campo_atualiza, campo_busca_t** campos,
              int num_campos, char* nome_campo, void** dados, int* num_indices, int tipoVar,
              int* flag_nova_pos);

#endif
