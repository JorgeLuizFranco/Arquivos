#include "busca.h"

void busca_bin_campos(void** ind_int, int num_regs, int* low_reg, int* high_reg, void* chaveBusca, int tipo_var, int flag_dinamica) {
    // quero que *low aponte pro menor para e *high aponte para o maior cara que satisfaçam a busca
    *low_reg = -1;
    *high_reg = -2;

    int low = 0, high = num_regs - 1;
    int mid;
    int result=-1;

    void* reg_mid;

    while (low <= high) {

        mid = (low + high) / 2;
        
        reg_mid = ind_int[mid];

        if (compara_chave_busca(reg_mid, chaveBusca, 1, tipo_var, flag_dinamica)) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    if (result == -1 || compara_chave_busca(ind_int[result], chaveBusca, 0, tipo_var, flag_dinamica) == 0)
        return;
    *low_reg = result;

    low = 0, high = num_regs - 1;

    result = -1;
    while (low <= high) {
        mid = (low + high) / 2;

        reg_mid = ind_int[mid];
        //reg_mid > chaveBusca?
        if (compara_chave_busca(reg_mid, chaveBusca, -1, tipo_var, flag_dinamica) == 0) {
            high = mid - 1;
            result = mid;
        } else {
            low = mid + 1;
        }
    }

    // se result = -1, n tem ngm maior do que chaveBusca
    // sei que o low é o menor cara = chaveBusca
    // entao se result = -1, qr dizer que do low ate num_regs-1 e todo mundo igual

    *high_reg = result != -1 ? result-1 : num_regs-1;
}

int indice_procura_registro(crime_t* crime_atual, long long int offset_atual, void** dados, int num_dados, char* nome_campo, int tipoVar) {
    if (dados == NULL) return -1;
    
    int low_reg = 0, high_reg = num_dados-1;
    void* dado_atual;
    void* dado_crime = pega_dado_generico(crime_atual, nome_campo, -1, tipoVar); // so serve pra pegar chave busca a partir do nome do campo
    if (dado_crime == NULL) return -2;

    if (checa_dado_nulo(dado_crime, tipoVar) == 0) 
        busca_bin_campos(dados, num_dados, &low_reg, &high_reg, pega_chave_generico(dado_crime, tipoVar), tipoVar, 0);
    
    free(dado_crime);

    if (low_reg < 0) return -1;

    int mid_reg;
    long long int offset_dado_atual;

    while (low_reg <= high_reg) {

        mid_reg = (low_reg + high_reg) / 2;
        dado_atual = dados[mid_reg];
        offset_dado_atual = pega_offset_generico(dado_atual, tipoVar);

        if (offset_atual > offset_dado_atual) {
            low_reg = mid_reg + 1;
        } else if (offset_atual < offset_dado_atual) {
            high_reg = mid_reg - 1;
        } else {
            return mid_reg;
        }
    }

    return -1;
}

