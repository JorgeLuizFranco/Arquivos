#include "./leitura_bin.h"

/**
 * Lê o cabeçalho do arquivo binário contendo os metadados do arquivo.
 *
 * @param arq_bin Arquivo binário que será lido.
 * @return Ponteiro para a estrutura contendo os dados do cabeçalho lido.
 *         Retorna NULL caso não seja possível alocar memória para a estrutura.
 */
cabecalho_t* le_cabecalho_bin(FILE* arq_bin) {
    cabecalho_t* cabecalho = (cabecalho_t*)malloc(sizeof(cabecalho_t));

    if (cabecalho == NULL) return NULL;

    fread(&(cabecalho->status), sizeof(char), 1, arq_bin);
    fread(&(cabecalho->proxByteOffset), sizeof(long long int), 1, arq_bin);
    fread(&(cabecalho->nroRegArq), sizeof(int), 1, arq_bin);
    fread(&(cabecalho->nroRegRem), sizeof(int), 1, arq_bin);

    return cabecalho;
}

/**
 * Lê um campo de tamanho variável de um registro de crime no arquivo binário.
 *
 * @param campo Ponteiro para a string onde será armazenado o campo lido.
 * @param arq_bin Arquivo binário que será lido.
 * @param delimitador Caractere que delimita o final do campo no arquivo binário.
 */
void le_campo_variavel_crime(char** campo, FILE* arq_bin, char delimitador) {
    *campo = NULL;
    char caractere_atual = 0;
    int tamanho_string = 0;

    // leitura caractere por caractere com realloc
    do {
        fread(&caractere_atual, sizeof(char), 1, arq_bin);
        tamanho_string++;
        *campo = (char*)realloc(*campo, tamanho_string);
        (*campo)[tamanho_string - 1] = caractere_atual;
    } while (caractere_atual != delimitador);

    (*campo)[tamanho_string - 1] = '\0';
}

/**
 *
 * Lê um registro do arquivo binário e retorna o ponteiro alocado para a struct crime_t contendo a
 * informação lida
 *
 * @param arq_bin o arquivo binário do qual se lê
 * @return um ponteiro para uma sruct crime_t contendo a informação lida (ou NULL caso ocorra falha
 * de alocação)
 */
crime_t* le_crime_bin(FILE* arq_bin) {
    crime_t* crime = (crime_t*)malloc(sizeof(crime_t));

    if (crime == NULL) return NULL;
    char delimitador = '$';

    fread(&(crime->removido), sizeof(char), 1, arq_bin);
    fread(&(crime->idCrime), sizeof(int), 1, arq_bin);
    fread(crime->dataCrime, sizeof(char), 10, arq_bin);
    fread(&(crime->numeroArtigo), sizeof(int), 1, arq_bin);
    fread(crime->marcaCelular, sizeof(char), 12, arq_bin);

    le_campo_variavel_crime(&(crime->lugarCrime), arq_bin, '|');
    le_campo_variavel_crime(&(crime->descricaoCrime), arq_bin, '|');

    crime->tamanho_real = tamanho_crime(crime) -
                          1; // por enquanto eh isso -1 (pq n contei # ainda) mas vai aumentando

    // ler lixo ($), apenas para avançar o ponteiro
    while (delimitador != '#') {
        fread(&delimitador, sizeof(char), 1, arq_bin); // $ ou #
        crime->tamanho_real++;
    }
    return crime;
}

/**
 * Lê crime com offset específico
 *
 * @param arq_bin
 * @param byteOffset
 * @return crime no offset especificado
 */
crime_t* le_crime_bin_offset(FILE* arq_bin, long long int byteOffset) {
    desloca_offset(arq_bin, byteOffset);
    return le_crime_bin(arq_bin);
}