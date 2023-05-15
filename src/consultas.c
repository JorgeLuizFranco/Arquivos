#include "consultas.h"

// esse arquivo serve só pra separar essa função pra deixar o resto do código mais limpo

/**
 * Aproveita um mesmo código para as funcionalidades 4, 5 e 7
 * 
 * @param nome_arq_bin nome do arquivo binário de registros criminais
 * @param nome_campo nome do campo do registro 'crime'
 * @param tipo_campo 'string' ou 'int' é o tipo do campo especificado
 * @param nome_arq_idx nome do arquivo binário de índices de destino
 * @param num_consultas número de vezes que a funcionalidade será executada
 * @param funcionalidade (=4, 5 ou 7); identifica a funcionalidade que se quer executar
 * 
 * 4 --> mostrar crimes
 * 5 --> excluir crimes
 * 7 --> atualizar crimes
*/
void realiza_consultas(char* nome_arq_bin, char* nome_campo, char* tipo_campo, char* nome_arq_idx, int num_consultas, int funcionalidade) {

    // abre os arquivos binário e de cabeçalho
    FILE* arq_bin = fopen(nome_arq_bin, "rb+");
    if (arq_bin == NULL) {
        libera_memo_consultas(1, NULL, NULL, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }
    cabecalho_t* cabecalho = le_cabecalho_bin(arq_bin);
    if (cabecalho == NULL || cabecalho->status == '0') {
        libera_memo_consultas(1, arq_bin, cabecalho, NULL, NULL, NULL, -1, NULL, -1);
        return;
    }

    seta_consistencia_bin(arq_bin, cabecalho, '0');

    // abre arquivo de índices e declara variáveis relacionadas
    FILE* arq_idx = fopen(nome_arq_idx, "rb+");

    void** dados = NULL;
    int tipoVar = get_tipo_var(tipo_campo);
    cabecalho_indice_t* cabecalho_indice = NULL;
    crime_t* crime_atual = NULL;
    int num_indices;

    le_arq_indices(arq_idx, &dados, tipoVar, &cabecalho_indice, &num_indices);
    if (arq_idx != NULL) seta_consistencia_ind(arq_idx, cabecalho_indice, '0');
    
    // se índice não estiver definido, libero memória e aborto
    // a não ser que seja a funcionalidade 4. Nesse caso, finge que apenas não existe arquivo de índices
    // e faz apenas buscas sequenciais
    if (cabecalho_indice == NULL && funcionalidade != 4) {
        libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, NULL, -1);
        return;
    }

    // para cada execução da funcionalidade...
    for (int i = 0; i < num_consultas; i++) {

        int regs_mostrados = 0; // serve para a funcionalidade 4. indica o numero de registros que foram mostrados
        if (funcionalidade == 4)
            printf("Resposta para a busca %d\n", i+1);

        int num_campos;
        // volta para o offset do cabeçalho para executar a funcionalidade novamente
        if (i != 0) desloca_offset(arq_bin, TAMANHO_CABECALHO);
        
        // pegar da entrada os campos de busca 
        scanf("%d", &num_campos);
        campo_busca_t** campos = le_campos_busca(num_campos);
        if (campos == NULL) {
            libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);
            return;
        }

        // se for a funcionalidade 7, também tem que pegar os campos de atualização
        int num_campos_atualizar;
        campo_busca_t** campos_atualizar;

        if (funcionalidade == 7) {
            scanf("%d", &num_campos_atualizar);
            campos_atualizar = le_campos_busca(num_campos_atualizar);
            // abortar em caso de erros de alocação
            if (campos_atualizar == NULL) {
                libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, -1, dados, num_indices);
                return;
            }
        } 

        // sera que tem o campo do arquivo de indice? essas flags indicam
        // se o campo do arquivo de índice está presente em cada um desses casos
        int flag_campo_procurado = checa_campo_procurado(campos, num_campos, nome_campo);
        int flag_campo_atualiza;
        if (funcionalidade == 7)
            flag_campo_atualiza = checa_campo_procurado(campos_atualizar, num_campos_atualizar, nome_campo);
        
        // se o campo do indice nao tiver entre os campos especificados pela query, fazer uma busca linear no arquivo binario
        if (flag_campo_procurado == -1) {
            long long int byteOffset = TAMANHO_CABECALHO;
            long long int byteOffsetFinal = cabecalho->proxByteOffset;
            while (byteOffset < byteOffsetFinal) {
                // vai lendo crime por crime
                crime_atual = le_crime_bin(arq_bin);
                if (crime_atual == NULL) {
                    libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                // se não for logicamente removido e satisfizer a query
                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    // cada funcionalidade faz uma coisa
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
                        if (atualizar(arq_bin, cabecalho, crime_atual, byteOffset, arq_idx, cabecalho_indice, 
                                  campos_atualizar, num_campos_atualizar, flag_campo_atualiza, 
                                  campos, num_campos, nome_campo, dados, &num_indices, tipoVar, NULL) == 0) {
                                    // abortar em caso de erro
                                    libera_crime(crime_atual);
                                    libera_vetor_ate_pos((void**)campos_atualizar, num_campos_atualizar-1);
                                    libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                                    return;
                                  }
                    }
                }

                byteOffset += crime_atual->tamanho_real;
                libera_crime(crime_atual);
            }
        } else {

            // se tiver, ver todos os caras com o indice que quero no arquivo de indices com busca binaria
            // percorrer esses caras, e pra cada um deles eu vou pra cada byteoffset do arquivo que to lendo e vejo se obedece
            // os outros requisitos

            int baixo;
            int cima;
            long long int byteOffset;
            // pega uma chave de busca genérica de acordo com o tipo (string ou int) do campo
            void* chaveBusca = tipoVar == 0 ? (void*)&(campos[flag_campo_procurado]->chaveBuscaInt) : (void*)(campos[flag_campo_procurado]->chaveBuscaStr);
            // faz uma busca binária no vetor de índices para achar "baixo" e "cima" tais que índices entre baixo e cima tem a chave buscada
            busca_bin_campos(dados, num_indices, &baixo, &cima, chaveBusca, tipoVar, 1);

            // vai iterando por esses caras que têm a chave buscada e faz uma query em cada um deles pra ver se realmente
            // obedecem aos requisitos
            // para isso, vou incrementando "baixo"
            while (baixo <= cima) {

                void* dado_atual = dados[baixo];
                byteOffset = pega_offset_generico(dado_atual, tipoVar);
                crime_atual = le_crime_bin_offset(arq_bin, byteOffset);
            
                if (crime_atual == NULL) {
                    if (funcionalidade == 7) libera_vetor_ate_pos((void**)campos_atualizar, num_campos_atualizar-1);
                    libera_memo_consultas(1, arq_bin, cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                    return;
                }

                // se o crime atual não tiver sido removido e ele satisfizer a query, ver a funcionalide que quero
                // e executá-la
                if (crime_atual->removido != '1' && satisfaz_query(crime_atual, campos, num_campos)) {
                    if (funcionalidade == 4) {
                        mostra_crime_tela(crime_atual);
                        regs_mostrados++;
                    } else if (funcionalidade == 5) {
                        remocao_logica(arq_bin, crime_atual, cabecalho, byteOffset);
                        remove_com_shift(&dados, tipoVar, &num_indices, baixo, cabecalho_indice);
                        // se o índice da posição "baixo" foi removido, a shiftação fez com que "baixo" agora 
                        // aponte para o próximo registro. Mas só quero fazer isso na próxima iteração do loop
                        // então desconto isso
                        baixo--;
                        // com a shiftação, preciso decrementar
                        cima--;
                    } else if (funcionalidade == 7) {
                        int flag_nova_pos;
                        if (atualizar(arq_bin, cabecalho, crime_atual, byteOffset, arq_idx, cabecalho_indice, 
                                  campos_atualizar, num_campos_atualizar, flag_campo_atualiza, 
                                  campos, num_campos, nome_campo, dados, &num_indices, tipoVar, &flag_nova_pos) ==0) {
                                    libera_crime(crime_atual);
                                    libera_vetor_ate_pos((void**)campos_atualizar, num_campos_atualizar-1);
                                    libera_memo_consultas(1, arq_bin,  cabecalho, arq_idx, cabecalho_indice, campos, num_campos, dados, num_indices);
                                    return;
                                  }
                        
                        if (flag_nova_pos <= baixo) {
                            // se igual a baixo: faco nada, porque nao ocorreu nenhuma shiftação
                            
                            // se menor que baixo: não faço nada. Porque agora que ele ajustou sua posição no vetor
                            // para um índice inferior, o cara que tava em baixo-1 agora está em baixo. Mas isso não importa, pois
                            // basta esperar a próxima iteração para baixo incrementar e eu ir pro próximo
                            
                            // se igual a -1: existem apenas as seguintes possibilidades do que pode ter ocorrido
                            // - foi feita uma inserção (mas é impossível isso ocorrer, pois inserções só ocorrem quando o campo vai 
                            // de NULL para algo não nulo. Mas era impossível ele ser inicialmente NULL, pois o registro atual foi obtido
                            // com auxílio do arquivo de índices)
                            // - o campo referente ao índice não sofreu atualização e o seu byteoffset não mudou (ignoro)

                            // se igual a -2, quer dizer que o campo passou a ser igual a NULL
                            // nesse caso preciso removê-lo do arquivo de índices
                            if (flag_nova_pos == -2) {
                                // pela mesma lógica da funcionalide 5 faço esses decrementos
                                baixo--;
                                cima--;
                            }
                        } else {
                            // se chegou aqui, flag > baixo
                            // isso quer dizer que aumentou a posicao
                            // entao agora "baixo" aponta precocemente para o proximo cara por conta da shiftação
                            // , entao conserto isso decrementando
                            baixo--;
                            if (flag_nova_pos <= cima) {
                                // mas se aumentou e continua <= cima quer dizer que a chave nao mudou
                                // ou seja, ele foi inserido no final do arquivo binario de registros 
                                // e esta com o maior byte offset de todos
                                // nesse caso, ele só pode estar na posicao "cima"
                                // entao eu nem preciso mais verificar essa posicao
                                // por isso, eu tiro ela do intervalo
                                cima--;
                            } else {
                                // por outro lado, se aumentou e ficou > cima, quer dizer que o cara que estava
                                // em "cima" foi shiftado uma vez para a esquerda
                                // para adequar o intervalo a isso, faz-se 
                                cima--;
                            }
                        }

                        // dito isso, eu poderia ter simplesmente escrito
                        // if (flag_nova_pos == -2 || flag_nova_pos > baixo) {
                        //      baixo--; cima--;
                        // }
                        // mas optei por ser mais claro
                    }
                }

                libera_crime(crime_atual);
                baixo++; // avança na pesquisa
            }
        }

        // se a funcionalidade for a 4, mas nenhum registro tiver sido mostrado
        if (regs_mostrados == 0 && funcionalidade == 4) {
            printf("Registro inexistente.\n");
        }

        // libera variáveis alocadas nessa iteração do loop
        libera_vetor_ate_pos((void**)campos, num_campos-1);
        if (funcionalidade == 7) libera_vetor_ate_pos((void**)campos_atualizar, num_campos_atualizar-1);
    }

    // volto para o início do binario
    seta_consistencia_bin(arq_bin, cabecalho, '1');

    // terminado o loop, se a funcionalidade for a 5 ou a 7
    if (funcionalidade == 5 || funcionalidade == 7) {
        if (arq_idx != NULL) {
            // se eu so der offset pro 0 e escrever o novo vetor de registros de dados,
            // vai sobrar no final do arquivo outros registros. So que quero limpar eles
            // um jeito de fazer isso e fechando o arquivo e abrindo no modo de escrita
            // para limpa-lo
            fclose(arq_idx);
            arq_idx = fopen(nome_arq_idx, "wb");
            cabecalho_indice->nro_reg = num_indices;
            cabecalho_indice->status = '1';
            escreve_cabecalho_ind(arq_idx, cabecalho_indice);
            escreve_dados_gen(arq_idx, dados, tipoVar, num_indices);
        }
    }

    libera_memo_consultas(0, arq_bin, cabecalho, arq_idx, cabecalho_indice, NULL, -1, dados, num_indices);

    // se for a funcionalidade 5 ou a 7, executa a função que se pede para ser executada
    if (funcionalidade == 5 || funcionalidade == 7) {
        binarioNaTela(nome_arq_bin);
        binarioNaTela(nome_arq_idx);
    }
}
