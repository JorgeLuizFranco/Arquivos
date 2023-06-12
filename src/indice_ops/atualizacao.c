#include "./atualizacao.h"

// basicamente esse arquivo so foi criado para essas duas funções não ficarem jogadas

/**
 * @param vetor_dados vetor de registros de indice genéricos
 * @param tipoVar 0 se int, 1 se string
 * @param tam_vetor ponteiro para o tamanho desse vetor
 * @param pos_dado posição a ser atualizada
 * @param nova_chave_busca novo valor para chave de busca
 * @param novo_byte_offset novo valor para o byteoffset
 *
 * @return numero >= 0 indicando a nova posição após a atualização OU
 *         -1 se a posição que se quer atualizar é -1 OU
 *         -2 se atualizei para um registro com chave de busca NULO (nesse caso, automaticamente ja
 * remove do vetor)
 */
int atualiza_dado(void** vetor_dados, int tipoVar, int* tam_vetor, int pos_dado,
                  void* nova_chave_busca, long long int novo_byte_offset) {

    if (pos_dado == -1) return -1;

    if (tipoVar == 0) {
        ((dados_int_t*)(vetor_dados[pos_dado]))->chaveBusca = *((int*)nova_chave_busca);
        ((dados_int_t*)(vetor_dados[pos_dado]))->byteOffset = novo_byte_offset;
    } else {
        copia_array_char(((dados_str_t*)(vetor_dados[pos_dado]))->chaveBusca,
                         (char*)nova_chave_busca, 12);
        ((dados_str_t*)(vetor_dados[pos_dado]))->byteOffset = novo_byte_offset;
    }

    if (checa_dado_nulo(vetor_dados[pos_dado], tipoVar)) {
        remove_dado(&vetor_dados, tipoVar, tam_vetor, pos_dado);
        return -2;
    }

    int pos_final = pos_dado;

    for (int i = pos_dado + 1; i < *tam_vetor; i++)
        if (compara_dados_gen(vetor_dados[i], vetor_dados[i - 1], tipoVar) < 0) {
            troca(&vetor_dados[i], &vetor_dados[i - 1]);
            pos_final = i;
        } else {
            break;
        }

    for (int i = pos_dado - 1; i >= 0; i--)
        if (compara_dados_gen(vetor_dados[i], vetor_dados[i + 1], tipoVar) > 0) {
            troca(&vetor_dados[i], &vetor_dados[i + 1]);
            pos_final = i;
        } else {
            break;
        }

    return pos_final;
}

/**
 * Atualiza um registro criminal e faz as alterações necessárias no arquivo binário de
 * registros e no de índices
 *
 * @param arq_bin arquivo binário de registros
 * @param cabecalho cabecalho desse arquivo
 * @param crime_atual registro a ser atualizado
 * @param byteOffset informa o byteOffset do registro crime_atual
 * @param arq_idx arquivo de registros de índices
 * @param cabecalho_indice cabecalho desse arquivo
 * @param campos_atualizar vetor de struct que guarda um campo da struct e o valor para o qual será
 * atualizado crime_atual
 * @param num_campos_atualizar tamanho desse vetor
 * @param flag_campo_atualiza indica se o arquivo de índices foi feito com base em um campo que terá
 * seu valor mudado nessa chamada de função
 * @param campos campos (vetor) da query. Irrelevante para a função em si. Só é passado como
 * parâmetro porque tenho que dar free em caso de erro
 * @param num_campos tamanho desse vetor
 * @param nome_campo nome do campo com base no qual foi feito o índice
 * @param dados vetor de registros de índice
 * @param num_indices ponteiro para o tamanho desse vetor
 * @param tipoVar 0 se o tipo desse campo de nome "nome_campo" for int e 1 se string
 * @param flag_nova_pos se não for NULL, será setada pela função com a nova posição do registro de
 * índices no vetor de índices (dados). É basicamente um "segundo retorno"
 * @return 0 se ocorreu algum erro, 1 se deu tudo certo
 */
