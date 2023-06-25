#include "./arvb_insercao.h"

/**
 * Função que insere chave de busca no vetor de chaves de busca do nó
 *
 * @param no ponteiro para nó em que a inserção será feita
 * @param chave ponteiro para chave de busca a ser inserida
 */
void insere_chave_no(no_t* no, dados_int_t* chave) {

    int resp_busca_bin = busca_bin_no(no, chave);

    if (resp_busca_bin != -1) { // no->chaves[resp_busca_bin] >= chave
        // faz um shift no vetor de chaves e incrementa número de chaves
        for (int i = no->num_chaves++; i > resp_busca_bin; i--) {
            copia_dado_gen((void*)(&(no->chaves[i])), (void*)(&(no->chaves[i - 1])), 0);
        }
    } else {
        // não tem nenhum cara >= chave, então insiro no final
        resp_busca_bin = no->num_chaves++;
    }

    // no caso de ter entrado no if, eu não defini essa posição
    // no caso de ter entrado no else, vou definir a posição final do vetor
    copia_dado_gen((void*)(&(no->chaves[resp_busca_bin])), chave, 0);
}

/**
 * Faz o split1p2 em um nó especificado
 *
 * @param arq_arvb ponteiro para arquivo de índices de árvore B*
 * @param cab_arvb ponteiro para cabeçalho de árvore B*
 * @param no ponteiro para nó (raiz) em que será feito o split1p2. É garantido que tem 5 chaves
 * (está cheio)
 * @return 0 em caso de erro; 1 em caso de sucesso
 */
int split1p2(FILE* arq_arvb, cab_arvb_t* cab_arvb, no_t* no) {
    cab_arvb->nroNiveis++; // quando split1p2 ocorre, o número de níveis aumenta

    /** Antes era
     *      0
     *     /  \
     *
     * Agora e
     *
     *     2
     *   /   \
     *  0     1
     *
     * entao precisa criar o rrn1 e o rrn2
     */

    no_t* rrn1 = aloca_no();
    if (rrn1 == NULL) return 0;
    no_t* rrn2 = aloca_no();
    if (rrn2 == NULL) {
        libera_memo(1, rrn1);
        return 0;
    }

    // Preenche dados do nó de rrn1
    rrn1->nivel = no->nivel;
    rrn1->num_chaves = (ORDEM_ARVORE - 1) / 2;

    for (int i = 0; i < rrn1->num_chaves; i++) {
        copia_dado_gen((void*)(&(rrn1->chaves[i])),
                       (void*)(&(no->chaves[ORDEM_ARVORE / 2 + 1 + i])), 0);
        rrn1->ponteiro[i] = no->ponteiro[ORDEM_ARVORE / 2 + 1 + i];
    }
    rrn1->ponteiro[rrn1->num_chaves] = no->ponteiro[ORDEM_ARVORE / 2 + 1 + rrn1->num_chaves];
    // depois de preencher dados, escreve no arquivo de índice
    escreve_no_rrn(arq_arvb, rrn1, cab_arvb->proxRRN++);

    // Mesma coisa pro 2
    rrn2->nivel = ++(no->nivel);
    rrn2->num_chaves = 1;
    rrn2->chaves[0] = no->chaves[ORDEM_ARVORE / 2];
    rrn2->ponteiro[0] = cab_arvb->noRaiz;      // aponta pra antiga raiz na esquerda
    rrn2->ponteiro[1] = cab_arvb->proxRRN - 1; // aponta pro novo no que foi escrito agora
    escreve_no_rrn(arq_arvb, rrn2, cab_arvb->proxRRN++);

    // agora raiz nao vai ser mais raiz
    // entao primeiro preencho como -1 as chaves que nao tem mais
    // depois atualiza outras coisas
    setam1_chaves(no, ORDEM_ARVORE / 2, no->num_chaves - 1);
    setam1_ponts(no, ORDEM_ARVORE / 2 + 1, no->num_chaves);
    no->num_chaves = ORDEM_ARVORE / 2;
    no->nivel--;
    escreve_no_rrn(arq_arvb, no, cab_arvb->noRaiz);

    cab_arvb->noRaiz = cab_arvb->proxRRN - 1; // nova raiz

    libera_memo(2, rrn1, rrn2);

    return 1;
}

/**
 * Para auxiliar na inserção, essa função cria um vetor auxiliar de chaves
 * que tem todas as chaves do no_esq, da chave, no_dir nessa ordem (ocorre concatenação e cópia
 * delas)
 *
 * @param chave chave em questão
 * @param no_esq nó da esquerda
 * @param no_dir nó da direita
 * @return ponteiro para primeiro elemento de vetor. NULL se houver falha em alocar
 */
