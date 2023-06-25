#include "./leitura_arvoreb.h"

/**
 *
 * Lê cabeçalho arquivo de índice de arvore B* e guarda em RAM
 *
 * @param arq_arvb arquivo de índice de árvore B*
 * @return ponteiro para cabeçalho alocado. NULL caso ocorra erro de alocação
 *
 */
cab_arvb_t* le_cab_arvb(FILE* arq_arvb) {
    cab_arvb_t* cab_arvb = (cab_arvb_t*)malloc(sizeof(cab_arvb));
    if (cab_arvb == NULL) return NULL;

    fread(&(cab_arvb->status), sizeof(char), 1, arq_arvb);
    fread(&(cab_arvb->noRaiz), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->proxRRN), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->nroNiveis), sizeof(int), 1, arq_arvb);
    fread(&(cab_arvb->nroChaves), sizeof(int), 1, arq_arvb);

    // lê lixo para avançar o ponteiro do arquivo para ler
    // o cabeçalho todo
    // isso é desnecessário porque pra ler nó eu dou fseek
    // mas fica mais legal assim
    char lixo;
    for (int i = 0; i < LIXO_CABECALHO; i++) {
        fread(&lixo, sizeof(char), 1, arq_arvb);
    }

    return cab_arvb;
}

/**
 * Lê o próximo dó no arquivo de índice de árvore B*
 *
 * @param arq_arvb ponteiro para arquivo de índice de árvore B*
 * @return nó alocado e lido. NULL caso ocorra erro de alocação
 *
 */
no_t* le_no_arvb(FILE* arq_arvb) {
    no_t* no = (no_t*)malloc(sizeof(no_t));
    if (no == NULL) return NULL;

    fread(&(no->nivel), sizeof(int), 1, arq_arvb);
    fread(&(no->num_chaves), sizeof(int), 1, arq_arvb);

    for (int i = 0; i < ORDEM_ARVORE - 1; i++) {
        fread(&(no->ponteiro[i]), sizeof(int), 1, arq_arvb);
        fread(&((no->chaves[i]).chaveBusca), sizeof(int), 1, arq_arvb);
        fread(&((no->chaves[i]).byteOffset), sizeof(long long int), 1, arq_arvb);
    }

    fread(&(no->ponteiro[ORDEM_ARVORE - 1]), sizeof(int), 1, arq_arvb);
    setam1_chaves(no, ORDEM_ARVORE - 1, ORDEM_ARVORE - 1);
    setam1_ponts(no, ORDEM_ARVORE, ORDEM_ARVORE);

    return no;
}

/**
 * Lê nó em um dado rrn
 *
 * @param arq_arvb ponteiro para arquivo de árvore B*
 * @param rrn_no rrn do nó a ser lido
 * @return nó lido e alocado. NULL se não tiver espaço
 */
no_t* pega_no_rrn(FILE* arq_arvb, int rrn_no) {
    desloca_offset(arq_arvb, 1LL * TAMANHO_PAGINA_ARVB * (rrn_no + 1));
    return le_no_arvb(arq_arvb);
}
