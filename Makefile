CC = gcc
CFLAGS = -Wall -Wextra -Werror

clean:
	rm *.o

all: main clean

run:
	./main

main: main.o
	$(CC) main.o funcionalidades.o escrita_tela.o escrita_bin.o leitura_bin.o leitura_csv.o utils.o registros.o indices.o escrita_bin_ind.o stringdinamica.o busca.o leitura_bin_ind.o funcoes_anonimas.o -o main

main.o: main.c funcionalidades.o
	$(CC) -c $(CFLAGS) main.c -o main.o




#extras/
stringdinamica.o: extras/stringdinamica.c extras/stringdinamica.h
	$(CC) -c $(CFLAGS) extras/stringdinamica.c -o stringdinamica.o

utils.o: extras/utils.c extras/utils.h
	$(CC) -c $(CFLAGS) extras/utils.c -o utils.o

# arquivos/
escrita_bin.o: arquivos/escrita_bin.c arquivos/escrita_bin.h registros.o
	$(CC) -c $(CFLAGS) arquivos/escrita_bin.c -o escrita_bin.o

leitura_bin.o: arquivos/leitura_bin.c arquivos/leitura_bin.h registros.o
	$(CC) -c $(CFLAGS) arquivos/leitura_bin.c -o leitura_bin.o

leitura_csv.o: arquivos/leitura_csv.c arquivos/leitura_csv.h registros.o stringdinamica.o
	$(CC) -c $(CFLAGS) arquivos/leitura_csv.c -o leitura_csv.o

escrita_bin_ind.o: arquivos/escrita_bin_ind.c arquivos/escrita_bin_ind.h indices.o escrita_bin.o
	$(CC) -c $(CFLAGS) arquivos/escrita_bin_ind.c -o escrita_bin_ind.o
 
leitura_bin_ind.o: arquivos/leitura_bin_ind.c arquivos/leitura_bin_ind.h indices.o escrita_bin.o
	$(CC) -c $(CFLAGS) arquivos/leitura_bin_ind.c -o leitura_bin_ind.o


#io/
escrita_tela.o: io/escrita_tela.c io/escrita_tela.h leitura_bin.o utils.o
	$(CC) -c $(CFLAGS) io/escrita_tela.c -o escrita_tela.o

#./

indices.o: indices.h indices.c utils.o
	$(CC) -c $(CFLAGS) indices.c -o indices.o

registros.o: registros.c registros.h 
	$(CC) -c $(CFLAGS) registros.c -o registros.o

busca.o: busca.c busca.h registros.o utils.o indices.o leitura_bin.o leitura_bin_ind.o
	$(CC) -c $(CFLAGS) busca.c -o busca.o

funcoes_anonimas.o: funcoes_anonimas.c funcoes_anonimas.h escrita_tela.o
	$(CC) -c $(CFLAGS) funcoes_anonimas.c -o funcoes_anonimas.o

funcionalidades.o: funcionalidades.h funcionalidades.c escrita_tela.o escrita_bin.o leitura_bin.o leitura_csv.o escrita_bin_ind.o  leitura_bin_ind.o busca.o funcoes_anonimas.o
	$(CC) -c $(CFLAGS) funcionalidades.c -o funcionalidades.o



