/*
 * ARTHUR QUEIROZ MOURA - 13671532 (100%)
 * JORGE LUIZ FRANCO - 13695091 (100%)
 * VEJA O README DA RAIZ DO ZIP PARA MAIS INFORMAÇÕES
 */

#include "./funcionalidades/funcionalidades.h"

int main() {
    // le a primeira linha da entrada do usuário e chama a funcionalidade
    // correspondente
    // as funções estão em funcionalidades.c

    int funcao;
    char nome_arq_csv[100];
    char nome_arq_bin[100];
    char nome_arq_idx[100];
    char nome_campo[20];
    char tipo_campo[10];
    int num_consultas;

    scanf("%d", &funcao);

    switch (funcao) {
    case 1:
        scanf("%s %s", nome_arq_csv, nome_arq_bin);
        csv_para_bin(nome_arq_csv, nome_arq_bin);
        break;

    case 2:
        scanf("%s", nome_arq_bin);
        mostra_crimes_tela(nome_arq_bin);
        break;

    case 3:

        scanf("%s %s %s %s", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx);
        cria_arq_indices(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx);

        break;

    case 4:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        procura_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;

    case 5:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        remove_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;

    case 6:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        insere_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;

    case 7:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        atualiza_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;

    case 8:

        break;

    case 9:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        arvb_procura_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;

    case 10:

        scanf("%s %s %s %s %d", nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, &num_consultas);
        arvb_insere_registros(nome_arq_bin, nome_campo, tipo_campo, nome_arq_idx, num_consultas);

        break;
    }
}