#ifndef CONSULTAS_H
#define CONSULTAS_H

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
#include "./atualizacao.h"
#include "./busca.h"

void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx,
                       int num_consultas, int funcionalidade);

#endif