dados_int_t* cria_vet_chaves(dados_int_t* chave, no_t* no_esq, no_t* no_dir) {
    dados_int_t* vet_aux =
        (dados_int_t*)malloc(sizeof(dados_int_t) * (1 + no_esq->num_chaves + no_dir->num_chaves));
    if (vet_aux == NULL) return NULL;

    // literalmente copia todo mundo pra o vetor que foi alocado
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

/**
 * Como na função cria_vet_chaves, faz a mesma coisa, só que para rrns de dois vetores
 *
 * @param ponts_esq ponteiro para primeiro elementor de vetor de rrns
 * @param num_ponts_esq número de elementos desse vetor
 * @param ponts_dir análogo
 * @param num_ponts_dir análogo
 * @return ponteiro para primeiro elemento do vetor alocado. NULL se houver falha em alocar
 */
int* cria_vet_ponts(int* ponts_esq, int num_ponts_esq, int* ponts_dir, int num_ponts_dir) {
    int* vet_aux = (int*)malloc(sizeof(int) * (num_ponts_esq + num_ponts_dir));

    int i = 0;
    for (; i < num_ponts_esq; i++)
        vet_aux[i] = ponts_esq[i];

    for (; i < num_ponts_dir + num_ponts_esq; i++)
        vet_aux[i] = ponts_dir[i - num_ponts_esq];

    return vet_aux;
}

/**
 * Dado um nó e um lado (esquerda ou direita), vê se dá pra redistribuir com nó desse lado
 * e assim rebalancear mais a árvore
 *
 * @param arq_arv ponteiro para arquivo de índice da árvore B*
 * @param no_atual ponteiro para o pai do nó em questão
 * @param rrn_atual rrn desse pai
 * @param ind_ponteiro_filho pai->ponteiros[ind_ponteiro_filho] eh o rrn do nó em questão
 * @param no_filho ponteiro para nó em questão alocado
 * @param rrn_filho rrn desse nó
 * @param flag_esquerda lado para o qual vai tentar redistribuir (1 se esquerda, 0 se direita)
 * @return -1 se ocorreu algum erro; 0 se não dá pra redistribuir para esse lado; 1 se dá para
 * redistribuir e a redistribuição ocorreu com sucesso
 */
int tenta_redistribuir(FILE* arq_arv, no_t* no_atual, int rrn_atual, int ind_ponteiro_filho,
                       no_t* no_filho, int rrn_filho, int flag_esquerda) {

    //  pai->ponteiros[ind_ponteiro_irmao] eh o rrn do irmão do lado especificado
    int ind_ponteiro_irmao = ind_ponteiro_filho + (flag_esquerda ? -1 : 1);
    // se não tiver irmão pra esse lado, n dá pra redistribuir
    if (ind_ponteiro_irmao < 0 || ind_ponteiro_irmao > no_atual->num_chaves) return 0;

    // lê nó irmão
    no_t* no_irmao = pega_no_rrn(arq_arv, no_atual->ponteiro[ind_ponteiro_irmao]);
    if (no_irmao == NULL) return -1;

    // se irmão tiver cheio, não dá pra redistribuir
    if (no_irmao->num_chaves == ORDEM_ARVORE - 1) {
        libera_memo(1, no_irmao);
        return 0;
    }

    // cria novas variáveis (esquerda, direita) esquecendo quem eh no_filho e no_irmao
    // porque assim e mais facil de programar
    no_t* no_esquerda = flag_esquerda ? no_irmao : no_filho;
    int ind_pont_esq = flag_esquerda ? ind_ponteiro_irmao : ind_ponteiro_filho;
    int rrn_esq = flag_esquerda ? no_atual->ponteiro[ind_ponteiro_irmao] : rrn_filho;
    no_t* no_direita = flag_esquerda ? no_filho : no_irmao;
    int rrn_dir = flag_esquerda ? rrn_filho : no_atual->ponteiro[ind_ponteiro_irmao];

    // coloca todas as chaves desses nós num vetor
    dados_int_t* todas_chaves =
        cria_vet_chaves(&(no_atual->chaves[ind_pont_esq]), no_esquerda, no_direita);
    if (todas_chaves == NULL) {
        libera_memo(1, no_irmao);
        return -1;
    }

    // coloca todos os ponteiros desses nós num vetor
    int* todos_ponts = cria_vet_ponts(no_esquerda->ponteiro, no_esquerda->num_chaves + 1,
                                      no_direita->ponteiro, no_direita->num_chaves + 1);

    if (todos_ponts == NULL) {
        libera_memo(2, todas_chaves, no_irmao);
        return -1;
    }

    // tamanho do vetor de chaves
    int tot_chaves = no_esquerda->num_chaves + 1 + no_direita->num_chaves;
    // tamanho que quero que o nó da esquerda após a redistribuição tenha
    int tamanho_esq = (tot_chaves) / 2;
    // mesma coisa pra direita
    int tamanho_dir = (tot_chaves - 1) / 2;

    // sobrescrevo dados do nó da esquerda
    for (int i = 0; i < tamanho_esq; i++) {
        copia_dado_gen((void*)(&(no_esquerda->chaves[i])), (void*)(&(todas_chaves[i])), 0);
        no_esquerda->ponteiro[i] = todos_ponts[i];
    }
    no_esquerda->ponteiro[tamanho_esq] = todos_ponts[tamanho_esq];
    setam1_chaves(no_esquerda, tamanho_esq, no_esquerda->num_chaves - 1);
    setam1_ponts(no_esquerda, tamanho_esq + 1, no_esquerda->num_chaves);
    no_esquerda->num_chaves = tamanho_esq;

    // sobrescrevo dados do nó pai
    copia_dado_gen((void*)(&(no_atual->chaves[ind_pont_esq])),
                   (void*)(&(todas_chaves[tamanho_esq])), 0);

    // sobrescrevo dados do nó da direita
    for (int i = 0; i < tamanho_dir; i++) {
        copia_dado_gen((void*)(&(no_direita->chaves[i])),
                       (void*)(&(todas_chaves[tamanho_esq + 1 + i])), 0);
        no_direita->ponteiro[i] = todos_ponts[tamanho_esq + 1 + i];
    }
    no_direita->ponteiro[tamanho_dir] = todos_ponts[tamanho_esq + tamanho_dir + 1];
    setam1_chaves(no_direita, tamanho_dir, no_direita->num_chaves - 1);
    setam1_ponts(no_direita, tamanho_dir + 1, no_direita->num_chaves);
    no_direita->num_chaves = tamanho_dir;

    // escrevo todos eles
    escreve_no_rrn(arq_arv, no_atual, rrn_atual);
    escreve_no_rrn(arq_arv, no_esquerda, rrn_esq);
    escreve_no_rrn(arq_arv, no_direita, rrn_dir);

    libera_memo(4, no_irmao, no_filho, todas_chaves, todos_ponts);

    return 1;
}

/**
 * Função que realiza split2p3
 *
 * @param arq_arv ponteiro para arquivo de índices de árvore B*
 * @param cab_arvb ponteiro para cabeçalho dessr arquivo
 * @param no_atual nó pai em questão
 * @param ind_ponteiro_filho tal que no_atual->ponteiros[ind_ponteiro_filho] eh o rrn do filho com 5
 * chaves
 * @param no_filho nó filho em RAM
 * @param rrn_filho rrn desse filho
 */
int split2p3(FILE* arq_arv, cab_arvb_t* cab_arvb, no_t* no_atual, int rrn_atual,
             int ind_ponteiro_filho, no_t* no_filho, int rrn_filho) {

    // Faço split2p3 com o irmão da direita dele. Se não tiver irmão da direita, pega o da esquerda
    // sempre vai ter algum irmão, pois no_filho não é raiz e todos os níveis são totalmente
    // preenchidos
    int ind_ponteiro_irmao = ind_ponteiro_filho == no_atual->num_chaves ? ind_ponteiro_filho - 1
                                                                        : ind_ponteiro_filho + 1;
    // coloca o irmão em RAM
    no_t* no_irmao = pega_no_rrn(arq_arv, no_atual->ponteiro[ind_ponteiro_irmao]);
    if (no_irmao == NULL) return 0;

    // 1 se o irmão é pra esquerda, 0 se direita
    int flag_esquerda = ind_ponteiro_irmao < ind_ponteiro_filho;

    // essa flag e so pra ajudar a renomear as variáveis envolvidas para esquerda
    // e direita
    no_t* no_esquerda = flag_esquerda ? no_irmao : no_filho;
    int ind_pont_esq = flag_esquerda ? ind_ponteiro_irmao : ind_ponteiro_filho;
    int rrn_esq = flag_esquerda ? no_atual->ponteiro[ind_ponteiro_irmao] : rrn_filho;
    no_t* no_direita = flag_esquerda ? no_filho : no_irmao;
    int rrn_dir = flag_esquerda ? rrn_filho : no_atual->ponteiro[ind_ponteiro_irmao];

    // armazena todas as chaves em um vetor
    dados_int_t* todas_chaves =
        cria_vet_chaves(&(no_atual->chaves[ind_pont_esq]), no_esquerda, no_direita);
    if (todas_chaves == NULL) {
        libera_memo(1, no_irmao);
        return 0;
    }

    // mesma coisa pra os rrns
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

    // sempre que ocorre split2p3, a distribuição é 3, 3, 2
    // coloquei formulas mais gerais para caso precissasse trocar a ordem da árvore, tivesse que nao
    // alterar o codigo
    int tam_esq = (2 * ORDEM_ARVORE - 2) / 3 + (ORDEM_ARVORE % 3 != 0 ? 1 : 0); // 3
    int tam_dir = (2 * ORDEM_ARVORE - 2) / 3 + (ORDEM_ARVORE % 3 == 2 ? 1 : 0); // 3
    int tam_novo_dir = (2 * ORDEM_ARVORE - 2) / 3;                              // 2

    // limpa todas as chaves e ponteiros irrelevantes
    setam1_chaves(no_esquerda, tam_esq, no_esquerda->num_chaves - 1);
    setam1_ponts(no_esquerda, tam_esq + 1, no_esquerda->num_chaves);
    no_esquerda->num_chaves = tam_esq;

    // atualiza variáveis do nó da direita (que na verdade é do meio considerando o que foi alocado
    // agora)
    for (int i = 0; i < tam_dir; i++) {
        copia_dado_gen((void*)(&(no_direita->chaves[i])), (void*)(&(todas_chaves[i + tam_esq + 1])),
                       0);
        no_direita->ponteiro[i] = todos_ponts[i + tam_esq + 1];
    }
    no_direita->ponteiro[tam_dir] = todos_ponts[tam_dir + tam_esq + 1];
    setam1_chaves(no_direita, tam_dir, no_direita->num_chaves - 1);
    setam1_ponts(no_direita, tam_dir + 1, no_direita->num_chaves);
    no_direita->num_chaves = tam_dir;

    // atualiza variáveis do mais da direita
    novo_no_direita->nivel = no_esquerda->nivel;
    novo_no_direita->num_chaves = tam_novo_dir;
    for (int i = 0; i < tam_novo_dir; i++) {
        copia_dado_gen((void*)(&(novo_no_direita->chaves[i])),
                       (void*)(&(todas_chaves[tam_dir + tam_esq + 2 + i])), 0);
        novo_no_direita->ponteiro[i] = todos_ponts[tam_dir + tam_esq + 2 + i];
    }
    novo_no_direita->ponteiro[tam_novo_dir] = todos_ponts[tam_novo_dir + tam_dir + tam_esq + 2];

    // atualiza chave do nó pai
    copia_dado_gen((void*)(&(no_atual->chaves[ind_pont_esq])), (void*)(&(todas_chaves[tam_esq])),
                   0);

    // shifta os ponteiros dele
    for (int i = no_atual->num_chaves + 1; i >= ind_pont_esq + 3; i--) {
        no_atual->ponteiro[i] = no_atual->ponteiro[i - 1];
    }
    no_atual->ponteiro[ind_pont_esq + 2] = cab_arvb->proxRRN;

    // insere chave nele
    insere_chave_no(no_atual, &(todas_chaves[tam_esq + 1 + tam_dir]));

    // escreve todos eles no arquivo em seus respectivos rrns
    escreve_no_rrn(arq_arv, no_esquerda, rrn_esq);
    escreve_no_rrn(arq_arv, no_direita, rrn_dir);
    escreve_no_rrn(arq_arv, novo_no_direita, cab_arvb->proxRRN++);

    // inclusive se o nó pai nao tiver com 5 eu posso escrever ele logo
    // se nao eu vou ter que balancear ele
    if (no_atual->num_chaves < ORDEM_ARVORE) escreve_no_rrn(arq_arv, no_atual, rrn_atual);

    libera_memo(4, todas_chaves, todos_ponts, no_irmao, novo_no_direita);

    return 1;
}

/**
 * Função que insere chave em árvore B tentando a partir de um dado nó
 *
 * @param arq_arv ponteiro para arquivo de índices de árvore B*
 * @param cab_arvb ponteiro para cabeçalho desse arquivo
 * @param chave ponteiro para chave a ser inserida
 * @param no nó a partir do qual tenta inserir
 * @param rrn rrn desse nó
 * @return 0 se ocorreu algum erro; 1 se inseriu sem problemas; 2 se inseriu mas agora "no" esta
 * sobrecarregado
 */
int insere_chave_arvb(FILE* arq_arv, cab_arvb_t* cab_arvb, dados_int_t* chave, no_t* no, int rrn) {

    if (rrn == -1) {
        // arvore vazia, tenho que criar raiz
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

    if (no->nivel == 1) { // folha

        // insiro chave logo na folha
        insere_chave_no(no, chave);

        // se não tiver enchido eu escrevo e pronto
        if (no->num_chaves < ORDEM_ARVORE) {
            escreve_no_rrn(arq_arv, no, rrn);
            return 1; // tudo certo
        }

        // se chegou aqui, entao folha encheu
        // se for raiz-folha, faz split1p2
        // se não, manda sinal pra o pai de que tá cheio
        if (cab_arvb->noRaiz == rrn) {
            if (split1p2(arq_arv, cab_arvb, no) == 1)
                return 1;
            else
                return 0;
        } else {
            return 2; // folha nao raiz encheu, manda sinal pro pai!
        }
    } else {
        // não é folha; vejo o nó que vou chamar a recursão
        int resp_busca_bin = busca_bin_no(no, chave);
        if (resp_busca_bin == -1) resp_busca_bin = no->num_chaves;

        // pego o rrn do nó e ponteiro dele
        int prox_rrn = no->ponteiro[resp_busca_bin];
        no_t* prox_no = pega_no_rrn(arq_arv, prox_rrn);
        if (prox_no == NULL) return 0;

        // chama recursão
        int feedback_insercao = insere_chave_arvb(arq_arv, cab_arvb, chave, prox_no, prox_rrn);
        if (feedback_insercao <= 1) {
            // se deu 0, retorno que deu erro
            // se deu 1, retorno que deu bom
            libera_memo(1, prox_no);
            return feedback_insercao;
        }

        // feedback = 2, entao filho encheu
        // checa se da pra fazer redistribuição na esquerda ou na direita
        for (int i = 1; i >= 0; i--) {
            // i=1 primeiro (esquerda primeiro) e depois i=0 (direita)
            int feedback_reds =
                tenta_redistribuir(arq_arv, no, rrn, resp_busca_bin, prox_no, prox_rrn, i);

            if (feedback_reds == -1) { // se ocorreu algum erro, retorna 0
                libera_memo(1, prox_no);
                return 0;
            } else if (feedback_reds == 1) { // se deu certo, já balanceou então para
                return 1;
            }
            // se não entrou em nenhum if, feedback=0, ou seja, não tem como redistribuir pra esse
            // lado
        }

        // se chegou aqui, nao da pra fazer redistribuição
        // entao tem que ser split2p3 (com prioridade pra direita)

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

/**
 * Função que insere chave associada a registro criminal em árvore B*
 *
 * @param arq_arv_b ponteiro para arquivo de índice de árvore B*
 * @param cab_arvb ponteiro para cabeçalho desse ídnice
 * @param nome_campo nome do campo indexado pela arvore B*
 * @param crime registro criminal a ter esse campo inserido na árvore B*
 * @param byteOffset byteoffset desse registro criminal
 * @return 0 se ocorreu algum erro, 1 se deu tudo certo
 */
int insere_crime_arvb(FILE* arq_arv_b, cab_arvb_t* cab_arvb, char* nome_campo, crime_t* crime,
                      long long int byteOffset) {

    cab_arvb->nroChaves++; // aumenta número total de chaves

    // cria struct que armazena chave em questão junto com o byteoffset
    dados_int_t* chave = pega_dado_int(crime, nome_campo, byteOffset);
    if (chave == NULL) return 0;

    // se tiver nó raiz, carrega ele em RAM
    no_t* no_raiz = NULL;
    if (cab_arvb->noRaiz != -1) {
        no_raiz = pega_no_rrn(arq_arv_b, cab_arvb->noRaiz);

        if (no_raiz == NULL) {
            libera_memo(1, chave);
            return 0;
        }
    }

    // tenta inserir nessa raiz (pode ser que mande NULL e nesse caso ele cria a raiz)
    if (insere_chave_arvb(arq_arv_b, cab_arvb, chave, no_raiz, cab_arvb->noRaiz) == 0) {
        libera_memo(2, chave, no_raiz);
        return 0;
    }

    // atualiza cabeçalho da arvore B*
    desloca_offset(arq_arv_b, 0);
    escreve_cab_arvb(arq_arv_b, cab_arvb);

    libera_memo(2, chave, no_raiz);

    return 1;
}