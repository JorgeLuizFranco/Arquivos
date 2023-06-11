#include "arvoreb.h"
#include "../arquivos/escrita_arvoreb.h"
#include "../arquivos/leitura_arvoreb.h"

// acha o menor i tal que no_atual->chaves[i].chaveBusca > chave->chaveBusca
int busca_bin_no(no_t* no_atual, dados_int_t* chave) {
    int baixo = 0;
    int alto = no_atual->num_chaves - 1;
    int meio;
    int resultado = -1;

    while (baixo <= alto) {
        meio = (baixo + alto) / 2;
        if (compara_chave_busca(&(no_atual->chaves[meio]), &(chave->chaveBusca), 1, 0, 0) == 1) {
            alto = meio - 1;
            resultado = meio;
        } else {
            baixo = meio + 1;
        }
    }

    return resultado;
}

void debuga_no(no_t* no) {
    fprintf(stderr, "O no atual tem nivel %d e %d chaves:\n", no->nivel, no->num_chaves);

    for (int i = 0; i < no->num_chaves; i++) {
        fprintf(stderr, "(%d) chave=%d ", no->ponteiro[i], no->chaves[i].chaveBusca);
    }
    fprintf(stderr, "(%d)\n", no->ponteiro[no->num_chaves]);
}

no_t* procura_onde_insere(FILE* arq_arvb, cab_arvb_t* cab, dados_int_t* chave) {
    no_t* no_atual = NULL;
    int res_busca_bin = -1;
    int ponteiro_prox_no = 0;

    while (ponteiro_prox_no != -1) {
        no_atual = le_no_arvb(arq_arvb);
        if (no_atual == NULL) return NULL;
        res_busca_bin = busca_bin_no(no_atual, chave);
        res_busca_bin = res_busca_bin != -1 ? res_busca_bin : ORDEM_ARVORE - 1;
        ponteiro_prox_no = no_atual->ponteiro[res_busca_bin];
        if (ponteiro_prox_no != -1) {
            desloca_offset(arq_arvb, ponteiro_prox_no);
            libera_memo(1, no_atual);
        }
    }

    return no_atual;
}

int insere_chave_arvb(FILE* arq_arvb, cab_arvb_t* cab_arvb, dados_int_t* chave) {}

void escreve_arvb(FILE* arq_bin, FILE* arq_arvb, char* nome_campo, char* tipo_campo,
                  long long int byteOffset_reg) {

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        fecha_arquivos(2, arq_arvb, arq_bin);
        libera_memo(1, cabecalho);
        erro();
        return;
    }

    cab_arvb_t* cab_arv = (cab_arvb_t*)malloc(sizeof(cab_arvb_t));
    if (cab_arv == NULL) {
        fecha_arquivos(2, arq_arvb, arq_bin);
        libera_memo(1, cabecalho);
        erro();
        return;
    }
    cab_arv->status = '0';
    cab_arv->noRaiz = -1;
    cab_arv->proxRRN = 0;
    cab_arv->nroNiveis = 0;
    cab_arv->nroChaves = 0;
    escreve_cab_arvb(arq_arvb, cab_arv);

    long long int offset_atual = TAMANHO_CABECALHO;
    crime_t* crime_atual = NULL;
    dados_int_t* dado_atual = NULL;

    while (offset_atual < cabecalho->proxByteOffset) {
        crime_atual = le_crime_bin(arq_bin);

        if (crime_atual == NULL) {
            fecha_arquivos(2, arq_arvb, arq_bin);
            libera_memo(2, cabecalho, cab_arv);
            erro();
            return;
        }

        dado_atual = (dados_int_t*)pega_dado_generico(crime_atual, nome_campo, offset_atual, 0);
        if (dado_atual == NULL) {
            fecha_arquivos(2, arq_arvb, arq_bin);
            libera_memo(2, cabecalho, cab_arv);
            libera_crime(crime_atual);
            erro();
            return;
        }

        if (checa_dado_nulo((void*)dado_atual, 0) == 0) {
            if (insere_chave_arvb(arq_arvb, cab_arv, dado_atual) == 0) {
                fecha_arquivos(2, arq_arvb, arq_bin);
                libera_memo(3, cabecalho, cab_arv, dado_atual);
                libera_crime(crime_atual);
                erro();
                return;
            }
            desloca_offset(arq_arvb, TAMANHO_PAGINA_ARVB);
        }

        offset_atual += crime_atual->tamanho_real;
        libera_crime(crime_atual);
        libera_memo(1, dado_atual);
    }

    fecha_arquivos(2, arq_arvb, arq_bin);
    libera_memo(2, cabecalho, cab_arv);
}