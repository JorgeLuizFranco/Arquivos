#include "utils_structs.h"

int compara_string_limitada(char* s1, char* s2, int tamanho, int flag_s2_dinamica) {
    int i = 0;
    for (; i < tamanho && s2[i] != '\0' && s1[i] != '$'; i++)
        if (s1[i] > s2[i]) {
            return -1;
        } else if (s1[i] < s2[i]) {
            return 1;
        }
    if (i == tamanho || s1[i] == '$') {
        if (flag_s2_dinamica) {
            if (s2[i] == '\0') {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    } else {
        return -1;
    }

}

void copia_array_char(char* dest, char* src, int tamanho) {
    for (int i = 0; i < tamanho; i++)
        dest[i] = src[i];
}
