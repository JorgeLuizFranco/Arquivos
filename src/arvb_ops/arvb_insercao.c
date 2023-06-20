#include "./arvb_insercao.h"

void insere_chave_no(no_t* no, dados_int_t* chave) {
    int resp_busca_bin = busca_bin_no(no, chave);
    if (resp_busca_bin != -1) {
        for (int i = no->num_chaves++; i > resp_busca_bin; i--) {
            copia_dado_gen((void*)(&(no->chaves[i])), (void*)(&(no->chaves[i - 1])), 0);
        }
    } else {
        resp_busca_bin = no->num_chaves++;
    }
    copia_dado_gen((void*)(&(no->chaves[resp_busca_bin])), chave, 0);
}

int split1p2(FILE* arq_arvb, cab_arvb_t* cab_arvb, no_t* no) {
    cab_arvb->nroNiveis++;

    no_t* rrn1 = aloca_no();
    if (rrn1 == NULL) return 0;
    no_t* rrn2 = aloca_no();
    if (rrn2 == NULL) {
        libera_memo(1, rrn1);
        return 0;
    }

    rrn1->nivel = no->nivel;
    rrn1->num_chaves = (ORDEM_ARVORE - 1) / 2;
    for (int i = 0; i < rrn1->num_chaves; i++) {
        copia_dado_gen((void*)(&(rrn1->chaves[i])),
                       (void*)(&(no->chaves[ORDEM_ARVORE / 2 + 1 + i])), 0);
        rrn1->ponteiro[i] = no->ponteiro[ORDEM_ARVORE / 2 + 1 + i];
    }
    rrn1->ponteiro[rrn1->num_chaves] = no->ponteiro[ORDEM_ARVORE / 2 + 1 + rrn1->num_chaves];
    escreve_no_rrn(arq_arvb, rrn1, cab_arvb->proxRRN++);

    rrn2->nivel = ++(no->nivel);
    rrn2->num_chaves = 1;
    rrn2->chaves[0] = no->chaves[ORDEM_ARVORE / 2];
    rrn2->ponteiro[0] = cab_arvb->noRaiz;      // aponta pra antiga raiz na esquerda
    rrn2->ponteiro[1] = cab_arvb->proxRRN - 1; // aponta pro novo no que foi escrito agora
    escreve_no_rrn(arq_arvb, rrn2, cab_arvb->proxRRN++);

    // agora raiz nao vai ser mais raiz
    setam1_chaves(no, ORDEM_ARVORE / 2, no->num_chaves - 1);
    setam1_ponts(no, ORDEM_ARVORE / 2 + 1, no->num_chaves);
    no->num_chaves = ORDEM_ARVORE / 2;
    no->nivel--;
    escreve_no_rrn(arq_arvb, no, cab_arvb->noRaiz);
    cab_arvb->noRaiz = cab_arvb->proxRRN - 1;

    libera_memo(2, rrn1, rrn2);
    return 1;
}

dados_int_t* cria_vet_chaves(dados_int_t* chave, no_t* no_esq, no_t* no_dir) {
    dados_int_t* vet_aux =
        (dados_int_t*)malloc(sizeof(dados_int_t) * (1 + no_esq->num_chaves + no_dir->num_chaves));
    if (vet_aux == NULL) return NULL;

    int i;
    for (i = 0; i < no_esq->num_chaves; i++) {
        copia_dado_gen((void*)(&(vet_aux[i])), (void*)(&(no_esq->chaves[i])), 0);
    }

    copia_dado_gen((void*)(&(vet_aux[i++])), (void*)chave, 0);

    for (; i < 1 + no_esq->num_chaves + no_dir->num_chaves; i++) {
        copia_dado_gen((void*)(&(vet_aux[i])),
                       (void*)(&(no_dir->chaves[i - 1 - no_esq->num_chaves])), 0);
    }

    return vet_aux;
}

int* cria_vet_ponts(int* ponts_esq, int num_ponts_esq, int* ponts_dir, int num_ponts_dir) {
    int* vet_aux = (int*)malloc(sizeof(int) * (num_ponts_esq + num_ponts_dir));

    int i = 0;
    for (; i < num_ponts_esq; i++)
        vet_aux[i] = ponts_esq[i];

    for (; i < num_ponts_dir + num_ponts_esq; i++)
        vet_aux[i] = ponts_dir[i - num_ponts_esq];

    return vet_aux;
}

