#include "indices.h"

cabecalho_indice_t* cria_cabecalho_indice(char status, int nro_reg) {
    cabecalho_indice_t* cabecalho_indice = (cabecalho_indice_t*) malloc(sizeof(cabecalho_indice_t));
    if (cabecalho_indice == NULL) return NULL;

    cabecalho_indice->status = status;
    cabecalho_indice->nro_reg = nro_reg;
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

void ordena_dados_gen(void** vetor_dados, int tipoVar, int tamanho_vetor) {
    if (tipoVar == 0)
        ordena_dados_int((dados_int_t**)vetor_dados, tamanho_vetor);
    else
        ordena_dados_str((dados_str_t**)vetor_dados, tamanho_vetor);
}

int pega_tipo_dado(void* dado) {
    if (sizeof(dado) == sizeof(dados_int_t)) {
        return 0;
    } else {
        return 1;
    }
}

int compara_chave_busca(void* generico_esq /* dados_int_t* ou dados_str_t* */, void* /* char* ou int* */ generico_dir, int flag_modo, int tipo_var) {
    if (tipo_var == 0) {
        if (flag_modo == 1) {
            return ((dados_int_t*)generico_esq)->chaveBusca >= *((int*)generico_dir);
        } else if (flag_modo == -1) {
            return ((dados_int_t*)generico_esq)->chaveBusca <= *((int*)generico_dir);
        } else {
            return ((dados_int_t*)generico_esq)->chaveBusca == *((int*)generico_dir);
        }
    } else {
        if (flag_modo == 1) {
            return compara_string_limitada(((dados_str_t*)generico_esq)->chaveBusca, (char*)generico_dir, 12, 1) <= 0;
        } else if (flag_modo == -1) {
            return compara_string_limitada(((dados_str_t*)generico_esq)->chaveBusca, (char*)generico_dir, 12, 1) >= 0;
        } else {
            return compara_string_limitada(((dados_str_t*)generico_esq)->chaveBusca, (char*)generico_dir, 12, 1) == 0;
        }
    }
}


dados_int_t* pega_dado_int(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_int_t* dado_atual = NULL;

    switch (nome_campo[0]) {
        // idCrime
        case 'i':
            dado_atual = cria_dados_int(crime_atual->idCrime, byteOffset);
            break;
        // numeroArtigo
        case 'n':
            dado_atual = cria_dados_int(crime_atual->numeroArtigo, byteOffset);
            break;
    }

    return dado_atual;
}

dados_str_t* pega_dado_str(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_str_t* dado_atual = NULL;

    switch (nome_campo[2]) {
        // dataCrime
        case 't':
            dado_atual = cria_dados_str(crime_atual->dataCrime, byteOffset);
            break;
        // marcaCelular
        case 'r':
            dado_atual = cria_dados_str(crime_atual->marcaCelular, byteOffset);
            break;
        // lugarCrime
        case 'g':
            dado_atual = cria_dados_str(crime_atual->lugarCrime, byteOffset);
            break;
        // descricaoCrime
        case 's':
            dado_atual = cria_dados_str(crime_atual->descricaoCrime, byteOffset);
            break;
    }

    return dado_atual;
}

void* pega_dado_generico(crime_t* crime_atual, char* nome_campo, long long int byteOffset, int tipoVar) {
    if (tipoVar == 0) {
        return (void*)pega_dado_int(crime_atual, nome_campo, byteOffset);
    } else {
        return (void*)pega_dado_str(crime_atual, nome_campo, byteOffset);
    }
}

void* pega_chave_generico(void* dado_generico, int tipoVar) {
    if (tipoVar == 0) {
        return (void*)(&(((dados_int_t*)dado_generico)->chaveBusca));
    } else {
        return (void*)(((dados_str_t*)dado_generico)->chaveBusca);
    }
}

long long int pega_offset_generico(void* dado_generico, int tipoVar) {
    if (tipoVar == 0) {
        return ((dados_int_t*)dado_generico)->byteOffset;
    } else {
        return ((dados_str_t*)dado_generico)->byteOffset;
    }
}

void copia_dado_int(dados_int_t* dest_dado, dados_int_t* src_dado) {
    dest_dado->byteOffset = src_dado->byteOffset;
    dest_dado->chaveBusca = src_dado->chaveBusca;
}

void copia_dado_str(dados_str_t* dest_dado, dados_str_t* src_dado) {
    dest_dado->byteOffset = src_dado->byteOffset;
    for (int i = 0; i < 12; i++)
        dest_dado->chaveBusca[i] = src_dado->chaveBusca[i];
}

void copia_dado_gen(void* dest_dado, void* src_dado, int tipoVar) {
    if (tipoVar == 0)
        copia_dado_int((dados_int_t*)dest_dado, (dados_int_t*)src_dado);
    else
        copia_dado_str((dados_str_t*)dest_dado, (dados_str_t*)src_dado);
}

void remove_dado(void*** vetor_dados, int tipoVar, int* tam_vetor, int pos_dado) {
    for (int i = pos_dado; i+1 < *tam_vetor; i++)
        copia_dado_gen((*vetor_dados)[i], (*vetor_dados)[i+1], tipoVar);
    
    *vetor_dados = (void**) realloc(*vetor_dados, (--(*tam_vetor))*get_tam_var(tipoVar));
}

int get_tipo_var(char* nome_tipo) {
    return nome_tipo[0] == 'i' ? 0 : 1;
}

size_t get_tam_var(int tipoVar) {
    if (tipoVar == 0) return sizeof(dados_int_t*);
    return sizeof(dados_str_t*);
}