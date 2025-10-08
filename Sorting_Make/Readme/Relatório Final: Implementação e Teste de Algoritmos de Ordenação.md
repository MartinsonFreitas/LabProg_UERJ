# Relatório Final: Implementação e Teste de Algoritmos de Ordenação

Este relatório detalha as implementações e correções realizadas nos arquivos fornecidos, seguindo as instruções do `Readme.txt`, e apresenta os resultados dos testes de desempenho dos algoritmos de ordenação.

## 1. Análise Inicial e Implementações Necessárias

Ao analisar os arquivos iniciais e o `Readme.txt`, foram identificados os seguintes pontos que necessitavam de implementação ou correção:

*   **`bubble.c`**: A implementação original do `bubble_sort` estava incorreta, com variáveis não definidas e lógica de loop inadequada.
*   **`utills.c`**: A função `fill_random` estava vazia, e as funções `print_array` e `is_sorted` (declaradas em `utils.h`) não possuíam implementação.
*   **`sort.h`**: Faltavam os protótipos para os algoritmos `insertion_sort` e `selection_sort`.
*   **`main.c`**: Necessitava de atualizações para incluir as chamadas e medições de tempo para os novos algoritmos (`insertion_sort` e `selection_sort`), além de ajustes para a alocação de memória.
*   **Novos Arquivos**: Era preciso criar `insertion.c` e `selection.c` para as implementações dos algoritmos adicionais.
*   **`Makefile`**: Precisava ser atualizado para incluir os novos arquivos `.c` e `.o` no processo de compilação.

## 2. Implementações e Correções Realizadas

As seguintes ações foram tomadas para atender aos requisitos:

1.  **`bubble.c`**: A função `bubble_sort` foi corrigida para uma implementação padrão e funcional do algoritmo Bubble Sort.
2.  **`utills.c`**: As funções `fill_random`, `print_array` e `is_sorted` foram implementadas. A função `fill_random` preenche um vetor com números aleatórios, `print_array` imprime o conteúdo de um vetor, e `is_sorted` verifica se um vetor está ordenado.
3.  **`sort.h`**: Foram adicionados os protótipos para `insertion_sort` e `selection_sort`.
4.  **`insertion.c`**: Implementação do algoritmo Insertion Sort.
5.  **`selection.c`**: Implementação do algoritmo Selection Sort.
6.  **`main.c`**: Atualizado para incluir as chamadas para `insertion_sort` e `selection_sort`, bem como suas respectivas medições de tempo. Também foram adicionadas as alocações de memória necessárias para os novos vetores de teste.
7.  **`Makefile`**: Atualizado para compilar `insertion.c`, `selection.c` e `utills.c`, e linkar os objetos correspondentes.

Durante a fase de testes, foi observado que a execução com um vetor de 1.000.000 de elementos era excessivamente demorada para algoritmos de complexidade O(n^2) (como Bubble Sort, Insertion Sort e Selection Sort). Conforme orientação, o teste para o tamanho de 1.000.000 foi comentado em `main.c` para permitir a conclusão dos testes em um tempo razoável.

## 3. Resultados dos Testes de Desempenho

O programa foi compilado e executado com sucesso para os tamanhos de vetor de 1.000, 10.000 e 100.000 elementos. Os tempos de execução para cada algoritmo são apresentados abaixo:

```
==== Teste de Algoritmos de Ordenação ====
Tamanho do vetor: 1000
Bubble Sort:   0.0008 s  (OK)
Shell Sort:    0.0001 s  (OK)
Quick Sort:    0.0001 s  (OK)
Insertion Sort:0.0001 s  (OK)
Selection Sort:0.0004 s  (OK)
Tamanho do vetor: 10000
Bubble Sort:   0.1235 s  (OK)
Shell Sort:    0.0010 s  (OK)
Quick Sort:    0.0006 s  (OK)
Insertion Sort:0.0120 s  (OK)
Selection Sort:0.0323 s  (OK)
Tamanho do vetor: 100000
Bubble Sort:   15.7291 s  (OK)
Shell Sort:    0.0142 s  (OK)
Quick Sort:    0.0075 s  (OK)
Insertion Sort:1.1906 s  (OK)
Selection Sort:3.3544 s  (OK)
```

### Análise dos Resultados:

*   **Bubble Sort**: Apresenta os maiores tempos de execução, confirmando sua ineficiência para grandes volumes de dados. Para 100.000 elementos, levou mais de 15 segundos.
*   **Selection Sort e Insertion Sort**: Embora melhores que o Bubble Sort, também demonstram degradação significativa de desempenho com o aumento do tamanho do vetor, levando segundos para 100.000 elementos.
*   **Shell Sort**: Mostra um desempenho consideravelmente melhor que os algoritmos O(n^2), com tempos na ordem de milissegundos para 100.000 elementos.
*   **Quick Sort**: É o algoritmo mais rápido entre os testados, com tempos de execução muito baixos, na ordem de milissegundos, mesmo para 100.000 elementos. Isso se alinha com sua complexidade média de O(n log n).

## 4. Conclusão

Todas as implementações e correções foram realizadas conforme as instruções do `Readme.txt`. O projeto agora inclui os algoritmos Bubble Sort, Shell Sort, Quick Sort, Insertion Sort e Selection Sort, com medição de tempo de execução e verificação de ordenação. Os testes demonstram claramente a diferença de desempenho entre algoritmos de ordenação com diferentes complexidades de tempo, especialmente para grandes conjuntos de dados.

## 5. Arquivos Entregues

Os seguintes arquivos foram modificados ou criados:

*   `bubble.c` (corrigido)
*   `main.c` (atualizado)
*   `Makefile` (atualizado)
*   `sort.h` (atualizado)
*   `utills.c` (implementado)
*   `insertion.c` (novo)
*   `selection.c` (novo)

Além disso, os arquivos `quick.c` e `utils.h` foram mantidos como estavam, pois já estavam corretos ou não necessitavam de modificação direta para esta tarefa.