int atualizar(FILE* arq_bin, cabecalho_t* cabecalho, crime_t* crime_atual, long long int byteOffset,
              FILE* arq_idx, cabecalho_indice_t* cabecalho_indice, campo_busca_t** campos_atualizar,
              int num_campos_atualizar, int flag_campo_atualiza, campo_busca_t** campos,
              int num_campos, char* nome_campo, void** dados, int* num_indices, int tipoVar,
              int* flag_nova_pos) {

    // primeiramente copia o registro
    crime_t* crime_atualizado = copia_crime(crime_atual);

    // se ocorrer problema de alocação ao fazer essa cópia ou então a cópia dê certo
    // mas ocorra algum erro de alocação ao atualizar o registro, libera todas as variáveis
    // e retorna que ocorreu erro
    if (crime_atualizado == NULL ||
        atualiza_registro(crime_atualizado, campos_atualizar, num_campos_atualizar) == 0) {
        if (crime_atualizado != NULL) libera_crime(crime_atualizado);
        libera_crime(crime_atual);
        libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, num_campos,
                              dados, *num_indices);
        return 0;
    }

    // cria registros de indice para pegar a chave de busca
    void* dado_crime = pega_dado_generico(crime_atualizado, nome_campo, -1, tipoVar);
    void* dado_crime_antes = pega_dado_generico(crime_atual, nome_campo, -1, tipoVar);

    // se nao der certo alocar memoria para eles, libera memoria e aborta
    if (dado_crime == NULL || dado_crime_antes == NULL) {
        if (dado_crime != NULL) free(dado_crime);
        if (dado_crime_antes != NULL) free(dado_crime_antes);
        libera_crime(crime_atual);
        libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, num_campos,
                              dados, *num_indices);
        return 0;
    }

    long long int byteOffset_final =
        byteOffset; // byteoffset em que o crime atualizado vai ficar no final das contas

    // se o tamanho do crime atualizado for menor ou igual ao tamanho que o crime_atual ocupa no
    // arquivo então é possível fazer a sobrescrição
    if (tamanho_crime(crime_atualizado) <= crime_atual->tamanho_real) {
        sobrescreve_crime(arq_bin, byteOffset, crime_atual->tamanho_real, crime_atualizado);
    } else {
        // se não for, insiro no final o crime atualizado e removo logicamente
        // o crime antes de ser atualizado
        byteOffset_final = cabecalho->proxByteOffset;
        insere_no_final(arq_bin, cabecalho, crime_atualizado);
        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
    }

    // seto o offset de dado_crime para o offset em que o crime atualizado foi inserido
    // faço isso porque agora vou atualizar o arquivo de índices
    seta_offset_gen(dado_crime, tipoVar, byteOffset_final);

    // se quero saber qual será a nova posição, digo que enquanto não sei ela é -1
    if (flag_nova_pos != NULL) *flag_nova_pos = -1;

    // se o arquivo de índices tiver sido feito com base em um campo que foi atualizado
    // ou então se o byte offset do registro a ser atualizado mudar

    if (flag_campo_atualiza != -1 || byteOffset_final != byteOffset) {
        // nesse caso terei que atualizar os dados (arq de indices)

        // Se o dado era nulo e agora nao e mais, vou acrescentar nos indices de maneira ordenada
        if (checa_dado_nulo(dado_crime_antes, tipoVar) &&
            checa_dado_nulo(dado_crime, tipoVar) == 0) {
            insere_dado_ordenado(&dados, tipoVar, num_indices, dado_crime);
            libera_crime(crime_atualizado);
            free(dado_crime_antes);
            // lembrando que se eu cheguei nessa situação, é porque nem tava no arquivo de índices
            // o que significa que nem preciso me importar com a flag_nova_pos
            return 1;
        } else {
            // Se o dado era nulo e continua nulo, o codigo a seguir nao faz nada
            // Se nao era nulo e passa a ser nulo, o codigo a seguir toma conta desse caso
            // fazendo uma remoção (isso ocorre na função "atualiza_dado")
            // Se nao era nulo e continua nao nulo, atualizo normalmente
            int indice_crime = indice_procura_registro(crime_atual, byteOffset, dados, *num_indices,
                                                       nome_campo, tipoVar);
            int nova_pos;

            nova_pos = atualiza_dado(dados, tipoVar, num_indices, indice_crime,
                                     pega_chave_generico(dado_crime, tipoVar), byteOffset_final);
            if (flag_nova_pos != NULL)
                *flag_nova_pos =
                    nova_pos; // seta a nova posição do registro de índice no vetor de índices
        }
    }

    libera_crime(crime_atualizado);
    free(dado_crime);
    return 1;
}
