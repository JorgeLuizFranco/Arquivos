#ifndef FUNCIONALIDADES_H
#define FUNCIONALIDADES_H

#include "./arquivos/arvb/escrita_arvoreb.h"
#include "./arquivos/indice/escrita_bin_ind.h"
#include "./arquivos/indice/leitura_bin_ind.h"
#include "./arquivos/leitura_csv.h"
#include "./arquivos/registros/escrita_bin.h"
#include "./arquivos/registros/leitura_bin.h"
#include "./arvb_ops/arvb_busca.h"
#include "./extras/utils.h"
#include "./indice_ops/consultas.h"
#include "./indice_ops/insercao.h"
#include "./io/io_crimes.h"

// Funcionalidade 1. A partir de um nome de arquivo csv e nome de arquivo binário,
// faz um binário com os registros descritos no csv
void csv_para_bin(char* nome_arq_csv, char* nome_arq_bin);

// Funcionalidade 2. A partir de um nome de arquivo binário, mostra os registros
// de crimes que armazena na tela
void mostra_crimes_tela(char* nome_arq_bin);

// Funcionalidade 3. A partir de um nome de arquivo binário de registros, nome do campo
// e do tipo desse campo, cria um arquivo binário de índices cujo nome é especificado
// pela variável nome_arq_ind
void cria_arq_indices(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_ind);

// Funcionalidade 4. A partir de um nome de arquivo binário de registros, nome de campo
// e do tipo desse campo, nome de um arquivo de índices lineares e de um número de consultas, lê
// da entrada o número especificado de consultas e ,para cada uma delas,
// mostra os registros que satisfazem a consulta, como na especificação do trabalho
void procura_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx,
                       int num_consultas);

// Funcionalidade 5. A partir de um nome de arquivo binário de registros, nome de campo
// e do tipo desse campo, nome de um arquivo de índices lineares e de um número de consultas, lê
// da entrada o número especificado de consultas e ,para cada uma delas,
// remove os registros que satisfazem as consultas
void remove_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx,
                      int num_consultas);

// Funcionalidade 6. A partir de um nome de arquivo binário de registros, nome de campo
// e do tipo desse campo, nome de um arquivo de índices lineares e de um número de consultas, lê
// da entrada o número especificado de consultas e ,para cada uma delas,
// adiciona os registros informados pelo usuário no arquivo binário e no de índices
void insere_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx,
                      int num_consultas);

// Funcionalidade 7. A partir de um nome de arquivo binário de registros, nome de campo
// e do tipo desse campo, nome de um arquivo de índices lineares e de um número de consultas, lê
// da entrada o número especificado de consultas e ,para cada uma delas,
// atualiza os registros informados pelo usuário da maneira como ele especificou
void atualiza_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx,
                        int num_consultas);

// Funcionalidade 9
void arvb_procura_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo,
                            char* nome_arq_arvb, int num_consultas);

// Funcionalidade 10
void arvb_insere_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo,
                           char* nome_arq_arvb, int num_consultas);

#endif
