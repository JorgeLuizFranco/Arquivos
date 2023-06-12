#include "./utils_structs.h"

// basicamente fornece algumas funções pros arquivos dessa pasta "structs"
// não coloquei tudo em "extras/utils" porque "extras/utils" precisa
// dar include nesses structs para algumas funções

/**
 * compara duas strings, mas a primeira delas é limitada por um tamanho (ou outro caractére além de
 * $) e também pode ser que a outra seja limitada. Por isso que existe a "flag_s2_dinamica" que
 * indica se devo me preocupar com ela terminar com \0
 *
 * @param s1 primeira string a ser comparada
 * @param s2 segunda string a ser comparada
 * @param tamanho tamanho da string 1
 * @param flag_s2_dinamica indica se devo me preocupar com a string 2 terminar com \0
 * @return -1 se s1 for maior, 1 se for menor e 0 se forem iguais (ordem alfabetica)
 */
int compara_string_limitada(char* s1, char* s2, int tamanho, int flag_s2_dinamica) {
    // ate eu terminar de ler s1, se tiver algum caractere diferente comparando com
    // s2, eu já retorno -1 ou 1 de acordo com quem é maior/menor (se s1 for menor alfabeticamente,
    // retorno -1)
    int i = 0;
    for (; i < tamanho && s2[i] != '\0' && s1[i] != '$'; i++)
        if (s1[i] > s2[i]) {
            return -1;
        } else if (s1[i] < s2[i]) {
            return 1;
        }
    // entao ate agora sao iguais
    // se eu ja tiver no fim da string 1...
    if (i == tamanho || s1[i] == '$') {
        // se a string 2 for dinâmica
        if (flag_s2_dinamica) {
            // vejo se ja cheguei no fim dela
            if (s2[i] == '\0') {
                // se sim, retorno que são iguais
                return 0;
            } else {
                // se nao chegar no fim dela, retorno que s2 é maior
                return 1;
            }
        } else {
            // se a string2 nao for dinamica, basta que sejam iguais ate onde eu olhei
            return 0;
        }
    } else {
        // se eu nao tiver chegado ao final da string 1, entao quer dizer que eu parei
        // o for por ter chegado ao final da string 2
        // nesse caso, retorno que a string 1 é maior
        return -1;
    }
}

/**
 * copia a string src para a dest estando atento para o final da string src
 * e para o tamanho de dest
 *
 * @param dest string de destino
 * @param nome_campo string fonte
 * @param tamanho tamanho de dest
 */
void copia_array_char(char* dest, char* src, int tamanho) {
    // copia src para dest ate chegar no tamanho ou no fim de src
    int i = 0;
    for (; i < tamanho && src[i] != '\0'; i++)
        dest[i] = src[i];

    // se eu cheguei no fim de src, preencho o resto com $
    for (; i < tamanho; i++)
        dest[i] = '$';
}

/**
 * troca o valor de dois ponteiros genéricos
 * @param var1 um dos ponteiros
 * @param var2 o outro ponteiro
 */
void troca(void** var1, void** var2) {
    void* temp = *var1;
    *var1 = *var2;
    *var2 = temp;
}