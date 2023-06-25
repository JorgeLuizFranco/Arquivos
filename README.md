# Trabalho da disciplina SCC0215 - Organização de Arquivos

## Dupla:

- Arthur Queiroz Moura (NUSP 13671532) - 100%
- Jorge Luiz Franco (NUSP 13695091) - 100%

## Como organizamos os .c e os .h?

O Makefile está na raiz do zip e os arquivos do código estão na pasta src. Para deixar o código mais limpo, criamos vários arquivos .c e .h, que estão divididos em pastas.

O programa principal está no main.c, que pega a primeira parte da entrada e joga como argumento de funções implementadas em funcionalidades/funcionalidades.c.

O arquivo funcionalidades/funcionalidades.c usa funções definidas nos vários outros arquivos (como indice_ops/consultas.h, indice_ops/busca.h, indice_ops/insercao.h e etc) para ajudar a implementar as funcionalides.

A pasta "structs" contém os .c e .h dos structs utilizados no trabalho (por exemplo, registro criminal, registros de índices, registros relacionados aos
campos, que ajudam a pegar as entradas do usuário que especificam nomes dos campos
e seus valores e registro de nó e cabeçalho de arvore B*). Também há um par .c/.h "utils_structs" que contém funções auxiliares para esses arquivos.

A pasta "io" tem um par .c/.h que contém funções para mostrar na tela/ler da tela 
registros de crimes.

A pasta "extras" tem um par .c/.h "utils" que contém funções auxilares e um par .c/.h "stringdinamica" que ajuda a ler arquivos .csv

A pasta "arquivos" lida com leitura e escrita em arquivos. Na root, tem leitura_csv. Também tem subpastas por exemplo para mexer no arquivo binário de árvore B*, etc.

A pasta indice_ops contém arquivos relacionados a operações em arquivos de índice linear do trabalho 1

A pasta arvb_ops contém arquivos relacionados a operações em arquivos de índoce de árvore B* do trabalho 2