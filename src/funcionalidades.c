#include "funcionalidades.h"

/**
 * FUNCIONALIDADE 1
 * Lê o CSV contendo os crimes e transcreve sua informação para um arquivo binário.
 *
 * @param nome_arq_csv Nome do arquivo CSV 
 * @param nome_arq_bin Nome do arquivo binário
 */
void csv_para_bin(char* nome_arq_csv, char* nome_arq_bin) {
    
    FILE* arq_csv = fopen(nome_arq_csv, "r");
    if (arq_csv == NULL) {
        erro();
        return;
    }

    // Abre o arquivo binário de modo que ele seja limpo e que seja possível
    // ler e escrever nele
    FILE* arq_bin = fopen(nome_arq_bin, "wb+");
    if (arq_bin == NULL) {
        fclose(arq_csv);
        erro();
        return;
    }

    cabecalho_t* cabecalho = (cabecalho_t*) malloc(sizeof(cabecalho_t));
    if (cabecalho == NULL) {
        fclose(arq_csv);
        fclose(arq_bin);
        erro();
        return;
    }

    cabecalho->nroRegArq = 0;
    cabecalho->nroRegRem = 0;
    cabecalho->proxByteOffset = TAMANHO_CABECALHO; // quanto o registro cabeçalho ocupa
    cabecalho->status = '0'; // status 0 indica que ta escrevendo
    escreve_cabecalho(arq_bin, cabecalho); // vai colocando informações temporárias referentes ao cabeçalho no binário

    // pula a primeira linha do csv
    char pula_primeira_linha = 0;
    while (pula_primeira_linha != '\n')
        fscanf(arq_csv, "%c", &pula_primeira_linha);
    
    // vai lendo crime por crime do csv
    crime_t* crime_lido = NULL;
    while ((crime_lido = le_registro_criminal(arq_csv)) != NULL) {
        escreve_registro_criminal(arq_bin, crime_lido);
        cabecalho->nroRegArq++; // mais um registro
        cabecalho->proxByteOffset += tamanho_crime(crime_lido); // atualiza número de bytes do proxByteOffset
        libera_crime(crime_lido); // libera memória
    }

    // Fecha o csv
    fclose(arq_csv);
    if (crime_lido != NULL && crime_lido->lugarCrime == NULL) {
        libera_crime(crime_lido);
        free(cabecalho);
        fclose(arq_bin);
        erro();
        return;
    }
    
    // volta ao início do binário
    fseek(arq_bin, 0, SEEK_SET);
    cabecalho->status = '1'; // indica que terminou de escrever
    escreve_cabecalho(arq_bin, cabecalho); // escreve o cabeçalho atualizado

    free(cabecalho); // libera o cabeçalho

    fclose(arq_bin); // fecha o binário
    binarioNaTela(nome_arq_bin); // função pedida pra ser executada
}

/**
 * Lê o arquivo binário e imprime as informações
 * FUNCIONALIDADE 2
 *
 * @param nome_arq_bin Nome do arquivo binário.
 */
void mostra_crimes_tela(char* nome_arq_bin) {
    FILE* arq_bin = fopen(nome_arq_bin, "rb");
    if (arq_bin == NULL) {
        erro();
        return;
    }

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        fclose(arq_bin);
        erro();
        return;
    }
    int nro_registros = cabecalho->nroRegArq - cabecalho->nroRegRem;
    free(cabecalho);

    mostra_crimes_tela_arq(arq_bin, nro_registros);

    fclose(arq_bin);
}

/**
 * FUNCIONALIDADE 3
 * Cria um arquivo de índices dado o nome do arquivo binário de registros, o nome do campo,
 * atributo do campo e o arquivo binário de destino
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
*/
void cria_arq_indices(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_ind) {

    FILE* arq_bin = fopen(nome_arq_bin, "rb"); // arquivo de registros
    if (arq_bin == NULL) {
        erro();
        return;
    }

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        fclose(arq_bin);
        erro();
        return;
    }

    int n_registros = cabecalho->nroRegArq - cabecalho->nroRegRem;

    FILE* arq_ind = fopen(nome_arq_ind, "wb");
    if (arq_ind == NULL) {
        fclose(arq_bin);
        free(cabecalho);
        erro();
        return;
    }

    escreve_arq_ind(arq_bin, arq_ind, nome_campo, tipo_campo, n_registros, cabecalho->proxByteOffset);

    free(cabecalho);
    fclose(arq_bin);
    fclose(arq_ind);
    binarioNaTela(nome_arq_ind);
}

void procura_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 4);
}

void remove_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 5);
}

void insere_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {

    FILE* arq_bin;
    FILE* arq_idx;
    cabecalho_t* cabecalho = NULL;
    cabecalho_indice_t* cabecalho_indice = NULL;
    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    int num_ind;

    abre_arq_bin_ind(&arq_bin, nome_arq_bin, &arq_idx, nome_arq_idx, &cabecalho, &cabecalho_indice, &dados, tipoVar, &num_ind);

    desloca_offset(arq_bin, cabecalho->proxByteOffset);

    crime_t* crime_atual;
    for (int i = 0; i < num_consultas; i++) {
        crime_atual = le_crime_tela();
        if (crime_atual == NULL || insere_crime_binario(arq_bin, nome_campo, &dados, tipoVar, &num_ind, cabecalho->proxByteOffset, crime_atual) == 0) {
            libera_vars(2, (void*)cabecalho, (void*)cabecalho_indice);
            fclose(arq_bin);
            fclose(arq_idx);
            erro();
            return;
        } else {
            cabecalho->proxByteOffset += tamanho_crime(crime_atual);
            cabecalho->nroRegArq++;
            cabecalho_indice->nro_reg++;
        }
        libera_crime(crime_atual);
    }

    desloca_offset(arq_bin, 0);
    escreve_cabecalho(arq_bin, cabecalho);

    desloca_offset(arq_idx, 0);
    escreve_cabecalho_ind(arq_idx, cabecalho_indice);
    escreve_dados_gen(arq_idx, dados, tipoVar, cabecalho_indice->nro_reg);

    libera_vars(2, (void*)cabecalho, (void*)cabecalho_indice);
    fclose(arq_bin);
    fclose(arq_idx);

    binarioNaTela(nome_arq_bin);
    binarioNaTela(nome_arq_idx);
}

void atualiza_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 7);
}