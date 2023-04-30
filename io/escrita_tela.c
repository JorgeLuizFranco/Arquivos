#include "escrita_tela.h"

/**
*
* Imprime uma string delimitada por caractere ou delimitada por tamanho.
* Ou seja, imprime tanto uma string fixa quanto uma string dinamica
*
* @param string string a ser printada.
*
* @param tamanho tamanho da string, se aplicavel.
*
* @param delimitador caracter delimitador, se aplicavel
*/
void mostra_string_delimitador(char* string, int tamanho, char delimitador) {
    if (tamanho == 0 || string[0] == delimitador) {
        // se o tamanho for 0 ou o primeiro caractere ja e o delimitador
        printf("NULO");
        return;
    }

    if (delimitador == '\0') {
        // programei de modo que se o parâmetro passado for \0, ele apenas
        // imprime a string
        printf("%s", string);
    } else {
        // mas se o delimitador for outra coisa, o printf(string) nao vai funcionar
        // entao manualmente itero pela string, me certificando que nao atinjo
        // o delimitador e eu nao passo do tamanho máximo
        // por exemplo se o delimitador for '$' e o tamanho for 12,
        // ele ou imprime todos os caracteres ate antes do primeiro $ ou
        // todos os 12 caracteres se nao houver '$'
        for (int i = 0; i < tamanho && string[i] != delimitador; i++) {
            printf("%c", string[i]);
        }
    }
}


/**
*
* Função que mostra as informações de um crime na tela
* @param crime_atual ponteiro para o crime a ser mostrado
*
*/
void mostra_crime_tela(crime_t* crime_atual) {
    if (crime_atual->removido == '1') return;

    printf("%d, ", crime_atual->idCrime);
    mostra_string_delimitador(crime_atual->dataCrime, 10, '$');
    if (crime_atual->numeroArtigo == -1)
        printf(", NULO, ");
    else
        printf(", %d, ", crime_atual->numeroArtigo);
    // passo o tamanho como -1 para enfatizar que ele não precisa ser usado
    // nesses casos
    mostra_string_delimitador(crime_atual->lugarCrime, -1, '\0');
    printf(", ");
    mostra_string_delimitador(crime_atual->descricaoCrime, -1, '\0');
    printf(", ");
    mostra_string_delimitador(crime_atual->marcaCelular, 12, '$');
    printf("\n");
}

/**
 * Imprime na tela a informação de um número "n_registros" de crimes armazenados no arq_bin.
 *
 * @param arq_bin Pointeiro para o arquivo binário contendo os crimes.
 * @param n_registros Número de crimes
 */
void mostra_crimes_tela_arq(FILE* arq_bin, int n_registros) {
    
    if (n_registros == 0) {
        printf("Registro inexistente.\n");
    }

    while (n_registros > 0) {
        crime_t* crime_atual = le_crime_bin(arq_bin);

        if (crime_atual == NULL) {
            // em caso de erro de alocação
            erro();
            return;
        }

        if (crime_atual->removido != '1') { 
            mostra_crime_tela(crime_atual);
            n_registros--;
        }
        
        libera_crime(crime_atual); // libera memória alocada
    }
}
