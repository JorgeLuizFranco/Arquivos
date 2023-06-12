#ifndef UTILS_H
#define UTILS_H

#include "../arquivos/arvb/leitura_arvoreb.h"
#include "../arquivos/indice/leitura_bin_ind.h"
#include "../arquivos/registros/leitura_bin.h"
#include "../structs/arvoreb.h"
#include "../structs/campos.h"
#include "../structs/indices.h"
#include "../structs/registros.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

// explicacao no .c

// Função fornecida
// Alterei para pegar como entrada o próprio ponteiro para arquivo
void binarioNaTela(char* nomeArquivoBinario);

void scan_quote_string(char* str);

// Imprime na tela que ocorreu erro
void erro();

void libera_vetor_ate_pos(void** vetor, int pos);

void libera_memo_consultas(int flag_erro, FILE* arq_bin, cabecalho_t* cabecalho, FILE* arq_idx,
                           cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos,
                           int num_campos, void** dados, int num_dados);

void desloca_offset(FILE* arq_bin, long long int byteOffset);

int abre_arq_bin_ind(FILE** arq_bin, char* nome_arq_bin, FILE** arq_idx, char* nome_arq_idx,
                     cabecalho_t** cabecalho, cabecalho_indice_t** cabecalho_indice, void*** dados,
                     int tipoVar, int* num_ind);

int abre_arq_bin_arv(FILE** arq_bin, char* nome_arq_bin, FILE** arq_arv, char* nome_arq_arv,
                     cabecalho_t** cab_arq_bin, cab_arvb_t** cab_arvb);

void fecha_arquivos(int num_arqs, ...);

void libera_memo(int num_ponts, ...);

void libera_arr(void** arr, int tamanho);

#endif
