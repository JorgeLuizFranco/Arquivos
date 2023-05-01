#include "busca.h"

campo_busca_t* le_campo_busca() {
    campo_busca_t* campo_atual = (campo_busca_t*) malloc(sizeof(campo_busca_t));
    if (campo_atual == NULL) return NULL;

    scanf("%s", campo_atual->campo_busca);

    if (strcmp("idCrime", campo_atual->campo_busca) == 0 || strcmp("numeroArtigo", campo_atual->campo_busca) == 0) {
        scanf("%d", &(campo_atual->chaveBuscaInt));
    } else {
        scan_quote_string(campo_atual->chaveBuscaStr);
    }
    return campo_atual;
}

campo_busca_t** le_campos_busca(int num_campos) {

    campo_busca_t** campos_buscados = (campo_busca_t**) malloc(num_campos * sizeof(campo_busca_t*));
    if (campos_buscados == NULL) return NULL;

    for (int i = 0; i < num_campos; i++) {
        campos_buscados[i] = le_campo_busca();
        if (campos_buscados[i] == NULL) {
            libera_vetor_ate_pos((void**)campos_buscados, i-1);
            return NULL;
        }
    }

    return campos_buscados;
}

int satisfaz_campo(crime_t* crime_atual, campo_busca_t* campo_atual) {
    if (strcmp(campo_atual->campo_busca, "idCrime") == 0) {
        return crime_atual->idCrime == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "numeroArtigo") == 0) {
        return crime_atual->numeroArtigo == campo_atual->chaveBuscaInt;
    } else if (strcmp(campo_atual->campo_busca, "dataCrime") == 0) {
        return strcmp(crime_atual->dataCrime, campo_atual->chaveBuscaStr) == 0;
    } else if (strcmp(campo_atual->campo_busca, "marcaCelular") == 0) {
        return strcmp(crime_atual->marcaCelular, campo_atual->chaveBuscaStr) == 0;
    } else {
        char string_a_se_comparar[12];
        char* string_dinamica_original;

        if (strcmp(campo_atual->campo_busca, "descricaoCrime") == 0) {
            string_dinamica_original = crime_atual->descricaoCrime;
        } else {
            string_dinamica_original = crime_atual->lugarCrime;
        }

        int i = 0;
        for (; i < 12 && string_dinamica_original[i] != '\0'; i++) {
            string_a_se_comparar[i] = string_dinamica_original[i];
        }
        for (; i < 12; i++) {
            string_a_se_comparar[i] = '$';
        }

        return strcmp(string_a_se_comparar, campo_atual->chaveBuscaStr) == 0;
    }
    return -1;
}

int satisfaz_query(crime_t* crime_atual, campo_busca_t** query_atual, int n_campos) {
    int resposta = 1; // satisfaz. Se eu encontrar evidencias do contrario, seto pra zero

    campo_busca_t* campo_atual;

    for (int i = 0; i < n_campos; i++) {
        campo_atual = query_atual[i];
        if (satisfaz_campo(crime_atual, campo_atual) == 0) {
            resposta = 0;
            break;
        }
    }

    return resposta;
}

void busca_bin_campos(void** ind_int, int num_regs, void** low_reg, void** high_reg, void* chaveBusca) {
    // quero que *low aponte pro menor para e *high aponte para o maior cara que satisfaçam a busca
    *low_reg = NULL;
    *high_reg = NULL;

    int low = 0, high = num_regs - 1;
    int mid;

    void* reg_mid;

    while (low <= high) {

        mid = (low + high) / 2;
        
        reg_mid = ind_int[mid];

        if (compara_chave_busca(reg_mid, chaveBusca, 1)) {
            if (mid > 0 && compara_chave_busca(ind_int[mid-1], chaveBusca, 1)) {
                high = mid - 1;
            } else {
                break;
            }
        } else {
            low = mid + 1;
        }
    }

    if (compara_chave_busca(reg_mid, chaveBusca, 0)) return;
    *low_reg = reg_mid;

    low = 0, high = num_regs - 1;

    while (low <= high) {
        mid = (low + high) / 2;

        reg_mid = ind_int[mid];

        if (compara_chave_busca(reg_mid, chaveBusca, -1)) {
            if (mid + 1 < num_regs && compara_chave_busca(ind_int[mid+1], chaveBusca, -1)) {
                low = mid + 1;
            } else {
                break;
            }
        } else {
            high = mid - 1;
        }
    }

    *high_reg = reg_mid;
}