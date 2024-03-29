#ifndef INDICES_H
#define INDICES_H

// mais detalhes no .c

#include "./registros.h"
#include "./utils_structs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// definição de struct de cabeçalho de arquivo de índices
typedef struct Cabecalho_indice cabecalho_indice_t;
struct Cabecalho_indice {
    char status;
    int nro_reg;
};

cabecalho_indice_t* cria_cabecalho_indice(char status, int nro_reg);

// struct de registro de índice do tipo inteiro
typedef struct Dados_inteiro dados_int_t;
struct Dados_inteiro {
    int chaveBusca;
    long long int byteOffset;
};

void ordena_dados_int(dados_int_t** vetor_dados_int, int tamanho_vetor);
dados_int_t* cria_dados_int(int chaveBusca, long long int byteOffset);

// struct de registro de índice do tipo string
typedef struct Dados_string dados_str_t;
struct Dados_string {
    char chaveBusca[12];
    long long int byteOffset;
};

void ordena_dados_str(dados_str_t** vetor_dados_str, int tamanho_vetor);
dados_str_t* cria_dados_str(char* chaveBusca, int limitacao, long long int byteOffset);

void ordena_dados_gen(void** vetor_dados, int tipoVar, int tamanho_vetor);

int compara_chave_busca(void* generico_esq, void* generico_dir, int flag_modo, int tipo_var,
                        int flag_dinamica);

dados_int_t* pega_dado_int(crime_t* crime_atual, char* nome_campo, long long int byteOffset);

dados_str_t* pega_dado_str(crime_t* crime_atual, char* nome_campo, long long int byteOffset);

void* pega_dado_generico(crime_t* crime_atual, char* nome_campo, long long int byteOffset,
                         int tipoVar);

void* pega_chave_generico(void* dado_generico, int tipoVar);

long long int pega_offset_generico(void* dado_generico, int tipoVar);

void copia_dado_gen(void* dest_dado, void* src_dado, int tipoVar);

void remove_dado(void*** vetor_dados, int tipoVar, int* tam_vetor, int pos_dado);

int get_tipo_var(char* nome_tipo);

size_t get_tam_var(int tipoVar);

int compara_dados_gen(void* esquerda, void* direita, int tipoVar);

int checa_dado_nulo(void* dado, int tipoVar);

void seta_offset_gen(void* dado, int tipoVar, long long int offset);
#endif
