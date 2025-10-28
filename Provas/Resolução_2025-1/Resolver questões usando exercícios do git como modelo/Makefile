# Makefile para os exercícios da Prova P1
# Autor: Manus AI (Refatorado para o LabProg_UERJ)

# Flags de compilação exigidas pela prova
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99

# Arquivos fonte e executáveis
Q1_SRC = provap1-q1-nomeAluno.c
Q1_BIN = provap1-q1-nomeAluno

Q2_SRC = provap1-q2-nomeAluno.c
Q2_BIN = provap1-q2-nomeAluno

# Alvo padrão: compila todos
all: $(Q1_BIN) $(Q2_BIN)

# Regra para compilar a Questão 1 (Soma de Primos)
# Adiciona -lm para o caso de usar sqrt() ou outras funções matemáticas
$(Q1_BIN): $(Q1_SRC)
	$(CC) $(CFLAGS) $< -o $@ -lm

# Regra para compilar a Questão 2 (Substituição e Formatação)
$(Q2_BIN): $(Q2_SRC)
	$(CC) $(CFLAGS) $< -o $@

# Regra para limpar os arquivos executáveis
clean:
	rm -f $(Q1_BIN) $(Q2_BIN)

.PHONY: all clean
