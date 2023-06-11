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
 * Calcula o tamanho em bytes que um registro ocupa(ria) no binário
 *
 * Não leva em conta os $ entre o último | e o #. Para isso, se usa crime->tamanho_real
 * @param crime Pointeiro para o crime
 * @return o tamanho em bytes do crime
 */
int tamanho_crime(crime_t* crime) {
    if (crime == NULL) return 0;

    int tamanho_atual = 4 + 10 + 4 + 12 + 1;        // campos fixos
    tamanho_atual += strlen(crime->lugarCrime);     // conteúdo de campo variável
    tamanho_atual += strlen(crime->descricaoCrime); // conteúdo de outro campo variável
    tamanho_atual += 3;                             // delimitadores (dois | e um #)

    return tamanho_atual;
}

/**
 *
 * Dado um ponteiro para um crime, aloca memória para outro crime,
 * faz as propriedades deles serem as mesmas e retorna o endereço
 * da região alocada
 *
 * @param original ponteiro para crime que sera copiado
 * @return ponteiro para crime copiado
 */
crime_t* copia_crime(crime_t* original) {
    if (original == NULL) return NULL;

    crime_t* copia = (crime_t*)malloc(sizeof(crime_t));
    if (copia == NULL) return NULL;

    copia->idCrime = original->idCrime;
    copia->numeroArtigo = original->numeroArtigo;
    copia->removido = '0';

    copia_array_char(copia->dataCrime, original->dataCrime, 10);
    copia_array_char(copia->marcaCelular, original->marcaCelular, 12);

    copia->descricaoCrime = NULL;
    copia->lugarCrime = NULL;

    copia->descricaoCrime = (char*)malloc(sizeof(char) * (strlen(original->descricaoCrime) + 1));
    if (copia->descricaoCrime == NULL) {
        libera_crime(copia);
        return NULL;
    }
    copia_array_char(copia->descricaoCrime, original->descricaoCrime,
                     strlen(original->descricaoCrime));
    copia->descricaoCrime[(int)strlen(original->descricaoCrime)] = '\0';

    copia->lugarCrime = (char*)malloc(sizeof(char) * (strlen(original->lugarCrime) + 1));
    if (copia->lugarCrime == NULL) {
        libera_crime(copia);
        return NULL;
    }
    copia_array_char(copia->lugarCrime, original->lugarCrime, strlen(original->lugarCrime));
    copia->lugarCrime[(int)strlen(original->lugarCrime)] = '\0';

    // levando em conta a funcionalidade de update, o tamanho real que essa copia ira ocupar
    // vai ser tamamho_crime(copia) se for inserida no final e o tamanho que era ocupada pela
    // original caso contrário
    copia->tamanho_real = tamanho_crime(copia) <= original->tamanho_real ? original->tamanho_real
                                                                         : tamanho_crime(copia);

    return copia;
}
