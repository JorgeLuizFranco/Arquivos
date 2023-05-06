#include "registros.h"

/**

*Função que libera a memória alocada para um crime
* @param crime_atual ponteiro para o crime a ser liberado
*/
void libera_crime(crime_t* crime_atual) {
    free(crime_atual->lugarCrime);
    free(crime_atual->descricaoCrime);
    free(crime_atual);
}

/**
 * Calcula o tamanho em bytes que um registro ocupa no binário
 *
 * @param crime Pointeiro para o crime
 * @return o tamanho em bytes do crime
 */
int tamanho_crime(crime_t* crime) {
    if (crime == NULL) return 0;

    int tamanho_atual = 4 + 10 + 4 + 12 + 1; // campos fixos
    tamanho_atual += strlen(crime->lugarCrime); // conteúdo de campo variável
    tamanho_atual += strlen(crime->descricaoCrime); // conteúdo de outro campo variável
    tamanho_atual += 3; // delimitadores (dois | e um #)

    return tamanho_atual;
}
