#include "./leitura_csv.h"

/**
 * A função le_linha_registro_criminal recebe uma string contendo os dados
 * de um registro de crime em um arquivo de dados e retorna uma estrutura
 * crime_t com esses dados. A função assume que a string passada como
 * parâmetro contém dados válidos e está no formato correto.
 *
 * @param linha_arquivo String com os dados do registro de crime em um
 *                      arquivo de dados.
 * @return Retorna um ponteiro para uma estrutura crime_t com os dados
 *         do registro de crime lido, ou NULL caso não seja possível
 *         alocar memória para a estrutura ou para algum campo da
 *         estrutura.
 */
crime_t* le_linha_registro_criminal(char* linha_arquivo) {
    // Aloca memória para a estrutura crime_t.
    crime_t* crime = (crime_t*)malloc(sizeof(crime_t));
    if (crime == NULL) return NULL;

    // Inicializa o campo removido como '0', indicando que o registro
    // não foi removido.
    crime->removido = '0';

    // Declara variáveis para auxiliar na leitura dos campos.
    char* pont_aux = linha_arquivo; // ponteiro auxiliar
    char* string_atual;
    StringDinamica*
        campo_ds; // essencial para simplificar o trabalho de processar uma linha do arquivo
    sd_constroi(&campo_ds);

    // Lê o campo idCrime.
    while (*pont_aux != ',')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    crime->idCrime = atoi(sd_obtem_string(campo_ds));
    sd_limpa(campo_ds);

    // Lê o campo dataCrime.
    pont_aux++;
    while (*pont_aux != ',')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    string_atual = sd_obtem_string(campo_ds);
    if (strlen(string_atual) == 0) {
        // Caso o campo dataCrime esteja vazio, preenche com '$'.
        for (int i = 0; i < 10; i++)
            crime->dataCrime[i] = '$';
    } else {
        // Caso contrário, copia a string lida para o campo dataCrime
        // e preenche os caracteres restantes com '$'.
        strcpy(crime->dataCrime, string_atual);
        for (int i = strlen(string_atual); i < 10; i++)
            crime->dataCrime[i] = '$';
    }
    sd_limpa(campo_ds);

    // Lê o campo numeroArtigo.
    pont_aux++;
    while (*pont_aux != ',')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    string_atual = sd_obtem_string(campo_ds);
    if (strlen(string_atual) == 0) {
        // Caso o campo numeroArtigo esteja vazio, atribui -1 ao campo.
        crime->numeroArtigo = -1;
    } else {
        // Caso contrário, converte a string lida para um inteiro e
        // atribui ao campo numeroArtigo.
        crime->numeroArtigo = atoi(string_atual);
    }
    sd_limpa(campo_ds);

    // Lê o campo lugarCrime.
    pont_aux++;
    while (*pont_aux != ',')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    string_atual = sd_obtem_string(campo_ds);
    crime->lugarCrime = (char*)malloc((strlen(string_atual) + 1) * sizeof(char));
    if (crime->lugarCrime == NULL) { // lida com o caso de memória insuficiente
        free(crime);
        return NULL;
    }
    strcpy(crime->lugarCrime, string_atual);
    sd_limpa(campo_ds);

    // Lê o campo descricaoCrime.

    pont_aux++;
    while (*pont_aux != ',')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    string_atual = sd_obtem_string(campo_ds);
    crime->descricaoCrime = (char*)malloc((strlen(string_atual) + 1) * sizeof(char));
    if (crime->descricaoCrime == NULL) { // lida com o caso de memória insuficiente
        free(crime->lugarCrime);
        free(crime);
        return NULL;
    }
    strcpy(crime->descricaoCrime, string_atual);
    sd_limpa(campo_ds);

    // Lê o campo marcaCelular.

    pont_aux++;
    while (*pont_aux != '\n')
        sd_adiciona_char(campo_ds, *(pont_aux++));
    string_atual = sd_obtem_string(campo_ds);
    // copia a string lida para o campo marcaCelular e preenche o restante com '$'
    for (int i = 0; i < (int)strlen(string_atual); i++)
        crime->marcaCelular[i] = string_atual[i];
    for (int i = strlen(string_atual); i < 12; i++)
        crime->marcaCelular[i] = '$';
    sd_limpa(campo_ds);
    sd_destroi(&campo_ds);

    return crime;
}

/**
 * Lê uma linha de um arquivo CSV contendo dados de um registro de crime e
 * retorna um ponteiro para uma estrutura crime_t com esses dados.
 *
 * @param arq_csv Arquivo CSV contendo os dados do registro de crime.
 * @return Retorna um ponteiro para uma estrutura crime_t com os dados
 *         do registro de crime lido, ou NULL caso não seja possível
 *         alocar memória para a estrutura ou para algum campo da
 *         estrutura.
 */
crime_t* le_registro_criminal(FILE* arq_csv) {
    char* linha_arquivo = NULL;
    size_t tamanho = 0;
    ssize_t lido;

    crime_t* crime = NULL;

    // se der errado ler a linha ou der certo, mas a linha for vazia, não faz nada
    // se der certo e linha não for vazia, ele dá um parse na linha e pega cada campo
    // , armazenando tudo em "crime"
    if ((lido = getline(&linha_arquivo, &tamanho, arq_csv)) != -1 && linha_arquivo[0] != '\n') {
        crime = le_linha_registro_criminal(linha_arquivo);
    }

    free(linha_arquivo);
    linha_arquivo = NULL;

    return crime;
}