int tenta_redistribuir(FILE* arq_arv, no_t* no_atual, int rrn_atual, int ind_ponteiro_filho,
                       no_t* no_filho, int rrn_filho, int flag_esquerda) {
    int ind_ponteiro_irmao = ind_ponteiro_filho + (flag_esquerda ? -1 : 1);
    if (ind_ponteiro_irmao < 0 || ind_ponteiro_irmao > no_atual->num_chaves) return 0;

    no_t* no_irmao = pega_no_rrn(arq_arv, no_atual->ponteiro[ind_ponteiro_irmao]);
    if (no_irmao == NULL) return -1;

    if (no_irmao->num_chaves == ORDEM_ARVORE - 1) {
        libera_memo(1, no_irmao);
        return 0;
    }

    no_t* no_esquerda = flag_esquerda ? no_irmao : no_filho;
    int ind_pont_esq = flag_esquerda ? ind_ponteiro_irmao : ind_ponteiro_filho;
    int rrn_esq = flag_esquerda ? no_atual->ponteiro[ind_ponteiro_irmao] : rrn_filho;
    no_t* no_direita = flag_esquerda ? no_filho : no_irmao;
    int rrn_dir = flag_esquerda ? rrn_filho : no_atual->ponteiro[ind_ponteiro_irmao];

    dados_int_t* todas_chaves =
        cria_vet_chaves(&(no_atual->chaves[ind_pont_esq]), no_esquerda, no_direita);
    if (todas_chaves == NULL) {
        libera_memo(1, no_irmao);
        return -1;
    }

    int* todos_ponts = cria_vet_ponts(no_esquerda->ponteiro, no_esquerda->num_chaves + 1,
                                      no_direita->ponteiro, no_direita->num_chaves + 1);

    if (todos_ponts == NULL) {
        libera_memo(2, todas_chaves, no_irmao);
        return -1;
    }

    int tot_chaves = no_esquerda->num_chaves + 1 + no_direita->num_chaves;
    int tamanho_esq = (tot_chaves) / 2;
    int tamanho_dir = (tot_chaves - 1) / 2;

    for (int i = 0; i < tamanho_esq; i++) {
        copia_dado_gen((void*)(&(no_esquerda->chaves[i])), (void*)(&(todas_chaves[i])), 0);
        no_esquerda->ponteiro[i] = todos_ponts[i];
    }
    no_esquerda->ponteiro[tamanho_esq] = todos_ponts[tamanho_esq];
    setam1_chaves(no_esquerda, tamanho_esq, no_esquerda->num_chaves - 1);
    setam1_ponts(no_esquerda, tamanho_esq + 1, no_esquerda->num_chaves);
    no_esquerda->num_chaves = tamanho_esq;

    copia_dado_gen((void*)(&(no_atual->chaves[ind_pont_esq])),
                   (void*)(&(todas_chaves[tamanho_esq])), 0);

    for (int i = 0; i < tamanho_dir; i++) {
        copia_dado_gen((void*)(&(no_direita->chaves[i])),
                       (void*)(&(todas_chaves[tamanho_esq + 1 + i])), 0);
        no_direita->ponteiro[i] = todos_ponts[tamanho_esq + 1 + i];
    }
    no_direita->ponteiro[tamanho_dir] = todos_ponts[tamanho_esq + tamanho_dir + 1];
    setam1_chaves(no_direita, tamanho_dir, no_direita->num_chaves - 1);
    setam1_ponts(no_direita, tamanho_dir + 1, no_direita->num_chaves);
    no_direita->num_chaves = tamanho_dir;

    escreve_no_rrn(arq_arv, no_atual, rrn_atual);
    escreve_no_rrn(arq_arv, no_esquerda, rrn_esq);
    escreve_no_rrn(arq_arv, no_direita, rrn_dir);

    libera_memo(4, no_irmao, no_filho, todas_chaves, todos_ponts);

    return 1;
}

