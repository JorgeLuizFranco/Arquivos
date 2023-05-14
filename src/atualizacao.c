#include "atualizacao.h"

int atualizar(FILE* arq_bin, cabecalho_t* cabecalho, crime_t* crime_atual, long long int byteOffset, FILE* arq_idx, cabecalho_indice_t* cabecalho_indice, 
               campo_busca_t** campos_atualizar, int num_campos_atualizar, int flag_campo_atualiza, campo_busca_t** campos, int num_campos, char* nome_campo,
               void** dados, int* num_indices, int tipoVar,
               int* flag_nova_pos) {

    crime_t* crime_atualizado = copia_crime(crime_atual);
    if (crime_atualizado == NULL || atualiza_registro(crime_atualizado, campos_atualizar, num_campos_atualizar) == 0) {
        libera_crime(crime_atualizado);
        libera_crime(crime_atual);
        libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, *num_indices);
        return 0;
    }

    void* dado_crime = pega_dado_generico(crime_atualizado, nome_campo, -1, tipoVar); // so serve pra pegar chave busca a partir do nome do campo

    if (dado_crime == NULL) {
        // TODO erro de alocacao

        free(dado_crime);
        return 0;
    }
    
    long long int byteOffset_final = byteOffset; // byteoffset em que o crime atualizado vai ficar no final das contas

    // vou dar update colocando diretamente lá as informações
    mostra_crime_tela(crime_atual);
    if (tamanho_crime(crime_atualizado) <= crime_atual->tamanho_real) {
        sobrescreve_crime(arq_bin, byteOffset, crime_atual->tamanho_real, crime_atualizado);
        desloca_offset(arq_bin, byteOffset);
        crime_atualizado = le_crime_bin(arq_bin);
        mostra_crime_tela(crime_atualizado);
    } else {
        byteOffset_final = cabecalho->proxByteOffset;
        insere_no_final(arq_bin, cabecalho, crime_atualizado);
        desloca_offset(arq_bin, byteOffset_final);
        crime_atualizado = le_crime_bin(arq_bin);
        mostra_crime_tela(crime_atualizado);
        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
    }

    if (flag_nova_pos != NULL) *flag_nova_pos = -1;

    if (flag_campo_atualiza != -1 || byteOffset_final != byteOffset) {
        // tenho que atualizar os dados (arq de indices)
        int indice_crime = indice_procura_registro(crime_atual, byteOffset, dados, *num_indices, nome_campo, tipoVar);
        int nova_pos;

        nova_pos = atualiza_dado(dados, tipoVar, num_indices, indice_crime, pega_chave_generico(dado_crime, tipoVar), byteOffset_final);
        if (flag_nova_pos != NULL) *flag_nova_pos = nova_pos;

        
    }

    libera_crime(crime_atualizado);
    free(dado_crime);
    return 1;
}
