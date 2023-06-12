#include "./utils.h"
#include <ctype.h>
#include <string.h>

// arquivo com diversas funções utilitárias usadas ao longo de todos os arquivos

/**
 * Função fornecida
 *
 * @param nomeArquivoBinario
 */
void binarioNaTela(char* nomeArquivoBinario) {
    /*
     * Você não precisa entender o código dessa função.
     *
     * Use essa função para comparação no run.codes.
     * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
     * Ela vai abrir de novo para leitura e depois fechar
     * (você não vai perder pontos por isso se usar ela).
     */

    unsigned long i, cs;
    unsigned char* mb;
    size_t fl;
    FILE* fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr, "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                        "não foi possível abrir o arquivo que me passou para leitura. "
                        "Ele existe e você tá passando o nome certo? Você lembrou de "
                        "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char*)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}

/**
 * Função fornecida
 *
 * @param str string em que e guardada o resultado do escaneamento
 */
void scan_quote_string(char* str) {
    /*
     * Use essa função para ler um campo string delimitado entre aspas (").
     * Chame ela na hora que for ler tal campo. Por exemplo:
     *
     * A entrada está da seguinte forma:
     *    nomeDoCampo "MARIA DA SILVA"
     *
     * Para ler isso para as strings já alocadas str1 e str2 do seu programa,
     * você faz:
     *    scanf("%s", str1); // Vai salvar nomeDoCampo em str1
     *    scan_quote_string(str2); // Vai salvar MARIA DA SILVA em str2
     *                             // (sem as aspas)
     */

    char R;

    while ((R = getchar()) != EOF && isspace(R)) {
        ; // ignorar espaços, \r, \n...
    }

    if (R == 'N' || R == 'n') { // campo NULO
        getchar();
        getchar();
        getchar();       // ignorar o "ULO" de NULO.
        strcpy(str, ""); // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) { // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();         // ignorar aspas fechando
    } else if (R != EOF) { // vc tá tentando ler uma string
                           // que não tá entre aspas!
                           // Fazer leitura normal %s então,
                           // pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else { // EOF
        strcpy(str, "");
    }
}

/**
 * Função que imprime na tela que ocorreu um erro
 */
void erro() { printf("Falha no processamento do arquivo.\n"); }

/**
 * Função que dá free em cada lugar do vetor até uma certa posição e depois da free
 * no ponteiro associado ao vetor em si
 * @param vetor vetor generico
 * @param pos posicao ate a qual eu limpo
 */
void libera_vetor_ate_pos(void** vetor, int pos) {
    for (int i = 0; i <= pos; i++)
        free(vetor[i]);
    free(vetor);
}

/**
 * Usada na funcao realiza_consultas diversas vezes
 * Libera memoria e fecha arquivos
 *
 * @param flag_erro se =0, nao chama erro() caso algo aconteca. se=1, chama
 * @param arq_bin
 * @param cabecalho
 * @param arq_idx
 * @param cabecalho_indice
 * @param campos
 * @param num_campos
 * @param dados
 * @param num_dados
 */
void libera_memo_consultas(int flag_erro, FILE* arq_bin, cabecalho_t* cabecalho, FILE* arq_idx,
                           cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos,
                           int num_campos, void** dados, int num_dados) {

    if (cabecalho_indice != NULL) free(cabecalho_indice);
    if (arq_idx != NULL) fclose(arq_idx);
    if (dados != NULL) libera_vetor_ate_pos(dados, num_dados - 1);
    if (cabecalho != NULL) free(cabecalho);
    if (arq_bin != NULL) fclose(arq_bin);
    if (campos != NULL) libera_vetor_ate_pos((void**)campos, num_campos - 1);

    if (flag_erro) erro();
}

/**
 * TODO
 */
void fecha_arquivos(int num_arqs, ...) {
    va_list argumentos;
    va_start(argumentos, num_arqs);
    FILE* arq_atual = NULL;
    for (int i = 0; i < num_arqs; i++) {
        arq_atual = va_arg(argumentos, FILE*);
        if (arq_atual != NULL) fclose(arq_atual);
    }
    va_end(argumentos);
}

/**
 * TODO
 */
void libera_memo(int num_ponts, ...) {
    va_list argumentos;
    va_start(argumentos, num_ponts);
    void* pont_atual = NULL;
    for (int i = 0; i < num_ponts; i++) {
        pont_atual = va_arg(argumentos, void*);
        if (pont_atual != NULL) free(pont_atual);
    }
}

/**
 * TODO
 */
void libera_arr(void** arr, int tamanho) {
    for (int i = 0; i < tamanho; i++)
        free(arr[i]);
    free(arr);
}

/**
 * Faz um fseek para um offset específico
 * @param arq_bin
 * @param byteOffset
 */
void desloca_offset(FILE* arq_bin, long long int byteOffset) {
    long long int posicao = ftell(arq_bin);
    fseek(arq_bin, byteOffset - posicao, SEEK_CUR);
}

/**
 * Abre arquivo binário, de índices e seus cabeçalhos. Lida com erros
 * @param arq_bin
 * @param nome_arq_bin
 * @param arq_idx
 * @param cabecalho
 * @param cabecalho_indice
 * @param dados
 * @param tipoVar
 * @param num_ind
 * @return 0 se ocorreu erro, 1 se deu tudo certo
 */
int abre_arq_bin_ind(FILE** arq_bin, char* nome_arq_bin, FILE** arq_idx, char* nome_arq_idx,
                     cabecalho_t** cabecalho, cabecalho_indice_t** cabecalho_indice, void*** dados,
                     int tipoVar, int* num_ind) {
    *arq_bin = fopen(nome_arq_bin, "rb+");
    if (*arq_bin == NULL) {
        erro();
        return 0;
    }

    *arq_idx = fopen(nome_arq_idx, "rb+");
    if (*arq_idx == NULL) {
        fclose(*arq_idx);
        erro();
        return 0;
    }

    *cabecalho = le_cabecalho_bin(*arq_bin);
    if (*cabecalho == NULL || (*cabecalho)->status == '0') {
        if (*cabecalho != NULL) free(*cabecalho);
        fclose(*arq_bin);
        fclose(*arq_idx);
        erro();
        return 0;
    }

    *cabecalho_indice = NULL;
    *dados = NULL;
    if (le_arq_indices(*arq_idx, dados, tipoVar, cabecalho_indice, num_ind) <= 0) {
        fclose(*arq_bin);
        fclose(*arq_idx);
        free(*cabecalho);
        return 0;
    }

    return 1;
}

int abre_arq_bin_arv(FILE** arq_bin, char* nome_arq_bin, FILE** arq_arv, char* nome_arq_arv,
                     cabecalho_t** cab_arq_bin, cab_arvb_t** cab_arvb) {
    *arq_bin = fopen(nome_arq_bin, "rb+");
    if (*arq_bin == NULL) {
        return 0;
    }
    *cab_arq_bin = le_cabecalho_bin(*arq_bin);
    if (*cab_arq_bin == NULL || (*cab_arq_bin)->status == '0') {
        fecha_arquivos(1, *arq_bin);
        libera_memo(1, *cab_arq_bin);
        return 0;
    }

    *arq_arv = fopen(nome_arq_arv, "rb");
    if (*arq_arv == NULL) {
        fecha_arquivos(1, *arq_bin);
        libera_memo(1, *cab_arq_bin);
        return 0;
    }
    *cab_arvb = le_cab_arvb(*arq_arv);
    if (*cab_arvb == NULL || (*cab_arvb)->status == '0') {
        fecha_arquivos(2, *arq_bin, *arq_arv);
        libera_memo(2, *cab_arvb, *cab_arq_bin);
        return 0;
    }

    return 1;
}