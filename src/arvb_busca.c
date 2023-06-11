#include "arvb_busca.h"

int busca_arvb(FILE* arq_bin, FILE* arq_arv, cab_arvb_t* cab_arvb, cabecalho_t* cab_arq_bin,
               campo_busca_t** campos, int num_campos, char* nome_campo) {
    int flag_achou_alguem = 0;
    int pos_campo = checa_campo_procurado(campos, num_campos, nome_campo);

    if (pos_campo == -1) {
        // Busca linear
        long long int offset_atual = TAMANHO_CABECALHO;
        crime_t* crime_atual = NULL;

        while (offset_atual < cab_arq_bin->proxByteOffset) {

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

    // Busca na árvore B
    no_t* no_atual = NULL;
    dados_int_t* chave_procurada =
        cria_dados_int(campos[pos_campo]->chaveBuscaInt, -1); // byteoffset irrelevante

    if (chave_procurada == NULL) return 0;
    int ponteiro_prox_no = cab_arvb->noRaiz;

    if (ponteiro_prox_no == -1) {
        // vazia
        libera_memo(1, chave_procurada);
        msg_inexistente();
        return 1;
    }

    desloca_offset(arq_arv, 1LL * TAMANHO_PAGINA_ARVB * (ponteiro_prox_no + 1));

    while (ponteiro_prox_no != -1) {
        no_atual = le_no_arvb(arq_arv);

        if (no_atual == NULL) {
            libera_memo(1, chave_procurada);
            return 0;
        }

        int resp_busca_bin = busca_bin_no(no_atual, chave_procurada);
        if (resp_busca_bin == -1)
            resp_busca_bin = no_atual->num_chaves;
        else if (no_atual->chaves[resp_busca_bin].chaveBusca == chave_procurada->chaveBusca) {
            desloca_offset(arq_bin, no_atual->chaves[resp_busca_bin].byteOffset);
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

        ponteiro_prox_no = no_atual->ponteiro[resp_busca_bin];
        if (ponteiro_prox_no != -1)
            desloca_offset(arq_arv, 1LL * (ponteiro_prox_no + 1) * TAMANHO_PAGINA_ARVB);

        libera_memo(1, no_atual);
    }

    msg_inexistente();
    libera_memo(2, chave_procurada, no_atual);

    return 1;
}