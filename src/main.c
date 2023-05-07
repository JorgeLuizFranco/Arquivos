/*
 * ARTHUR QUEIROZ MOURA - 13671532 (100%)
 * JORGE LUIZ FRANCO - 13695091 (100%)
 *
 */


#include "funcionalidades.h"

int main() {
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
    }
}