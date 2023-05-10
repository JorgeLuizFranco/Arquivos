#ifndef UTILS_H
    #define UTILS_H

    #include <stdio.h>
    #include <stdlib.h>
    #include "../registros.h"
    #include "../indices.h"
    #include "../campos.h"

    // Função fornecida
    // Alterei para pegar como entrada o próprio ponteiro para arquivo
    void binarioNaTela(char *nomeArquivoBinario);

    void scan_quote_string(char *str);

    // Imprime na tela que ocorreu erro
    void erro();

    void libera_vetor_ate_pos(void** vetor, int pos);

    int checa_existencia_arquivo(char* nome_arquivo);

    void libera_memo_consultas(int flag_erro, FILE* arq_bin, cabecalho_t* cabecalho, FILE* arq_idx, cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos, int num_campos, void** dados, int num_dados);

    void desloca_offset(FILE* arq_bin, long long int byteOffset);
    
#endif
