#include "./indices.h"

/**
 * cria cabeçalho do arquivo de índices
 *
 * @param status determina se está sendo realizada uma operação no arquivo: '0' ou '1'
 * @param arq_ind ponteiro para o arquivo de índices
 * @return ponteiro para o cabeçalho alocado
 */
cabecalho_indice_t* cria_cabecalho_indice(char status, int nro_reg) {
    cabecalho_indice_t* cabecalho_indice = (cabecalho_indice_t*)malloc(sizeof(cabecalho_indice_t));
    if (cabecalho_indice == NULL) return NULL;

    cabecalho_indice->status = status;
    cabecalho_indice->nro_reg = nro_reg;
    return cabecalho_indice;
}

/**
 * cria registro de índice do tipo inteiro
 *
 * @param chaveBusca chave de busca que guarda
 * @param byteOffset byteoffset que guarda
 * @return ponteiro para o registro de índice
 */
dados_int_t* cria_dados_int(int chaveBusca, long long int byteOffset) {
    dados_int_t* dados_int = (dados_int_t*)malloc(sizeof(dados_int_t));
    if (dados_int == NULL) return NULL;

    dados_int->chaveBusca = chaveBusca;
    dados_int->byteOffset = byteOffset;

    return dados_int;
}

/**
 * cria registro de índice do tipo string
 *
 * @param chaveBusca chave de busca que guarda
 * @param limitacao limitador para pegar so parte de chaveBusca
 * @param byteOffset byteoffset que guarda
 * @return ponteiro para o registro de índice
 */
dados_str_t* cria_dados_str(char* chaveBusca, int limitacao, long long int byteOffset) {
    dados_str_t* dados_str = (dados_str_t*)malloc(sizeof(dados_str_t));
    if (dados_str == NULL) return NULL;

    // geralmente pega 12 caracteres. mas se tiver alguma limitacao na string,
    // pega esses primeiros caracteres e o resto preenche com $
    int i = 0;
    for (; i < 12 && i < limitacao; i++)
        dados_str->chaveBusca[i] = chaveBusca[i];

    for (; i < 12; i++)
        dados_str->chaveBusca[i] = '$';

    dados_str->byteOffset = byteOffset;

    return dados_str;
}

/**
 * compara registros de índice do tipo int
 *
 * @param esquerda parte da esquerda a se comparar
 * @param direita parte da direita
 * @return número negativo se esquerda menor. positivo se esquerda maior. zero se iguais.
 */
int compara_dados_int(const void* esquerda, const void* direita) {
    // calcula a diferença entre as chaves de busca
    // se forem diferentes, essa difereça já tem o sinal que quero, porque chaveBusca tem maior
    // prioridade na hora de identificar se um registro de indice e menor ou maior que outro
    // se sub = 0, então preciso comparar pelo byteoffset
    // novamente, pego o sinal da diferença deles
    int sub =
        (*(const dados_int_t**)esquerda)->chaveBusca - (*(const dados_int_t**)direita)->chaveBusca;
    return sub != 0 ? sub
                    : ((*(const dados_int_t**)esquerda)->byteOffset -
                                   (*(const dados_int_t**)direita)->byteOffset >
                               0
                           ? 1
                           : -1);
}

/**
 * ordena registros de índice do tipo int
 *
 * @param vetor_dados_int vetor de registros a ser ordenado
 * @param tamanho_vetor tamanho desse vetor
 */
void ordena_dados_int(dados_int_t** vetor_dados_int, int tamanho_vetor) {
    qsort(vetor_dados_int, tamanho_vetor, sizeof(dados_int_t*), compara_dados_int);
}

/**
 * compara registros de índice do tipo string
 *
 * @param esquerda parte da esquerda a se comparar
 * @param direita parte da direita
 * @return número negativo se esquerda menor. positivo se esquerda maior. zero se iguais.
 */
int compara_dados_str(const void* esquerda, const void* direita) {
    // lógica análoga à de compara_dados_int
    int comp = (int)strcmp((*(const dados_str_t**)esquerda)->chaveBusca,
                           (*(const dados_str_t**)direita)->chaveBusca);
    return comp != 0 ? comp
                     : ((*(const dados_str_t**)esquerda)->byteOffset -
                                    (*(const dados_str_t**)direita)->byteOffset >
                                0
                            ? 1
                            : -1);
}

/**
 * ordena registros de índice do tipo string
 *
 * @param vetor_dados_str vetor de registros a ser ordenado
 * @param tamanho_vetor tamanho desse vetor
 */
void ordena_dados_str(dados_str_t** vetor_dados_str, int tamanho_vetor) {
    qsort(vetor_dados_str, tamanho_vetor, sizeof(dados_str_t*), compara_dados_str);
}

/**
 * compara registros de índice de tipo genérico (pode ser string ou int)
 *
 * @param esquerda parte da esquerda a se comparar
 * @param direita parte da direita
 * @param tipoVar 0 se int, 1 se string
 * @return número negativo se esquerda menor. positivo se esquerda maior. zero se iguais.
 */
