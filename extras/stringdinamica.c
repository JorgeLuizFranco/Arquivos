#include "stringdinamica.h"

/**
 * Constrói um novo objeto StringDinamica.
 *
 * @param ds Ponteiro para o ponteiro StringDinamica que irá conter o novo objeto.
 */
void sd_constroi(StringDinamica** ds) {
    *ds = (StringDinamica*)malloc(sizeof(StringDinamica));
    if (*ds == NULL) {
        exit(EXIT_FAILURE);
    }

    (*ds)->len = 0;
    (*ds)->capacidade = 16;
    (*ds)->str = (char*)malloc((*ds)->capacidade);
    if ((*ds)->str == NULL) {
        exit(EXIT_FAILURE);
    }
    (*ds)->str[0] = '\0';
}

/**
 * Adiciona um caractere ao final da string armazenada em um objeto StringDinamica.
 * Redimensiona o buffer interno, se necessário.
 *
 * @param ds Ponteiro para o objeto StringDinamica.
 * @param c O caractere a ser adicionado.
 */
void sd_adiciona_char(StringDinamica* ds, char c) {
    // A ideia dessa maneira de realocar é não precisar ficar dando realloc toda hora
    if (ds->len + 1 >= ds->capacidade) {
        ds->capacidade *= 2;
        ds->str = (char*)realloc(ds->str, ds->capacidade);
        if (ds->str == NULL) {
            exit(EXIT_FAILURE);
        }
    }
    ds->str[ds->len++] = c;
    ds->str[ds->len] = '\0';
}

/**
 * Limpa a string armazenada em um objeto StringDinamica.
 *
 * @param ds Ponteiro para o objeto StringDinamica.
 */
void sd_limpa(StringDinamica* ds) {
    ds->len = 0;
    ds->str[0] = '\0';
}

/**
 * Retorna um ponteiro para a string terminada em nulo armazenada em um objeto StringDinamica.
 *
 * @param ds Ponteiro para o objeto StringDinamica.
 * @return Um ponteiro para a string terminada em nulo.
 */
char* sd_obtem_string(StringDinamica* ds) {
    return ds->str;
}

/**
 * Destroi um objeto StringDinamica e libera sua memória.
 *
 * @param ds Ponteiro para o ponteiro StringDinamica a ser destruído.
 */
void sd_destroi(StringDinamica** ds) {
    free((*ds)->str);
    free(*ds);
    *ds = NULL;
}