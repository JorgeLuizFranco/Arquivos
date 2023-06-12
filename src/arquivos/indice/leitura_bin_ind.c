#include "./leitura_bin_ind.h"

/**
 * Lê o cabeçalho do arquivo binário de índicess contendo os metadados do arquivo.
 *
 * @param arq_indices Arquivo binário que será lido.
 * @param cabecalho_indice ponteiro que referencia a estrutura que armazenará o cabeçalho lido
 */
void le_cabecalho_indice(FILE* arq_indices, cabecalho_indice_t** cabecalho_indice) {
    char status;
    int nro_reg;
    fread(&status, sizeof(char), 1, arq_indices);
    fread(&nro_reg, sizeof(int), 1, arq_indices);
    *cabecalho_indice = cria_cabecalho_indice(status, nro_reg);
}

/**
 * Lê um dado inteiro do arquivo binário de índices
 *
 * @param arq_indices Arquivo binário que será lido.
 * @param dado_int ponteiro que referencia a estrutura que armazenará o dado lido
 */
void le_dado_int(FILE* arq_indices, dados_int_t** dado_int) {
    int chaveBusca;
    long long int byteOffset;
    fread(&chaveBusca, sizeof(int), 1, arq_indices);
    fread(&byteOffset, sizeof(long long int), 1, arq_indices);
    *dado_int = cria_dados_int(chaveBusca, byteOffset);
}

/**
 * Lê um dado string do arquivo binário de índices
 *
 * @param arq_indices Arquivo binário que será lido.
 * @param dado_str ponteiro que referencia a estrutura que armazenará o dado lido
 */
void le_dado_str(FILE* arq_indices, dados_str_t** dado_str) {
    char chaveBusca[12];
    long long int byteOffset;
    fread(chaveBusca, sizeof(char), 12, arq_indices);
    fread(&byteOffset, sizeof(long long int), 1, arq_indices);
    *dado_str = cria_dados_str(chaveBusca, 12, byteOffset);
}

/**
 * Lê um dado genérico do arquivo binário de índices
 *
 * @param arq_indices Arquivo binário que será lido.
 * @param dado_gen ponteiro que referencia a estrutura que armazenará o dado lido (antes do cast)
 * @param tipo_var tipo de índice genérico (de string(1) ou de int(0))
 */
void le_dado_gen(FILE* arq_indices, void** dado_gen, int tipoVar) {
    if (tipoVar == 0) {
        le_dado_int(arq_indices, (dados_int_t**)dado_gen);
    } else {
        le_dado_str(arq_indices, (dados_str_t**)dado_gen);
    }
}

/**
 * Lê um dado genérico do arquivo binário de índices
 *
 * @param arq_indices Arquivo binário que será lido.
 * @param dados lista de dados genericos
 * @param tipo_var tipo de índice genérico (de string(1) ou de int(0))
 * @param cabecalho_indice ponteiro que referencia a estrutura que armazenará o cabeçalho lido
 * @param num_ind referenciaa a int que armazenará número de índices
 */
int le_arq_indices(FILE* arq_indices, void*** dados, int tipoVar,
                   cabecalho_indice_t** cabecalho_indice, int* num_ind) {
    *num_ind = 0;
    if (arq_indices == NULL) return -1;

    le_cabecalho_indice(arq_indices, cabecalho_indice);

    if (*cabecalho_indice == NULL || (*cabecalho_indice)->status == '0') {
        if (*cabecalho_indice != NULL) {
            free(*cabecalho_indice);
            *cabecalho_indice = NULL;
            return -2;
        }
        return 0;
    }

    int num_dados = 0;
    for (int i = 0; i < (*cabecalho_indice)->nro_reg; i++) {
        (*num_ind)++;

        void* dado_atual = NULL;
        le_dado_gen(arq_indices, &dado_atual, tipoVar);
        if (dado_atual == NULL) {
            free(*cabecalho_indice);
            *cabecalho_indice = NULL;
            libera_vetor_ate_pos(*dados, num_dados - 1);
            *dados = NULL;
            return 0;
        }
        void** dados_realloc = (void**)realloc(*dados, (++num_dados) * get_tam_var(tipoVar));
        if (dados_realloc == NULL) {
            free(dado_atual);
            free(*cabecalho_indice);
            *cabecalho_indice = NULL;
            libera_vetor_ate_pos(*dados, num_dados - 2);
            *dados = NULL;
            return 0;
        }
        *dados = dados_realloc;
        (*dados)[num_dados - 1] = dado_atual;
    }

    return 1;
}