int split2p3(FILE* arq_arv, cab_arvb_t* cab_arvb, no_t* no_atual, int rrn_atual,
             int ind_ponteiro_filho, no_t* no_filho, int rrn_filho) {
    int ind_ponteiro_irmao = ind_ponteiro_filho == no_atual->num_chaves ? ind_ponteiro_filho - 1
                                                                        : ind_ponteiro_filho + 1;

    no_t* no_irmao = pega_no_rrn(arq_arv, no_atual->ponteiro[ind_ponteiro_irmao]);
    if (no_irmao == NULL) return 0;

    int flag_esquerda = ind_ponteiro_irmao < ind_ponteiro_filho;

    no_t* no_esquerda = flag_esquerda ? no_irmao : no_filho;
    int ind_pont_esq = flag_esquerda ? ind_ponteiro_irmao : ind_ponteiro_filho;
    int rrn_esq = flag_esquerda ? no_atual->ponteiro[ind_ponteiro_irmao] : rrn_filho;
    no_t* no_direita = flag_esquerda ? no_filho : no_irmao;
    int rrn_dir = flag_esquerda ? rrn_filho : no_atual->ponteiro[ind_ponteiro_irmao];

    dados_int_t* todas_chaves =
        cria_vet_chaves(&(no_atual->chaves[ind_pont_esq]), no_esquerda, no_direita);
    if (todas_chaves == NULL) {
        libera_memo(1, no_irmao);
        return 0;
    }

    int* todos_ponts = cria_vet_ponts(no_esquerda->ponteiro, no_esquerda->num_chaves + 1,
                                      no_direita->ponteiro, no_direita->num_chaves + 1);

    if (todos_ponts == NULL) {
        libera_memo(2, todas_chaves, no_irmao);
        return 0;
    }

    no_t* novo_no_direita = aloca_no(); // terceiro no da esquerda pra direita
    if (novo_no_direita == NULL) {
        libera_memo(3, todas_chaves, todos_ponts, no_irmao);
        return 0;
    }

    int tam_esq = (2 * ORDEM_ARVORE - 2) / 3 + (ORDEM_ARVORE % 3 != 0 ? 1 : 0); // 3
    int tam_dir = (2 * ORDEM_ARVORE - 2) / 3 + (ORDEM_ARVORE % 3 == 2 ? 1 : 0); // 3
    int tam_novo_dir = (2 * ORDEM_ARVORE - 2) / 3;                              // 2

    setam1_chaves(no_esquerda, tam_esq, no_esquerda->num_chaves - 1);
    setam1_ponts(no_esquerda, tam_esq + 1, no_esquerda->num_chaves);
    no_esquerda->num_chaves = tam_esq;

    for (int i = 0; i < tam_dir; i++) {
        copia_dado_gen((void*)(&(no_direita->chaves[i])), (void*)(&(todas_chaves[i + tam_esq + 1])),
                       0);
        no_direita->ponteiro[i] = todos_ponts[i + tam_esq + 1];
    }
    no_direita->ponteiro[tam_dir] = todos_ponts[tam_dir + tam_esq + 1];
    setam1_chaves(no_direita, tam_dir, no_direita->num_chaves - 1);
    setam1_ponts(no_direita, tam_dir + 1, no_direita->num_chaves);
    no_direita->num_chaves = tam_dir;

    novo_no_direita->nivel = no_esquerda->nivel;
    novo_no_direita->num_chaves = tam_novo_dir;
    for (int i = 0; i < tam_novo_dir; i++) {
        copia_dado_gen((void*)(&(novo_no_direita->chaves[i])),
                       (void*)(&(todas_chaves[tam_dir + tam_esq + 2 + i])), 0);
        novo_no_direita->ponteiro[i] = todos_ponts[tam_dir + tam_esq + 2 + i];
    }
    novo_no_direita->ponteiro[tam_novo_dir] = todos_ponts[tam_novo_dir + tam_dir + tam_esq + 2];

    copia_dado_gen((void*)(&(no_atual->chaves[ind_pont_esq])), (void*)(&(todas_chaves[tam_esq])),
                   0);

    for (int i = no_atual->num_chaves + 1; i >= ind_pont_esq + 3; i--) {
        no_atual->ponteiro[i] = no_atual->ponteiro[i - 1];
    }
    no_atual->ponteiro[ind_pont_esq + 2] = cab_arvb->proxRRN;
    insere_chave_no(no_atual, &(todas_chaves[tam_esq + 1 + tam_dir]));

    escreve_no_rrn(arq_arv, no_esquerda, rrn_esq);
    escreve_no_rrn(arq_arv, no_direita, rrn_dir);
    escreve_no_rrn(arq_arv, novo_no_direita, cab_arvb->proxRRN++);
    if (no_atual->num_chaves < ORDEM_ARVORE) escreve_no_rrn(arq_arv, no_atual, rrn_atual);

    libera_memo(4, todas_chaves, todos_ponts, no_irmao, novo_no_direita);
    return 1;
}

