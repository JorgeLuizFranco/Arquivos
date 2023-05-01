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
        return compara_string_limitada(crime_atual->dataCrime, campo_atual->chaveBuscaStr, 10, 0) == 0;
    } else if (strcmp(campo_atual->campo_busca, "marcaCelular") == 0) {
        return compara_string_limitada(crime_atual->marcaCelular, campo_atual->chaveBuscaStr, 12, 0) == 0;
    } else {
        char* string_dinamica_original;
        
        if (strcmp(campo_atual->campo_busca, "descricaoCrime") == 0) {
            string_dinamica_original = crime_atual->descricaoCrime;
        } else {
            string_dinamica_original = crime_atual->lugarCrime;
        }

        return strcmp(campo_atual->chaveBuscaStr, string_dinamica_original) == 0;
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

void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var) {
    // quero que *low aponte pro menor para e *high aponte para o maior cara que satisfaçam a busca
    // fprintf(stderr, "Tipo var eh %d\n", tipo_var);
    *low_reg = -1;
    *high_reg = -2;

    int low = 0, high = num_regs - 1;
    int mid;
    int result=-1;

    void* reg_mid;

    while (low <= high) {

        mid = (low + high) / 2;
        
        reg_mid = ind_int[mid];

        //fprintf(stderr, "Estou comparando %d com %d (ind %d)\n", ((dados_int_t*)reg_mid)->chaveBusca, *((int*)chaveBusca), mid);

        if (compara_chave_busca(reg_mid, chaveBusca, 1, tipo_var)) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    if (result == -1 || compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var) == 0)
        return;
    *low_reg = result;

    low = 0, high = num_regs - 1;

    //fprintf(stderr, "==%d====%d====%d==\n", result, ((dados_int_t*)reg_mid)->chaveBusca, *((int*)chaveBusca));
    result = -1;
    while (low <= high) {
        mid = (low + high) / 2;

        reg_mid = ind_int[mid];
        //fprintf(stderr, "Estou comparando %d com %d (ind %d)\n", ((dados_int_t*)reg_mid)->chaveBusca, *((int*)chaveBusca), mid);

        if (compara_chave_busca(reg_mid, chaveBusca, -1, tipo_var) == 0) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }
    if (result-- <= 0) {
        result = -2;
    } else if (!compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var)) {
        result = -2;
    }
    *high_reg = result;
}