int compara_dados_gen(void* esquerda, void* direita, int tipoVar) {
    if (tipoVar == 0) return compara_dados_int(&esquerda, &direita);
    return compara_dados_str(&esquerda, &direita);
}

/**
 * ordena registros de índice de tipo generico
 *
 * @param vetor_dados vetor de registros a ser ordenado
 * @param tipoVar 0 se int, 1 se string
 * @param tamanho_vetor tamanho desse vetor
 */
void ordena_dados_gen(void** vetor_dados, int tipoVar, int tamanho_vetor) {
    if (tipoVar == 0)
        ordena_dados_int((dados_int_t**)vetor_dados, tamanho_vetor);
    else
        ordena_dados_str((dados_str_t**)vetor_dados, tamanho_vetor);
}

/**
 * retorna se um registro genérico é int(0) ou string(1)
 *
 * @param dado registro de índice genérico
 * @return 0(int) ou 1(string)
 */
int pega_tipo_dado(void* dado) {
    if (sizeof(dado) == sizeof(dados_int_t)) {
        return 0;
    } else {
        return 1;
    }
}

/**
 * compara um registro de indice generico com uma chave de busca generica
 *
 * @param generico_esq registro generico
 * @param generico_dir chave de busca generica (char* ou int*)
 * @param flag_modo se 1, retorna o resultado de >=. se -1, de <=. se 0, de ==
 * @param tipo_var 0 se int, 1 se string
 * @param flag_dinamica flag que ajuda a realizar a comparação no caso de ser string. Olhar a função
 * que usa essa flag para mais informações
 * @return 1 se obedece a comparação feita (que foi especificada pela flag_modo), 0 se não
 */
int compara_chave_busca(void* generico_esq /* dados_int_t* ou dados_str_t* */,
                        void* /* char* ou int* */ generico_dir, int flag_modo, int tipo_var,
                        int flag_dinamica) {
    if (tipo_var == 0) {
        if (flag_modo == 1) {
            return *((int*)pega_chave_generico(generico_esq, tipo_var)) >= *((int*)generico_dir);
        } else if (flag_modo == -1) {
            return *((int*)pega_chave_generico(generico_esq, tipo_var)) <= *((int*)generico_dir);
        } else {
            return *((int*)pega_chave_generico(generico_esq, tipo_var)) == *((int*)generico_dir);
        }
    } else {
        if (flag_modo == 1) {
            return compara_string_limitada((char*)pega_chave_generico(generico_esq, tipo_var),
                                           (char*)generico_dir, 12, flag_dinamica) <= 0;
        } else if (flag_modo == -1) {
            return compara_string_limitada((char*)pega_chave_generico(generico_esq, tipo_var),
                                           (char*)generico_dir, 12, flag_dinamica) >= 0;
        } else {
            return compara_string_limitada((char*)pega_chave_generico(generico_esq, tipo_var),
                                           (char*)generico_dir, 12, flag_dinamica) == 0;
        }
    }
}

/**
 * dado registro, retorna um registro de índice do tipo int com o campo requisisitado
 *
 * @param crime_atual registro de crime
 * @param nome_campo nome do campo requisitado
 * @param byteOffset byteoffset em que se encontra o crime
 */
