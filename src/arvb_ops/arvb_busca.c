#include "./arvb_busca.h"

/**
 * Dada uma chave a ser procurada, percorre uma árvore B* com base no arquivo
 * de índices de árvore B* e chave passados como argumentos. Altera os parâmetros flag_presente e
 * pos_chave
 *
 * @param arq_arv ponteiro para arquivo de indice de arvore B*
 * @param cab_arvb ponteiro para cabeçalho em RAM da árvore B*
 * @param chave_procurada ponteiro para struct que contém a chave a ser procurada na árvore B*
 * @param no ponteiro de ponteiro para nó que faz com que (*no) aponte para o último nó que preciou
 * ser olhado na busca
 * @param pos_chave ponteiro para int que no final da execução aponta para a posição no último nó
 * percorrido que a chave foi encontrada. Se a chave não estiver presente, pos_chave é a posição
 * que a chave teria se ela estivesse na árvore B*
 * @param flag_presente 1 se a chave procurada estava presente, 0 se não
 * @return 0 se ocorreu algum erro de execução, 1 se deu tudo certo
 */
int busca_chave(FILE* arq_arv, cab_arvb_t* cab_arvb, dados_int_t* chave_procurada, no_t** no,
                int* pos_chave, int* flag_presente) {

    int ponteiro_prox_no = cab_arvb->noRaiz; // primeiro nó a iniciar a busca é a raiz

    if (ponteiro_prox_no == -1) { // não tem raiz
        // não precisa fazer busca, só diz que não tá presente e pronto
        if (flag_presente != NULL) *flag_presente = 0;
        return 1;
    }

    // se tem raiz, desloca offset pra raiz
    desloca_offset(arq_arv, byteoffset_no(ponteiro_prox_no));
    int resp_busca_bin;

    while (ponteiro_prox_no != -1) {
        *no = le_no_arvb(arq_arv); // lê nó no offset atual

        if (*no == NULL) {
            // erro de alocação
            return 0;
        }

        // no[resp_busca_bin] >= chave_procurada
        resp_busca_bin = busca_bin_no(*no, chave_procurada);
        // se essa resp != -1, quer dizer que no[resp_busca_bin-1] < chave_procurada
        // e tambem quer dizer que se a chave procurada nao for no[resp_busca_bin],
        // ponteiro[resp_busca_bin] é o RRN do nó que tem que descer na árvore para continuar
        // na busca
        if (resp_busca_bin == -1)               // se -1, então ninguém é >=
            resp_busca_bin = (*no)->num_chaves; // então o ponteiro é o mais da direita
        else if ((*no)->chaves[resp_busca_bin].chaveBusca == chave_procurada->chaveBusca) {
            // se achou a chave
            // marcar como presente
            if (flag_presente != NULL) *flag_presente = 1;
            // marcar a posição da chave achada
            *pos_chave = resp_busca_bin;

            // acaba a busca
            return 1;
        }

        // se chegou aqui, a busca ainda deve continuar e vou pro próximo nó
        ponteiro_prox_no = (*no)->ponteiro[resp_busca_bin];
        if (ponteiro_prox_no != -1) {
            // desloca offset pra ele
            desloca_offset(arq_arv, byteoffset_no(ponteiro_prox_no));
        }

        // libera o que foi alocado
        libera_memo(1, *no);
    }

    // se chegou aqui, chave não está na árvore
    if (flag_presente != NULL) *flag_presente = 0;
    // *pos_chave é a posição que ela deveria ter
    *pos_chave = resp_busca_bin;

    return 1;
}

/**
 * Faz uma query de busca, que possivelmente pode usufruir da árvore B*
 *
 * @param arq_bin ponteiro para arquivo binario de registros criminais
 * @param arq_arv ponteiro para arquivo de índices árvore B*
 * @param cab_arvb ponteiro para cabeçalho de arquivo de indices de arvore B*
 * @param cab_arq_bin ponteiro para cabeçalho do arquivo de registros criminais
 * @param campos ponteiro para vetor de campos que compõem a query de busca a ser feita
 * @param num_campos tamanho desse vetor
 * @param nome_campo nome do campo que o arquivo de índices de arvore B* usa
 * @return 0 se ocorreu algum erro, 1 se deu tudo certo
 */
int busca_arvb(FILE* arq_bin, FILE* arq_arv, cab_arvb_t* cab_arvb, cabecalho_t* cab_arq_bin,
               campo_busca_t** campos, int num_campos, char* nome_campo) {
    int flag_achou_alguem = 0;
    // vê a posição do campo de nome nome_campo no vetor de campos usados
    int pos_campo = checa_campo_procurado(campos, num_campos, nome_campo);

    // se = -1, a query não usa esse campo, então faço uma busca linear
    if (pos_campo == -1) {
        // Busca linear
        long long int offset_atual = TAMANHO_CABECALHO; // offset do próximo registro criminal
        crime_t* crime_atual = NULL;

        while (offset_atual < cab_arq_bin->proxByteOffset) { // percorre arquivo

            crime_atual = le_crime_bin(arq_bin);

            if (crime_atual == NULL) return 0;

            if (satisfaz_query(crime_atual, campos, num_campos) && crime_atual->removido != '1') {
                mostra_crime_tela(crime_atual);
                flag_achou_alguem = 1;
            }

            offset_atual += crime_atual->tamanho_real;
            libera_crime(crime_atual);
        }

        if (flag_achou_alguem == 0) {
            msg_inexistente();
        }

        return 1;
    }

    // Busca na árvore B*
    no_t* no_atual = NULL;
    // vê o campo com nome nome_campo e pega a chave dele
    dados_int_t* chave_procurada =
        cria_dados_int(campos[pos_campo]->chaveBuscaInt, -1); // byteoffset irrelevante

    if (chave_procurada == NULL) return 0;

    // Usa a função busca chave para checar se existe alguém com essa chave de busca na árvore B*
    int pos_chave, flag_presente;
    if (busca_chave(arq_arv, cab_arvb, chave_procurada, &no_atual, &pos_chave, &flag_presente) ==
        0) {
        // se der algum erro, libera memórias alocadas e retorna 0 indicando erro
        libera_memo(2, chave_procurada, no_atual);
        return 0;
    } else if (flag_presente == 1) {
        // se a busca não tiver tido erros e a chave estiver presente, tenho que verificar os outros
        // campos

        // então vou pro offset do arquivo binário
        desloca_offset(arq_bin, no_atual->chaves[pos_chave].byteOffset);
        crime_t* crime_achado = le_crime_bin(arq_bin);

        if (crime_achado == NULL) {
            libera_memo(2, chave_procurada, no_atual);
            return 0;
        }

        if (satisfaz_query(crime_achado, campos, num_campos) && crime_achado->removido != '1') {
            mostra_crime_tela(crime_achado);
        } else {
            msg_inexistente();
        }

        libera_memo(2, no_atual, chave_procurada);
        libera_crime(crime_achado);
        return 1;
    }

    // Se não tiver presente, imprime que não achou

    msg_inexistente();
    libera_memo(2, chave_procurada, no_atual);

    return 1;
}