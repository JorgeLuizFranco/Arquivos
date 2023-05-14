#include "busca.h"

/**
 * Faz uma busca binária nos índices e com isso define baixo_reg e alto_reg
 * de maneira que todos os registros de índices de posições >= baixo_reg e <= alto_reg
 * tem a chave de busca especificada
 * se a chave de busca especificada não estiver presente, esses valores são -1 e -2
 * 
 * @param ind_ind vetor de registros de índice genéricos
 * @param num_regs tamanho desse vetor
 * @param baixo_reg ponteiro para a variável baixo_reg, já explicada
 * @param alto_reg análogo, já explicado
 * @param chaveBusca a chave de busca que está sendo buscada no vetor de índices
 * @param tipo_var tipo de índice genérico (de string(1) ou de int(0))
 * @param flag_dinamica so e relevante pra se for string. Ela ajuda a comparar chaves de busca
 * que são strings. Ela é melhor explicada na função chamada aqui que está usando essa flag
*/
void busca_bin_campos(void** ind_int, int num_regs, int* baixo_reg, int* alto_reg, void* chaveBusca, int tipo_var, int flag_dinamica) {
    // a busca binária é feita com base na função compara_chave_busca de nome auto-explicativo, que lida com o caso
    // em que a chave é int ou string

    // quero que *baixo aponte pro menor para e *alto aponte para o maior cara que satisfaçam a busca
    // por enquanto, seto como -1 e -2 seus valores, como indicação de que não existe a chave buscada
    *baixo_reg = -1;
    *alto_reg = -2;

    // primeiramente, acho o menor índice tal que sua chave é maior ou igual à chave de busca
    // faço isso com uma busca binária e guardo a resposta em "resultado", ou seja, uso um
    // algoritmo de lower_bound clássico
    int baixo = 0, alto = num_regs - 1;
    int meio;
    int resultado=-1;

    void* reg_meio;

    while (baixo <= alto) {

        meio = (baixo + alto) / 2;
        
        reg_meio = ind_int[meio];

        if (compara_chave_busca(reg_meio, chaveBusca, 1, tipo_var, flag_dinamica)) {
            alto = meio - 1;
            resultado = meio;
        } else {
            baixo = meio + 1;
        }
    }

    // se resultado = -1, quer dizer que nao tem nenhum índice >= à chave de busca. Portanto ela não está presente
    // se algum resultado for encontrado, mas esse resultado for uma chave de busca diferente da que quero, então quer dizer
    // que eu apenas consegui encontrar uma chave de busca maior nesse vetor
    // em ambos os casos, paro aqui e continuo com -1 e -2 como valores finais
    if (resultado == -1 || compara_chave_busca(ind_int[resultado], chaveBusca, 0, tipo_var, flag_dinamica) == 0)
        return;
    *baixo_reg = resultado;

    // agora que sei que a chave de busca está presente, procuro pelo menor índice que é estritamente
    // maior que a minha chave de busca e guardo em "resultado"
    // faço isso usando uma versão da busca binária que implementa o clássico algoritmo de upper bound
    
    baixo = 0, alto = num_regs - 1;
    resultado = -1;
    while (baixo <= alto) {
        meio = (baixo + alto) / 2;

        reg_meio = ind_int[meio];
        //reg_meio > chaveBusca?
        if (compara_chave_busca(reg_meio, chaveBusca, -1, tipo_var, flag_dinamica) == 0) {
            alto = meio - 1;
            resultado = meio;
        } else {
            baixo = meio + 1;
        }
    }

    // se resultado = -1, nao tem ninguem estritamente maior que a chave de busca
    // eu também sei que, como o código chegou até aqui, a chave de busca está presente no arquivo
    // então dito isso, a única possibilidade é que a partir do índice *baixo_reg, todos os registros
    // tem essa mesma chave de busca

    // se resultado for diferente de -1, quer dizer que realmente esse menor indice estritamente maior
    // existe. Então pego ele menos 1, porque alguém menor que ele tem que ser menor ou igual a chave de busca
    // em particular, será obviamente igual

    *alto_reg = resultado != -1 ? resultado-1 : num_regs-1;
}

/**
 * procura um registro criminal para saber qual a posição do registro de índices associado a ele
 * 
 * @param crime_atual registro em questão
 * @param offset_atual informa o offset do registro criminal no arquivo de registros
 * @param dados vetor de registros de índice
 * @param num_dados tamanho desse vetor
 * @param nome_campo nome do campo associado ao arquivo de índices
 * @param tipoVar tipo de índice em questão (de int(0) ou de string(1))
 * 
 * @return índice do vetor de índices que corresponde ao registro criminal em questão
 * -1 se não for encontrado
 * -2 caso ocorra algum erro de alocação
*/
int indice_procura_registro(crime_t* crime_atual, long long int offset_atual, void** dados, int num_dados, char* nome_campo, int tipoVar) {
    if (dados == NULL) return -1; // lista vazia
    
    int baixo_reg = 0, alto_reg = num_dados-1; // variáveis para busca binária que será feita
    void* dado_atual; // variável auxiliar na busca binária
    void* dado_crime = pega_dado_generico(crime_atual, nome_campo, -1, tipoVar); // so serve pra pegar chave busca a partir do nome do campo
    if (dado_crime == NULL) return -2;

    // se o campo "nome_campo" do crime for NULO, esse crime com certeza não está no arquivo de índice
    // se não for nulo (==0), pode estar, então com isso encontra-se baixo_reg e alto_reg, que são os índices tais que
    // entre eles todos tem a chave buscada
    if (checa_dado_nulo(dado_crime, tipoVar) == 0) 
        busca_bin_campos(dados, num_dados, &baixo_reg, &alto_reg, pega_chave_generico(dado_crime, tipoVar), tipoVar, 0);
    else
        return -1; // nao esta no arquivo de indices
    free(dado_crime); // nao precisa mais dessa variável

    // se a chave procurada não for encontrada, retorna não encontrado
    if (baixo_reg < 0) return -1;

    // fazer uma busca binária para encontrar aquele índice com o byte offset especificado
    // algoritmo clássico de busca binária
    int mid_reg;
    long long int offset_dado_atual;

    while (baixo_reg <= alto_reg) {

        mid_reg = (baixo_reg + alto_reg) / 2;
        dado_atual = dados[mid_reg];
        offset_dado_atual = pega_offset_generico(dado_atual, tipoVar);

        if (offset_atual > offset_dado_atual) {
            baixo_reg = mid_reg + 1;
        } else if (offset_atual < offset_dado_atual) {
            alto_reg = mid_reg - 1;
        } else {
            return mid_reg;
        }
    }

    // se ninguém com o offset especificado for encontrado, retorna não encontrado
    return -1;
}