int insere_chave_arvb(FILE* arq_arv, cab_arvb_t* cab_arvb, dados_int_t* chave, no_t* no, int rrn) {

    if (rrn == -1) {
        // arvore vazia
        no_t* nova_raiz = aloca_no();
        if (nova_raiz == NULL) {
            return 0;
        }
        copia_dado_gen((void*)(&(nova_raiz->chaves[0])), (void*)chave, 0);
        nova_raiz->nivel = 1;
        nova_raiz->num_chaves = 1;
        cab_arvb->noRaiz = cab_arvb->proxRRN++;
        cab_arvb->nroNiveis = 1;

        escreve_no_arvb(arq_arv, nova_raiz);
        libera_memo(1, nova_raiz);

        return 1;
    }

    if (no->ponteiro[0] == -1) { // folha

        insere_chave_no(no, chave);

        if (no->num_chaves < ORDEM_ARVORE) {
            escreve_no_rrn(arq_arv, no, rrn);
            return 1; // tudo certo
        }
        // se chegou aqui, entao folha encheu
        // se for raiz, faz split1p2
        // se não, manda sinal pra o pai
        if (cab_arvb->noRaiz == rrn) {
            if (split1p2(arq_arv, cab_arvb, no) == 1)
                return 1;
            else
                return 0;
        } else {
            return 2; // folha nao raiz encheu, manda sinal pro pai!
        }
    } else {
        int resp_busca_bin = busca_bin_no(no, chave);
        if (resp_busca_bin == -1) resp_busca_bin = no->num_chaves;
        int prox_rrn = no->ponteiro[resp_busca_bin];
        no_t* prox_no = pega_no_rrn(arq_arv, prox_rrn);
        if (prox_no == NULL) return 0;

        int feedback_insercao = insere_chave_arvb(arq_arv, cab_arvb, chave, prox_no, prox_rrn);
        if (feedback_insercao <= 1) {
            libera_memo(1, prox_no);
            return feedback_insercao;
        }

        // feedback = 2, entao filho encheu
        // checa se da pra fazer redistribuição na esquerda ou na direita
        for (int i = 1; i >= 0; i--) {
            int feedback_reds =
                tenta_redistribuir(arq_arv, no, rrn, resp_busca_bin, prox_no, prox_rrn, i);
            if (feedback_reds == -1) {
                libera_memo(1, prox_no);
                return 0;
            } else if (feedback_reds == 1) {
                return 1;
            }
        }

        // se chegou aqui, nao da pra fazer redistribuição
        // entao tem que ser split2p3 (com prioridade pra esquerda)

        if (split2p3(arq_arv, cab_arvb, no, rrn, resp_busca_bin, prox_no, prox_rrn) == 0) {
            libera_memo(1, prox_no);
            return 0;
        }

        // se tiver enchido por conta do split2p3...
        if (no->num_chaves == ORDEM_ARVORE) {
            // entao se for raiz, faz split1p2
            if (cab_arvb->noRaiz == rrn) {
                libera_memo(1, prox_no);
                if (split1p2(arq_arv, cab_arvb, no) == 1)
                    return 1;
                else
                    return 0;
            }
            // e se nao for, retorna 2 informando que esta sobrecarregado
            return 2;
        }

        // se não tiver enchido, ok!
        return 1;
    }
}

int insere_crime_arvb(FILE* arq_arv_b, cab_arvb_t* cab_arvb, char* nome_campo, crime_t* crime,
                      long long int byteOffset) {

    cab_arvb->nroChaves++;

    dados_int_t* chave = pega_dado_int(crime, nome_campo, byteOffset);
    if (chave == NULL) return 0;

    no_t* no_raiz = NULL;
    if (cab_arvb->noRaiz != -1) {
        no_raiz = pega_no_rrn(arq_arv_b, cab_arvb->noRaiz);

        if (no_raiz == NULL) {
            libera_memo(1, chave);
            return 0;
        }
    }

    if (insere_chave_arvb(arq_arv_b, cab_arvb, chave, no_raiz, cab_arvb->noRaiz) == 0) {
        libera_memo(2, chave, no_raiz);
        return 0;
    }

    desloca_offset(arq_arv_b, 0);
    escreve_cab_arvb(arq_arv_b, cab_arvb);

    libera_memo(2, chave, no_raiz);
    return 1;
}