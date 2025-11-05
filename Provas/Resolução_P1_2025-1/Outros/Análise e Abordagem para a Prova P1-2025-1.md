# Análise e Abordagem de Solução para a Prova P1-2025-1

Este documento apresenta a análise das questões da Prova P1-2025-1 de Laboratório de Programação I (FEN06-04049) e define a abordagem de solução em ANSI C, seguindo o estilo de codificação observado no repositório GitHub [1] do usuário.

## Estilo de Codificação C (Baseado no Repositório LabProg_UERJ)

O estilo de codificação adotado será o seguinte, conforme o arquivo de exemplo `ImprimeMaiorLinha.c` [2]:

| Característica | Padrão Adotado | Exemplo de Implementação |
| :--- | :--- | :--- |
| **Linguagem** | ANSI C | Uso de bibliotecas padrão como `<stdio.h>`. |
| **Indentação** | 4 espaços | O corpo das funções e blocos de controle (`if`, `while`, `for`) será indentado com 4 espaços. |
| **Comentários** | Comentários de bloco (`/* ... */`) para funções e de linha (`//`) para variáveis e lógica. | `/* função: descrição */` e `int var; // descrição da variável` |
| **Constantes** | Uso de `#define` para constantes. | `#define MAXLINHA 100` |
| **Variáveis Globais** | **Estritamente proibidas**, conforme a restrição da prova ("cada variável global desconta um ponto"). | Todas as variáveis serão declaradas dentro do escopo de funções. |
| **Estrutura** | Uso de funções auxiliares para modularizar o código (ex: `lelinha`, `copia`). | Funções como `eh_primo` e `converte_texto` serão criadas. |

## Questão 1: Soma de Números Primos (2 pts)

**Requisito:** Escrever um programa em ANSI C que leia 2 números quaisquer e calcule a soma de todos os números primos entre os 2, inclusive.

**Abordagem de Solução:**

1.  **Função de Leitura:** Usar `scanf` no `main` para ler os dois números inteiros (`num1` e `num2`).
2.  **Função Auxiliar `eh_primo(int n)`:** Uma função que recebe um inteiro e retorna 1 se for primo, e 0 caso contrário.
    *   Casos base: 0 e 1 não são primos. 2 é o único primo par.
    *   Iterar de 3 até a raiz quadrada de `n`, incrementando de 2 em 2 (após verificar o 2) para otimizar.
3.  **Função de Soma `soma_primos(int a, int b)`:** Uma função que recebe os dois limites.
    *   Garantir que o primeiro limite seja menor ou igual ao segundo (trocar se necessário).
    *   Iterar do menor número ao maior, chamando `eh_primo` para cada número e acumulando a soma.
4.  **Função `main`:** Coordenar a leitura, a chamada à função de soma e a impressão do resultado.

## Questão 2: Substituição de Palavras e Formatação (3 + 1 + 1 + 2 = 7 pts)

**Requisito:** Ler um texto e substituir:
*   "ponto" por "vocábulo"
*   "reta" por "frase"
*   "plano" por "parágrafo"

**Sub-requisitos:**
*   a. Conversão de múltiplos termos.
*   b. Mensagem de erro se a linha > 100 caracteres, mas processar os 100 primeiros e continuar.
*   c. Ler linhas até EOF.
*   d. Quebra de linha da saída a cada 20 caracteres.

**Abordagem de Solução:**

1.  **Constantes:** Definir `MAX_LINHA` como 100 e `MAX_SAIDA_LINHA` como 20 usando `#define`.
2.  **Função de Leitura com Limite `lelinha(char s[], int lim)`:** Adaptar a função `lelinha` do repositório para ler a linha.
    *   Se o número de caracteres lidos for igual ao limite (`lim - 1`) e o próximo caractere não for `\n` ou `EOF`, emitir a mensagem de erro da prova e descartar o restante da linha até o próximo `\n` ou `EOF`.
3.  **Função de Substituição e Quebra de Linha `processa_linha(char linha[])`:** Esta será a função principal de processamento.
    *   **Substituição:** Iterar sobre a string de entrada, procurando as substrings a serem substituídas. A substituição será feita *in-place* em um buffer de saída maior para evitar problemas de redimensionamento e complexidade.
        *   Usar `strstr` (se permitido) ou implementar uma lógica de busca de substring para encontrar as palavras-chave.
        *   Como as palavras de substituição são maiores que as originais, é necessário um buffer de saída. O tamanho máximo da linha de entrada é 100. A maior substituição é de "plano" (5) para "parágrafo" (9). O aumento máximo é de 4 caracteres por substituição. Se houver 20 substituições, o tamanho máximo será `100 + (20 * 4) = 180`. Um buffer de 200 caracteres será suficiente.
    *   **Quebra de Linha (Word Wrap):** Após a substituição, a string de saída deve ser impressa com quebras de linha a cada 20 caracteres, garantindo que as quebras não ocorram no meio de palavras.
        *   Iterar sobre a string de saída e imprimir até o caractere 20. Se o caractere 21 for um espaço, quebrar ali. Se não for, retroceder até o último espaço antes do limite de 20 e quebrar ali.

**Referências**

[1] Repositório GitHub MartinsonFreitas/LabProg_UERJ: https://github.com/MartinsonFreitas/LabProg_UERJ
[2] Exemplo de código C `ImprimeMaiorLinha.c`: https://github.com/MartinsonFreitas/LabProg_UERJ/blob/main/Exercicios/ImprimeMaiorLinha.c
