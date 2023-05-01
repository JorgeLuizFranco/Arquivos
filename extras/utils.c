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

int compara_string_limitada(char* s1, char* s2, int tamanho, int flag_s2_dinamica) {
    int i = 0;
    for (; i < tamanho && s2[i] != '\0'; i++)
        if (s1[i] > s2[i]) {
            return -1;
        } else if (s1[i] < s2[i]) {
            return 1;
        }
    if (i == tamanho) {
        if (flag_s2_dinamica) {
            if (s2[i] == '\0') {
                return 0;
            } else {
                return 1;
            }
        } else {
            return 0;
        }
    } else {
        return -1;
    }

}