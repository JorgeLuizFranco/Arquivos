#ifndef UTILS_H
    #define UTILS_H

    #include <stdio.h>
    #include <stdlib.h>

    // Função fornecida
    // Alterei para pegar como entrada o próprio ponteiro para arquivo
    void binarioNaTela(char *nomeArquivoBinario);

    void scan_quote_string(char *str);

    // Imprime na tela que ocorreu erro
    void erro();

    void libera_vetor_ate_pos(void** vetor, int pos);

    int checa_existencia_arquivo(char* nome_arquivo);

    int compara_string_limitada(char* s1, char* s2, int tamanho, int flag_s2_dinamica);
#endif
