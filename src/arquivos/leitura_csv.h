#ifndef LEITURA_CSV_H
    #define LEITURA_CSV_H

    #include <stdio.h>
    #include <stdlib.h>
    #include "../structs/registros.h"
    #include "../extras/stringdinamica.h"

    crime_t* le_registro_criminal(FILE* arq_csv);
#endif
