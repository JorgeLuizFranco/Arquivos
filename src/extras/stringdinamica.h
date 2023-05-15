#ifndef STRINGDINAMICA_H_
#define STRINGDINAMICA_H_

// explicado no .c

#include <stdlib.h>
#include <string.h>

typedef struct {
    char* str;
    size_t len;
    size_t capacidade;
} StringDinamica;

void sd_constroi(StringDinamica** ds);

void sd_adiciona_char(StringDinamica* ds, char c);

void sd_limpa(StringDinamica* ds);

char* sd_obtem_string(StringDinamica* ds);

void sd_destroi(StringDinamica** ds);

#endif /* STRINGDINAMICA_H_ */
