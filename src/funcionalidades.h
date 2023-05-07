#ifndef FUNCIONALIDADES_H
    #define FUNCIONALIDADES_H

    #include "./io/escrita_tela.h"
    #include "./arquivos/escrita_bin_ind.h"
    #include "./arquivos/escrita_bin.h"
    #include "./arquivos/leitura_bin.h"
    #include "./arquivos/leitura_csv.h"
    #include "./arquivos/leitura_bin_ind.h"
    #include "./extras/utils.h"
    #include "busca.h"

    
    typedef void (*funcao_anonima_t)(crime_t*, FILE*, long long int);

    // Funcionalidade 1. A partir de um nome de arquivo csv e nome de arquivo binário,
    // faz um binário com os registros descritos no csv
    void csv_para_bin(char* nome_arq_csv, char* nome_arq_bin);

    // Funcionalidade 2. A partir de um nome de arquivo binário, mostra os registros
    // de crimes que armazena na tela
    void mostra_crimes_tela(char* nome_arq_bin);

    // Funcionalidade 3. A partir de um nome de arquivo binário de registros, nome do campo
    // e do tipo desse campo, cria um arquivo binário de índices cujo nome é especificado
    // pela variável nome_arq_ind
    void cria_arq_indices(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_ind);

    void procura_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas);

    void remove_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas);

#endif
