#include "insercao.h"

int insere_dado_ordenado(void*** dados, int tipoVar, int* num_dados, void* dado) {
    void** dados_realloc = (void**) realloc(*dados, (++(*num_dados))*get_tam_var(tipoVar));
    if (dados_realloc == NULL) return 0;
    *dados = dados_realloc;

    for (int i = (*num_dados)-2; i >= 0; i--) {
        // eh para o dado estar nessa posicao?
        if (compara_dados_gen(dado, (*dados)[i], tipoVar) > 0) {
            (*dados)[i+1] = dado;
            return 1;
        }
        (*dados)[i+1] = (*dados)[i];
    }

    (*dados)[0] = dado;

    return 1;
}

int insere_crime_binario(FILE* arq_bin, cabecalho_t* cabecalho, cabecalho_indice_t* cabecalho_indice, char* nome_campo, void*** dados, int tipoVar, int* num_dados, crime_t* crime) {
    
    void* dado = pega_dado_generico(crime, nome_campo, cabecalho->proxByteOffset, tipoVar);
    if (dado == NULL) return 0;
    
    if (checa_dado_nulo(dado, tipoVar)) {
        escreve_registro_criminal(arq_bin, crime);
        cabecalho->nroRegArq++;
        cabecalho->proxByteOffset += tamanho_crime(crime);
        return 1;
    }

    if (insere_dado_ordenado(dados, tipoVar, num_dados, dado) == 0) {
        free(dado);
        return 0;
    }
    cabecalho_indice->nro_reg++;

    escreve_registro_criminal(arq_bin, crime);
    cabecalho->nroRegArq++;
    cabecalho->proxByteOffset += tamanho_crime(crime);
        
    return 1;
}