#include "escrita_bin.h"

/**
 * Escreve o cabeçalho do arquivo binário no disco.
 *
 * @param arq_bin Arquivo binário que terá seu cabeçalho escrito.
 * @param cabecalho Ponteiro para o cabeçalho que será escrito no arquivo.
 */
void escreve_cabecalho(FILE* arq_bin, cabecalho_t* cabecalho) {
    fwrite(&(cabecalho->status), sizeof(char), 1, arq_bin);
    fwrite(&(cabecalho->proxByteOffset), sizeof(long long int), 1, arq_bin);
    fwrite(&(cabecalho->nroRegArq), sizeof(int), 1, arq_bin);
    fwrite(&(cabecalho->nroRegRem), sizeof(int), 1, arq_bin);
}

/**
 * Escreve um registro de crime no arquivo binário no disco.
 *
 * @param arq_bin Arquivo binário que terá o registro escrito.
 * @param crime Ponteiro para a estrutura que contém os dados do registro a ser escrito.
 */
void escreve_registro_criminal(FILE* arq_bin, crime_t* crime) {
    // Escreve campos fixos contiguamente
    fwrite(&(crime->removido), sizeof(char), 1, arq_bin);
    fwrite(&(crime->idCrime), sizeof(int), 1, arq_bin);
    fwrite(crime->dataCrime, sizeof(char), 10, arq_bin);
    fwrite(&(crime->numeroArtigo), sizeof(int), 1, arq_bin);
    fwrite(crime->marcaCelular, sizeof(char), 12, arq_bin);

    // Escreve campos variáveis com delimitadores e sem \0
    char delimitador = '|';
    fwrite(crime->lugarCrime, sizeof(char), (int) strlen(crime->lugarCrime), arq_bin);
    fwrite(&delimitador, sizeof(char), 1, arq_bin);
    fwrite(crime->descricaoCrime, sizeof(char), (int) strlen(crime->descricaoCrime), arq_bin);
    fwrite(&delimitador, sizeof(char), 1, arq_bin);
    delimitador = '#';
    fwrite(&delimitador, sizeof(char), 1, arq_bin);
}

void remocao_logica(FILE* arq_bin, crime_t* crime, cabecalho_t* cabecalho, long long int byteOffset) {
    desloca_offset(arq_bin, byteOffset);
    crime->removido = '1';
    cabecalho->nroRegRem++;
    escreve_registro_criminal(arq_bin, crime);
}

void sobrescreve_crime(FILE* arq_bin, long long int byteOffset, int tamanho_crime_antigo, crime_t* crime_novo) {
    // se chamei aqui tenho a garantia de que o tamanho do crime antigo eh menor ou igual
    desloca_offset(arq_bin, byteOffset);
    
    // Escreve campos fixos contiguamente
    fwrite(&(crime_novo->removido), sizeof(char), 1, arq_bin);
    fwrite(&(crime_novo->idCrime), sizeof(int), 1, arq_bin);
    fwrite(crime_novo->dataCrime, sizeof(char), 10, arq_bin);
    fwrite(&(crime_novo->numeroArtigo), sizeof(int), 1, arq_bin);
    fwrite(crime_novo->marcaCelular, sizeof(char), 12, arq_bin);

    // Escreve campos variáveis com delimitadores e sem \0
    char delimitador = '|';
    fwrite(crime_novo->lugarCrime, sizeof(char), (int) strlen(crime_novo->lugarCrime), arq_bin);
    fwrite(&delimitador, sizeof(char), 1, arq_bin);
    fwrite(crime_novo->descricaoCrime, sizeof(char), (int) strlen(crime_novo->descricaoCrime), arq_bin);
    fwrite(&delimitador, sizeof(char), 1, arq_bin);

    // aqui tem lixo
    delimitador = '$';
    int tamanho_crime_novo = tamanho_crime(crime_novo);
    for (int i = tamanho_crime_antigo; i < tamanho_crime_novo; i++)
        fwrite(&delimitador, sizeof(char), 1, arq_bin);

    delimitador = '#';
    fwrite(&delimitador, sizeof(char), 1, arq_bin);
}