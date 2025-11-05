# 1. Definição de Variáveis
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 
EXECUTAVEL = calc
OBJETOS = main.o pilha.o getop.o getch.o

# 2. Regra Principal (Target 'all')
# O alvo padrão é 'all'. Se você rodar apenas 'make', ele tentará construir o executável.
all: $(EXECUTAVEL)

# 3. Regra de Linkagem
# Cria o executável 'calc' a partir dos arquivos objeto (.o).
$(EXECUTAVEL): $(OBJETOS)
	$(CC) $(OBJETOS) -o $(EXECUTAVEL)

# 4. Regras de Compilação dos Arquivos Objeto
# O Makefile usa regras implícitas, mas podemos ser explícitos para incluir as dependências .h.

# main.o depende de main.c e calc.h
main.o: main.c calc.h
	$(CC) $(CFLAGS) -c main.c

# pilha.o depende de pilha.c e calc.h
pilha.o: pilha.c calc.h
	$(CC) $(CFLAGS) -c pilha.c

# getop.o depende de getop.c e calc.h
getop.o: getop.c calc.h
	$(CC) $(CFLAGS) -c getop.c

# getch.o depende de getch.c e calc.h
getch.o: getch.c calc.h
	$(CC) $(CFLAGS) -c getch.c

# 5. Regra de Limpeza (Target 'clean')
# Limpa o diretório de arquivos gerados durante a compilação.
.PHONY: clean
clean:
	rm -f $(OBJETOS) $(EXECUTAVEL)
