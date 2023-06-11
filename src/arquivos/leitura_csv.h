#ifndef LEITURA_CSV_H
#define LEITURA_CSV_H

// comentado no .c

#include "../extras/stringdinamica.h"
#include "../structs/registros.h"
#include <stdio.h>
#include <stdlib.h>

crime_t* le_registro_criminal(FILE* arq_csv);
#endif
