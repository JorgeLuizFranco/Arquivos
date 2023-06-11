#include "campos.h"
#include "../extras/utils.h"

/**
 * le da entrada padrão um campo inteiro
 *
 * @param inteiro ponteiro para o inteiro
 *
 * se NULO, bota -1
 * se não, lê normalmente
 */
void le_campo_inteiro(int* inteiro) {
    char c = getc(stdin);
    if ((c = getc(stdin)) == 'N') {
        getc(stdin); // U
        getc(stdin); // L
        getc(stdin); // O
        *inteiro = -1;
    } else {
        ungetc(c, stdin);
        scanf("%d", inteiro);
    }
}

/**
 * le da entrada padrão um campo de busca
 *
 * @return ponteiro para o campo de busca
 */
campo_busca_t* le_campo_busca() {
    campo_busca_t* campo_atual = (campo_busca_t*)malloc(sizeof(campo_busca_t));
    if (campo_atual == NULL) return NULL;

    scanf("%s", campo_atual->campo_busca);

    if (strcmp("idCrime", campo_atual->campo_busca) == 0 ||
        strcmp("numeroArtigo", campo_atual->campo_busca) == 0) {
        le_campo_inteiro(&(campo_atual->chaveBuscaInt));
    } else {
        scan_quote_string(campo_atual->chaveBuscaStr);
    }
    return campo_atual;
}

/**
 * lê o numero especificado de campos de busca e aloca eles
 *
 * @param num_campos numero a ser lido
 * @return vetor (ponteiro) de ponteiro de campos
 */
campo_busca_t** le_campos_busca(int num_campos) {

    campo_busca_t** campos_buscados = (campo_busca_t**)malloc(num_campos * sizeof(campo_busca_t*));
    if (campos_buscados == NULL) return NULL;

    for (int i = 0; i < num_campos; i++) {
        campos_buscados[i] = le_campo_busca();
        if (campos_buscados[i] == NULL) {
            libera_vetor_ate_pos((void**)campos_buscados, i - 1);
            return NULL;
        }
    }

    return campos_buscados;
}

/**
 * retorna se registro tem campo com valor especificado
 *
 * @param crime_atual registro atual
 * @param campo_atual campo atual (contem nome do campo e valor)
 * @return 1 se registro tem, 0 se não
 */
int satisfaz_campo(crime_t* crime_atual, campo_busca_t* campo_atual) {
    if (strcmp(campo_atual->campo_busca, "idCrime") == 0) {
        return crime_atual->idCrime == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "numeroArtigo") == 0) {
        return crime_atual->numeroArtigo == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "dataCrime") == 0) {
        // uso a flag dinamica 1 para indicar que campo_atual->chaveBuscaStr termina com \0
        return compara_string_limitada(crime_atual->dataCrime, campo_atual->chaveBuscaStr, 10, 1) ==
               0;
    } else if (strcmp(campo_atual->campo_busca, "marcaCelular") == 0) {
        return compara_string_limitada(crime_atual->marcaCelular, campo_atual->chaveBuscaStr, 12,
                                       1) == 0;
    } else {
        char* string_dinamica_original;

        if (strcmp(campo_atual->campo_busca, "descricaoCrime") == 0) {
            string_dinamica_original = crime_atual->descricaoCrime;
        } else {
            string_dinamica_original = crime_atual->lugarCrime;
        }

        return strcmp(campo_atual->chaveBuscaStr, string_dinamica_original) == 0;
    }
    return -1; // inutil, nunca chega aqui
}

/**
 * retorna se registro satisfaz vários campos
 *
 * @param crime_atual registro atual
 * @param query_atual vetor de campos
 * @param n_campos numero de campos desse vetor
 * @return 1 se satisfaz a query, 0 se nao
 */
int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos) {
    int resposta = 1; // satisfaz. Se eu encontrar evidencias do contrario, seto pra zero

    campo_busca_t* campo_atual;

    for (int i = 0; i < n_campos; i++) {
        campo_atual = query_atual[i];
        if (satisfaz_campo(crime_atual, campo_atual) == 0) {
            resposta = 0;
            break;
        }
    }

    return resposta;
}

/**
 * atualiza campo de registro
 *
 * @param crime_atual registro atual
 * @param campo_atual campo (atualizar com base nele)
 * @return 1 se houve sucesso, 0 se nao (erro de alocacao)
 */
int atualiza_campo(crime_t* crime_atual, campo_busca_t* campo_atual) {
    if (strcmp(campo_atual->campo_busca, "idCrime") == 0) {
        crime_atual->idCrime = campo_atual->chaveBuscaInt;
        return 1;
    } else if (strcmp(campo_atual->campo_busca, "numeroArtigo") == 0) {
        crime_atual->numeroArtigo = campo_atual->chaveBuscaInt;
        return 1;
    } else if (strcmp(campo_atual->campo_busca, "dataCrime") == 0) {
        copia_array_char(crime_atual->dataCrime, campo_atual->chaveBuscaStr, 10);
        return 1;
    } else if (strcmp(campo_atual->campo_busca, "marcaCelular") == 0) {
        copia_array_char(crime_atual->marcaCelular, campo_atual->chaveBuscaStr, 12);
        return 1;
    } else {

        char** string_dinamica_original;

        if (strcmp(campo_atual->campo_busca, "descricaoCrime") == 0) {
            string_dinamica_original = &(crime_atual->descricaoCrime);
        } else {
            string_dinamica_original = &(crime_atual->lugarCrime);
        }

        *string_dinamica_original = (char*)realloc(
            *string_dinamica_original, sizeof(char) * (strlen(campo_atual->chaveBuscaStr) + 1));
        if (*string_dinamica_original == NULL) return 0;

        copia_array_char(*string_dinamica_original, campo_atual->chaveBuscaStr,
                         (int)strlen(campo_atual->chaveBuscaStr));
        (*string_dinamica_original)[(int)strlen(campo_atual->chaveBuscaStr)] = '\0';

        return 1;
    }
    return -1; // nunca chega aqui
}

/**
 * atualiza registro com base em multiplas atualizações
 *
 * @param crime_atual registro atual
 * @param atualizacoes vetor de campos, cada campo corresponde a uma atualizacao
 * @param n_campos tamanho do vetor
 * @return 1 se houve sucesso, 0 se nao (erro)
 */
int atualiza_registro(crime_t* crime_atual, campo_busca_t** atualizacoes, int n_campos) {
    campo_busca_t* campo_atual;

    for (int i = 0; i < n_campos; i++) {
        campo_atual = atualizacoes[i];
        if (atualiza_campo(crime_atual, campo_atual) == 0) return 0;
    }

    return 1;
}

/**
 * checa se no vetor de campos "campos" ha algum campo de nome "nome_campo"
 *
 * @param campos vetor de campos
 * @param n_campos tamanho do vetor
 * @param nome_campo nome de campo
 * @return numero i >= 0 indicando que campos[i] tem esse nome ou -1
 */
int checa_campo_procurado(campo_busca_t** campos, int n_campos, char* nome_campo) {
    for (int i = 0; i < n_campos; i++) {
        if (strcmp(campos[i]->campo_busca, nome_campo) == 0) {
            return i;
        }
    }
    return -1;
}
