#include "consultas.h"


void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade) {

    // se esse arquivo de indice existir, armazeno ele na memoria
    // se não, digo que ele e NULL



    FILE* arq_bin = fopen(nome_arq_bin, "rb+");
    if (arq_bin == NULL) {
        libera_memo_consultas(1, NULL, NULL, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }
    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL) {
        libera_memo_consultas(1, arq_bin, NULL, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }

    FILE* arq_idx = fopen(nome_arq_idx, "rb+");

    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    cabecalho_indice_t* cabecalho_indice = NULL;
    crime_t* crime_atual = NULL;
    int num_indices;

    le_arq_indices(arq_idx, &dados, tipoVar, &cabecalho_indice, &num_indices);
    if (cabecalho_indice == NULL) {
        libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, NULL, -1);
        return;
    }

    for (int i = 0; i < num_consultas; i++) {

        int regs_mostrados = 0;
        if (funcionalidade == 4)
            printf("Resposta para a busca %d\n", i+1);

        int num_campos;
        if (i != 0) desloca_offset(arq_bin, TAMANHO_CABECALHO);
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);
            return;
        }

        int num_campos_atualizar;
        campo_busca_t** campos_atualizar;

        if (funcionalidade == 7) {
            scanf("%d", &num_campos_atualizar);
            campos_atualizar = le_campos_busca(num_campos_atualizar);
            if (campos_atualizar == NULL) {
                libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, -1, dados, num_indices);
                return;
            }
        } 

        // sera que tem o campo do arquivo de indice?
        int flag_campo_procurado = checa_campo_procurado(campos, num_campos, nome_campo);
        int flag_campo_atualiza = checa_campo_procurado(campos_atualizar, num_campos_atualizar, nome_campo);
        // se o campo do indice nao tiver entre os campos especificados pela query, fazer uma busca linear no arquivo binario
        
        if (flag_campo_procurado == -1) {
            long long int byteOffset = TAMANHO_CABECALHO;
            long long int byteOffsetFinal = cabecalho->proxByteOffset;
            while (byteOffset < byteOffsetFinal) {
                crime_atual = le_crime_bin(arq_bin);
                if (crime_atual == NULL) {
                    libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        // remover esse crime do arquivo binario marcando a flag removido como 1
                        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
                        // procurar no arquivo de índices o registro atual e remove-lo por shiftacao
                        int ind_arquivo = indice_procura_registro(crime_atual, byteOffset, dados, num_indices, nome_campo, tipoVar);
                        remove_com_shift(&dados, tipoVar, &num_indices, ind_arquivo, cabecalho_indice);
                    } else if (funcionalidade == 7) {
                        atualizar(arq_bin, cabecalho, crime_atual, byteOffset, arq_idx, cabecalho_indice, 
                                  campos_atualizar, num_campos_atualizar, flag_campo_atualiza, 
                                  campos, num_campos, nome_campo, dados, &num_indices, tipoVar, NULL);
                    }
                }

                byteOffset += crime_atual->tamanho_real;
                libera_crime(crime_atual);
            }
        } else {

            // se tiver (e existir), ver todos os caras com o indice que quero no arquivo de indices com busca binaria
            // percorrer esses caras, e pra cada um deles eu vou pra cada byteoffset do arquivo que to lendo e vejo se obedece
            // os outros requisitos

            int low;
            int high;
            long long int byteOffset;
            void* chaveBusca = tipoVar == 0 ? (void*)&(campos[flag_campo_procurado]->chaveBuscaInt) : (void*)(campos[flag_campo_procurado]->chaveBuscaStr);
            busca_bin_campos(dados, num_indices, &low, &high, chaveBusca, tipoVar, 1);
            while (low <= high) {

                void* dado_atual = dados[low];
                byteOffset = pega_offset_generico(dado_atual, tipoVar);
                crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
            
                if (crime_atual == NULL) {
                    libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
                        remove_com_shift(&dados, tipoVar, &num_indices, low, cabecalho_indice);
                        low--;
                        high--;
                    } else if (funcionalidade == 7) {
                        int flag_nova_pos;
                        atualizar(arq_bin, cabecalho, crime_atual, byteOffset, arq_idx, cabecalho_indice, 
                                  campos_atualizar, num_campos_atualizar, flag_campo_atualiza, 
                                  campos, num_campos, nome_campo, dados, &num_indices, tipoVar, &flag_nova_pos);
                        
                        if (flag_nova_pos <= low) {
                            // se igual: faco nada
                            // se menor:
                            // low agora aponta para um cara anteior; mas sera incrementado no fim do loop, levando ao proximo cara
                            // entao nao faco nada
                            // se igual a -1, ignora
                            if (flag_nova_pos == -2) {
                                low--;
                                high--; // ocorreu remocao
                            }
                        } else {
                            // se chegou aqui, flag > low
                            // isso quer dizer que aumentou a posicao
                            // entao agora low aponta precocemente para o proximo cara, entao conserto isso
                            low--;
                            if (flag_nova_pos <= high) {
                                // mas se aumentou e continua <= high quer dizer que a chave nao mudou
                                // ou seja, ele foi inserido no final do arquivo binario e esta com o maior byte offset de todos
                                // nesse caso, ele esta na posicao "high"
                                // entao eu nem preciso mais verificar essa posicao
                                high--;
                            } else {
                                // se aumentou e eh >high, o cara que eh high deu uma movida pra esquerda
                                high--;
                            }
                        }
                    }
                }

                libera_crime(crime_atual);
                low++;
            }
        }

        if (regs_mostrados == 0 && funcionalidade == 4) {
            printf("Registro inexistente.\n");
        }

        libera_vetor_ate_pos((void**)campos, num_campos-1);
        if (funcionalidade == 7) libera_vetor_ate_pos((void**)campos_atualizar, num_campos_atualizar-1);
    }

    if (funcionalidade == 5 || funcionalidade == 7) {
        desloca_offset(arq_bin, 0);
        escreve_cabecalho(arq_bin, cabecalho);
        if (arq_idx != NULL) {
            // se eu so der offset pro 0 e escrever o novo vetor de registros de dados,
            // vai sobrar no final do arquivo outros registros. So que quero limpar eles
            // um jeito de fazer isso e fechando o arquivo e abrindo no modo de escrita
            // para limpa-lo
            fclose(arq_idx);
            arq_idx = fopen(nome_arq_idx, "wb");
            escreve_cabecalho_ind(arq_idx, cabecalho_indice);
            escreve_dados_gen(arq_idx, dados, tipoVar, num_indices);
        }
    }

   libera_memo_consultas(0, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);

    if (funcionalidade == 5 || funcionalidade == 7) {
        binarioNaTela(nome_arq_bin);
        binarioNaTela(nome_arq_idx);
    }
}
