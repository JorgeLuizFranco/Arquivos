#include "utils.h"
#include<string.h>
#include <ctype.h>


// Função fornecida
void binarioNaTela(char *nomeArquivoBinario) {
    /*
     * Você não precisa entender o código dessa função.
     *
     * Use essa função para comparação no run.codes.
     * Lembre-se de ter fechado (fclose) o arquivo anteriormente.
     * Ela vai abrir de novo para leitura e depois fechar
     * (você não vai perder pontos por isso se usar ela).
     */

    unsigned long i, cs;
    unsigned char *mb;
    size_t fl;
    FILE *fs;
    if (nomeArquivoBinario == NULL || !(fs = fopen(nomeArquivoBinario, "rb"))) {
        fprintf(stderr,
                "ERRO AO ESCREVER O BINARIO NA TELA (função binarioNaTela): "
                "não foi possível abrir o arquivo que me passou para leitura. "
                "Ele existe e você tá passando o nome certo? Você lembrou de "
                "fechar ele com fclose depois de usar?\n");
        return;
    }
    fseek(fs, 0, SEEK_END);
    fl = ftell(fs);
    fseek(fs, 0, SEEK_SET);
    mb = (unsigned char *)malloc(fl);
    fread(mb, 1, fl, fs);

    cs = 0;
    for (i = 0; i < fl; i++) {
        cs += (unsigned long)mb[i];
    }
    printf("%lf\n", (cs / (double)100));
    free(mb);
    fclose(fs);
}


void scan_quote_string(char *str) {
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
        ;  // ignorar espaços, \r, \n...
    }

    if (R == 'N' || R == 'n') {  // campo NULO
        getchar();
        getchar();
        getchar();  // ignorar o "ULO" de NULO.
        strcpy(str, "");  // copia string vazia
    } else if (R == '\"') {
        if (scanf("%[^\"]", str) != 1) {  // ler até o fechamento das aspas
            strcpy(str, "");
        }
        getchar();  // ignorar aspas fechando
    } else if (R != EOF) {  // vc tá tentando ler uma string
                            // que não tá entre aspas!
                            // Fazer leitura normal %s então,
                            // pois deve ser algum inteiro ou algo assim...
        str[0] = R;
        scanf("%s", &str[1]);
    } else {  // EOF
        strcpy(str, "");
    }
}


/**
 * Função que imprime na tela que ocorreu um erro
*/
void erro() {
    printf("Falha no processamento do arquivo.\n");
}

void libera_vetor_ate_pos(void** vetor, int pos) {
    for (int i = 0; i <= pos; i++)
        free(vetor[i]);
    free(vetor);
}

int checa_existencia_arquivo(char* nome_arquivo) {
    FILE* f = fopen(nome_arquivo, "rb");
    if (f == NULL) return 1;
    fclose(f);
    return 0;
}

void libera_memo_consultas(int flag_erro, FILE* arq_bin, cabecalho_t* cabecalho, FILE* arq_idx, cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos, int num_campos, void** dados, int num_dados) {

    if (cabecalho_indice != NULL) free(cabecalho_indice);
    if (arq_idx != NULL) fclose(arq_idx);    
    if (dados != NULL) libera_vetor_ate_pos(dados, num_dados-1);
    if (cabecalho != NULL) free(cabecalho);
    if (arq_bin != NULL) fclose(arq_bin);
    if (campos != NULL) libera_vetor_ate_pos((void**)campos, num_campos-1);

    if (flag_erro) erro();
}

void desloca_offset(FILE* arq_bin, long long int byteOffset) {
    long long int posicao = ftell(arq_bin);
    fseek(arq_bin, byteOffset - posicao, SEEK_CUR);
}

int abre_arq_bin_ind(FILE** arq_bin, char* nome_arq_bin, FILE** arq_idx, char* nome_arq_idx, cabecalho_t** cabecalho, cabecalho_indice_t** cabecalho_indice, void*** dados, int tipoVar, int* num_ind) {
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

    *cabecalho = le_cabecalho_bin(arq_bin);
    if (*cabecalho == NULL) {
        fclose(*arq_bin);
        fclose(*arq_idx);
        erro();
        return 0;
    }

    *cabecalho_indice = NULL;
    *dados = NULL;
    if (le_arq_indices(*arq_idx, dados, tipoVar, cabecalho_indice, num_ind) == 0) {
        fclose(*arq_bin);
        fclose(*arq_idx);
        free(*cabecalho);
        return 0;
    }

    return 1;
}

void libera_vars(int qnt, ...) {
    va_list lista_args;
    int j = 0;

    va_start(lista_args, qnt);

    for (j = 0; j < qnt; j++) {
        free(va_arg(lista_args, void*));
    }

    va_end(lista_args);
}
