
Objetivo do exercício:

1. Criar um programa que gere vetores de números inteiros aleatórios com tamanhos:
    1.000
    10.000
    100.000
    1.000.000

2. Aplique ao mesmo vetor:
    Bubble Sort
    Shell Sort
    Quick Sort (já implementado, como exemplo)

3. Mostre o tempo de execução de cada algoritmo para cada tamanho de vetor.

4. Expanda o projeto para testar também os algoritmos Insertion Sort e Selection Sort

5. Estrutura do projeto
Organize seu projeto em arquivos separados:
sorting/ 
├── main.c # Função principal: gera vetores, chama os algoritmos e mede o tempo 
├── bubble.c # Implementação do Bubble Sort 
├── shell.c # Implementação do Shell Sort 
├── quick.c # Implementação do Quick Sort 
├── sort.h # Protótipos das funções de ordenação 
├── utils.c # Funções de apoio
├── utils.h # Protótipos das funções de apoio
└── Makefile # Arquivo de automação da compilação

O algoritmo bubblesort pode ser descito como:
Começamos percorrendo o vetor do início ao fim, comparando cada elemento com o próximo. Se o elemento atual for maior que o próximo, trocamos os dois de posição. Ao final dessa primeira varredura, o maior elemento estará corretamente posicionado na última posição. Em seguida, repetimos o processo, mas agora ignorando a última posição já ordenada. A cada passagem completa, mais um elemento se posiciona no lugar correto, e o trecho restante a ser ordenado diminui.
O algoritmo termina quando uma passagem completa não realiza nenhuma troca, o que significa que todos os elementos estão em ordem.

O funcionamento do algoritmo Shell Sort é o seguinte: em vez de comparar e inserir elementos adjacentes (como faz o Insertion Sort), o Shell Sort começa comparando elementos que estão separados por um determinado intervalo, chamado de . Inicialmente, esse gap é relativamente grande (geralmente cerca de metade do tamanho do vetor), e os elementos são comparados e reordenados em cada “subsequência” formada por elementos que estão nessa distância. Após essa primeira passagem, o gap é reduzido, geralmente pela metade, e o processo se repete, com comparações acontecendo entre elementos cada vez mais próximos.
À medida que o gap diminui, o vetor vai se tornando cada vez mais ordenado, o que facilita o trabalho das últimas passagens. Quando o gap chega a 1, o algoritmo realiza uma última varredura equivalente a um Insertion Sort tradicional, mas sobre um vetor que já está quase ordenado, tornando essa etapa muito mais rápida do que seria no início.

A ideia central do Quick Sort é escolher um elemento do vetor chamado e reorganizar os demais elementos de forma que todos os menores que o pivô fiquem à sua esquerda e todos os maiores fiquem à sua direita. Esse processo é chamado de . Depois que o pivô está na posição correta, o algoritmo aplica o mesmo procedimento recursivamente às duas partes restantes do vetor, à esquerda e à direita do pivô.
O processo termina quando os subvetores têm tamanho 0 ou 1, pois nesses casos já estão ordenados. Ao final da execução, todo o vetor estará ordenado.

O funcionamento do algoritmo Selection Sort é o seguinte: começamos procurando o menor elemento em todo o vetor e o trocamos com o elemento da primeira posição. Assim, garantimos que o primeiro elemento está corretamente ordenado. Em seguida, repetimos o processo, mas agora considerando apenas a parte não ordenada (do segundo elemento em diante): procuramos o menor elemento nessa parte e o trocamos com o elemento da segunda posição. Esse procedimento continua até que todo o vetor esteja ordenado.
Ao final de cada passagem, um novo elemento é colocado na posição correta, e a parte ordenada cresce da esquerda para a direita. Mesmo que o vetor já esteja parcialmente ordenado, o Selection Sort percorre sempre toda a parte não ordenada para encontrar o menor elemento, realizando o mesmo número de comparações independentemente da entrada.

O funcionamento básico do Insertion Sort é o seguinte: consideramos que o primeiro elemento do vetor já está ordenado. A partir do segundo elemento, percorremos o vetor da esquerda para a direita e, para cada elemento, buscamos a posição correta dentro da parte já ordenada à esquerda. Fazemos isso deslocando os elementos maiores uma posição à direita até encontrar o lugar onde o novo elemento deve ser inserido.
Por exemplo, se estivermos no índice i, o algoritmo compara o elemento na posição i com os anteriores (i−1,i−2,…) até encontrar a posição adequada para inseri-lo, mantendo a parte inicial do vetor sempre ordenada. Em seguida, ele avança para o próximo elemento e repete o processo até o fim do vetor.