dados_int_t* pega_dado_int(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_int_t* dado_atual = NULL;

    // no caso de campos inteiros, so temos "idCrime" e "numeroArtigo"
    // entao posso olhar apenas para o primeiro caractere para diferencia-los
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

/**
 * dado registro, retorna um registro de índice do tipo string com o campo requisisitado
 *
 * @param crime_atual registro de crime
 * @param nome_campo nome do campo requisitado
 * @param byteOffset byteoffset em que se encontra o crime
 */
dados_str_t* pega_dado_str(crime_t* crime_atual, char* nome_campo, long long int byteOffset) {
    dados_str_t* dado_atual = NULL;

    // no caso de campos inteiros, so temos quatro nomes diferentes de campos
    // entao posso olhar apenas para o terceiro caractere para diferencia-los
    switch (nome_campo[2]) {
    // da[t]aCrime
    case 't':
        dado_atual = cria_dados_str(crime_atual->dataCrime, 10, byteOffset);
        break;
    // ma[r]caCelular
    case 'r':
        dado_atual = cria_dados_str(crime_atual->marcaCelular, 12, byteOffset);
        break;
    // lu[g]arCrime
    case 'g':
        dado_atual = cria_dados_str(crime_atual->lugarCrime, (int)strlen(crime_atual->lugarCrime),
                                    byteOffset);
        break;
    // de[s]cricaoCrime
    case 's':
        dado_atual = cria_dados_str(crime_atual->descricaoCrime,
                                    (int)strlen(crime_atual->descricaoCrime), byteOffset);
        break;
    }

    return dado_atual;
}

/**
 * dado registro, retorna um registro de índice e tipo genérico com o campo requisisitado
 *
 * @param crime_atual registro de crime
 * @param nome_campo nome do campo requisitado
 * @param byteOffset byteoffset em que se encontra o crime
 * @param tipoVar 0 se int, 1 se string
 */
void* pega_dado_generico(crime_t* crime_atual, char* nome_campo, long long int byteOffset,
                         int tipoVar) {
    if (tipoVar == 0) {
        return (void*)pega_dado_int(crime_atual, nome_campo, byteOffset);
    } else {
        return (void*)pega_dado_str(crime_atual, nome_campo, byteOffset);
    }
}

/**
 * dado registro de indice generico, retorna sua chave de busca
 *
 * @param dado_generico registro de indice
 * @param tipoVar 0 se int, 1 se string
 * @return chave de busca (char*) ou ponteiro para int
 */
void* pega_chave_generico(void* dado_generico, int tipoVar) {
    if (tipoVar == 0) {
        return (void*)(&(((dados_int_t*)dado_generico)->chaveBusca));
    } else {
        return (void*)(((dados_str_t*)dado_generico)->chaveBusca);
    }
}

/**
 * pega o byteOffset de um registro de indice generico
 *
 * @param dado_generico registro de indice
 * @param tipoVar 0 se int, 1 se string
 * @return byteOffset
 */
long long int pega_offset_generico(void* dado_generico, int tipoVar) {
    if (tipoVar == 0) {
        return ((dados_int_t*)dado_generico)->byteOffset;
    } else {
        return ((dados_str_t*)dado_generico)->byteOffset;
    }
}

/**
 * copia um registro de indice int para outro
 *
 * @param dest_dado registro de destino
 * @param src_dado regitro de origem
 */
void copia_dado_int(dados_int_t* dest_dado, dados_int_t* src_dado) {
    dest_dado->byteOffset = src_dado->byteOffset;
    dest_dado->chaveBusca = src_dado->chaveBusca;
}

/**
 * copia um registro de indice str para outro
 *
 * @param dest_dado registro de destino
 * @param src_dado regitro de origem
 */
void copia_dado_str(dados_str_t* dest_dado, dados_str_t* src_dado) {
    dest_dado->byteOffset = src_dado->byteOffset;
    for (int i = 0; i < 12; i++)
        dest_dado->chaveBusca[i] = src_dado->chaveBusca[i];
}

/**
 * copia um registro de indice generico para outro
 *
 * @param dest_dado registro de destino
 * @param src_dado regitro de origem
 * @param tipoVar 0 se int, 1 se string
 */
void copia_dado_gen(void* dest_dado, void* src_dado, int tipoVar) {
    if (tipoVar == 0)
        copia_dado_int((dados_int_t*)dest_dado, (dados_int_t*)src_dado);
    else
        copia_dado_str((dados_str_t*)dest_dado, (dados_str_t*)src_dado);
}

/**
 * remove um dado de um vetor generico
 *
 * @param vetor_dados vetor de registros de indice genericos
 * @param tipoVar 0 se int, 1 se string
 * @param tam_vetor tamanho desse vetor
 * @param pos_dado posicao a ser removida
 */
void remove_dado(void*** vetor_dados, int tipoVar, int* tam_vetor, int pos_dado) {
    // faz uma shiftacao ate o fim do vetor, ate que a ultima posicao fique "duplicada"
    for (int i = pos_dado; i + 1 < *tam_vetor; i++)
        copia_dado_gen((*vetor_dados)[i], (*vetor_dados)[i + 1], tipoVar);

    // remove essa ultima posicao com um realloc que diminui o tamanho do vetor
    *vetor_dados = (void**)realloc(*vetor_dados, (--(*tam_vetor)) * get_tam_var(tipoVar));
}

/**
 * retorna 0 se int, 1 se string
 *
 * @param nome_tipo uma string que eh "int" ou "string"
 * @return 0 no primeiro caso e 1 no segundo. basta olhar o primeiro caracere
 */
int get_tipo_var(char* nome_tipo) { return nome_tipo[0] == 'i' ? 0 : 1; }

/**
 * retorna o tamanho de uma variável
 *
 * @param tipoVar 0 se int, 1 se string
 * @return sizeof de ponteiro para os respectivos structs relacionados
 */
size_t get_tam_var(int tipoVar) {
    if (tipoVar == 0) return sizeof(dados_int_t*);
    return sizeof(dados_str_t*);
}

/**
 * checa se dado generico tem chade de busca nula
 *
 * @param dado registro de indice generico
 * @param tipoVar 0 se int, 1 se string
 * @return 1 se e nulo, 0 se nao
 */
int checa_dado_nulo(void* dado, int tipoVar) {
    if (tipoVar == 0) return ((dados_int_t*)dado)->chaveBusca == -1;
    return ((dados_str_t*)dado)->chaveBusca[0] == '$';
}

/**
 * seta o campo de byte offset para um registro generico
 *
 * @param dado registro de indice generico
 * @param tipoVar 0 se int, 1 se string
 */
void seta_offset_gen(void* dado, int tipoVar, long long int offset) {
    if (tipoVar == 0)
        ((dados_int_t*)dado)->byteOffset = offset;
    else
        ((dados_str_t*)dado)->byteOffset = offset;
}