#include "indices.h"

cabecalho_indice_t* cria_cabecalho_indice(char status) {
    cabecalho_indice_t* cabecalho_indice = (cabecalho_indice_t*) malloc(sizeof(cabecalho_indice_t));
    if (cabecalho_indice == NULL) return NULL;

    cabecalho_indice->status = status;
    return cabecalho_indice;
}

dados_int_t* cria_dados_int(int chaveBusca, long long int byteOffset) {
    dados_int_t* dados_int = (dados_int_t*) malloc(sizeof(dados_int_t));
    if (dados_int == NULL) return NULL;

    dados_int->chaveBusca = chaveBusca;
    dados_int->byteOffset = byteOffset;

    return dados_int;
}

dados_str_t* cria_dados_str(char* chaveBusca, long long int byteOffset) {
    dados_str_t* dados_str = (dados_str_t*) malloc(sizeof(dados_str_t));
    if (dados_str == NULL) return NULL;

    int i = 0;
    for (; i < 12 && chaveBusca[i] != '\0'; i++)
        dados_str->chaveBusca[i] = chaveBusca[i];
    
    for (; i<12; i++)
        dados_str->chaveBusca[i] = '$';

    dados_str->byteOffset = byteOffset;

    return dados_str;
}

int compara_dados_int(const void* esquerda, const void* direita) {
    return (*(const dados_int_t**)esquerda)->chaveBusca - (*(const dados_int_t**)direita)->chaveBusca;
}

void ordena_dados_int(dados_int_t** vetor_dados_int, int tamanho_vetor) {
    qsort(vetor_dados_int, tamanho_vetor, sizeof(dados_int_t*), compara_dados_int);
}

int compara_dados_str(const void* esquerda, const void* direita) {
    return (int)strcmp((*(const dados_str_t**)esquerda)->chaveBusca, (*(const dados_str_t**)direita)->chaveBusca);
}

void ordena_dados_str(dados_str_t** vetor_dados_str, int tamanho_vetor) {
    qsort(vetor_dados_str, tamanho_vetor, sizeof(dados_str_t*), compara_dados_str);
}

