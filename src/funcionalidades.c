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

    FILE* arq_bin = fopen(nome_arq_bin, "r+b"); // arquivo de registros. vou ler, mas preciso escrever como inconsistente
    if (arq_bin == NULL) {
        erro();
        return;
    }

    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        if (cabecalho != NULL) free(cabecalho);
        fclose(arq_bin);
        erro();
        return;
    }

    seta_consistencia_bin(arq_bin, cabecalho, '0');

    // número de registro nao removidos
    int n_registros = cabecalho->nroRegArq - cabecalho->nroRegRem; 

    FILE* arq_ind = fopen(nome_arq_ind, "wb"); // arquivo de índices. só vou escrever
    if (arq_ind == NULL) {
        fclose(arq_bin);
        free(cabecalho);
        erro();
        return;
    }

    // depois de abrir arquivos, chama função implementada em escrita_bin_ind.c
    escreve_arq_ind(arq_bin, arq_ind, nome_campo, tipo_campo, n_registros, cabecalho->proxByteOffset);
    seta_consistencia_bin(arq_bin, cabecalho, '1');
    
    // desalocações de memória, fechamento dos arquivos
    free(cabecalho);
    fclose(arq_bin);
    fclose(arq_ind);

    binarioNaTela(nome_arq_ind); // função pedida para ser executada
}   

/**
 * FUNCIONALIDADE 4
 * Procura e mostra registros que satisfazem consultas
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
 * @param num_consultas número de consultas a se fazer
*/
void procura_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    // Especifico que quero aplicar a funcionalidade 4
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 4);
}

/**
 * FUNCIONALIDADE 5
 * Remove registros que satisfazem consultas
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
 * @param num_consultas número de consultas a se fazer
*/
void remove_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    // Especifico que quero aplicar a funcionalidade 5
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 5);
}

/**
 * FUNCIONALIDADE 6
 * Insere registros
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
 * @param num_consultas número de registros que o usuário vai escrever e colocar no arquivo de índices e no binário
*/
void insere_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {

    // declara variáveis a serem usadas
    FILE* arq_bin;
    FILE* arq_idx;
    cabecalho_t* cabecalho = NULL;
    cabecalho_indice_t* cabecalho_indice = NULL;
    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    int num_ind;

    // função para abrir o arquivo binário e o arquivo de índices que detecta se ocorreu erro
    if (abre_arq_bin_ind(&arq_bin, nome_arq_bin, &arq_idx, nome_arq_idx, &cabecalho, &cabecalho_indice, &dados, tipoVar, &num_ind) == 0)
        return;

    // enquanto uso os arquivos, deixo status como 0
    // faço isso pro arquivo binário
    seta_consistencia_bin(arq_bin, cabecalho, '0');
    // faço isso pro arquivo de índices
    seta_consistencia_ind(arq_idx, cabecalho_indice, '0');


    // dou fseek para o final porque insercao eh no final do arquivo
    desloca_offset(arq_bin, cabecalho->proxByteOffset);

    crime_t* crime_atual;
    for (int i = 0; i < num_consultas; i++) {
        crime_atual = le_crime_tela(); // leio da tela a consulta atual
        // se ocorrer algum erro de alocação em algum ponto no código (inclusive ao tentar inserir), aborto
        if (crime_atual == NULL || insere_crime_binario(arq_bin, cabecalho, cabecalho_indice, nome_campo, &dados, tipoVar, &num_ind, crime_atual) == 0) {
            free(cabecalho);
            free(cabecalho_indice);
            fclose(arq_bin);
            fclose(arq_idx);
            erro();
            return;
        }
        libera_crime(crime_atual);
    }

    // finaliza a escrita do arquivo binário
    seta_consistencia_bin(arq_bin, cabecalho, '1');
    free(cabecalho);
    fclose(arq_bin);

    // finaliza a escrita do arquivo de índices
    desloca_offset(arq_idx, 0);
    seta_consistencia_ind(arq_idx, cabecalho_indice, '1');
    escreve_dados_gen(arq_idx, dados, tipoVar, cabecalho_indice->nro_reg);
    fclose(arq_idx);

    // mostra o que e pedido na tela
    binarioNaTela(nome_arq_bin);
    binarioNaTela(nome_arq_idx);
}

/**
 * FUNCIONALIDADE 7
 * Atualiza registros
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_ind nome do arquivo binário de índices de destino
 * @param num_consultas número de atualizações que o usuário fará
*/
void atualiza_registros(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas) {
    // Especifico que quero aplicar a funcionalidade 7
    realiza_consultas(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas, 7);
}
