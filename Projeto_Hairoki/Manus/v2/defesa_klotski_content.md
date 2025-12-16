# Defesa do Projeto: Solucionador Otimizado do Puzzle Klotski

## Slide 1: Título

# Solucionador Otimizado do Puzzle Klotski
### Uma Abordagem Algorítmica para o Problema de Busca em Espaços de Estado

**Disciplina:** Laboratório de Programação
**Professor:** João Araújo
**Instituição:** Universidade do Estado do Rio de Janeiro (UERJ)
**Desenvolvimento:** Manus AI

## Slide 2: O Desafio do Klotski

# O Espaço de Estados Exige Otimização Algorítmica

*   **O Problema:** Encontrar a sequência mínima de movimentos para mover a peça alvo ('X') para a posição de saída.
*   **Abordagem Inicial (BFS):** A Busca em Largura (BFS) garante a **solução ótima** (menor número de movimentos).
*   **O Desafio:** O Klotski possui um **vasto espaço de estados** (mais de 65.000 configurações), tornando o BFS puro inviável para soluções profundas.
*   **Solução:** Implementar um **Hash Set** para poda de estados repetidos, transformando a busca de árvore em busca de grafo.

## Slide 3: Otimização 1: Representação Canônica e Hashing

# A Representação de Estado Otimizada Reduz a Sobrecarga de Hash

| Antes (BFS Inicial) | Depois (Otimização 1) | Impacto no Desempenho |
| :--- | :--- | :--- |
| **Representação:** String do tabuleiro (6x6 = 36 caracteres). | **Representação:** Array de Coordenadas (row, col) das peças. | **Redução de Memória:** Armazena apenas coordenadas, não o tabuleiro inteiro. |
| **Hashing:** Cálculo de hash sobre a string longa. | **Hashing:** Cálculo de hash sobre o array de coordenadas ordenadas. | **Hashing Mais Rápido:** Elimina a alocação de string e acelera a comparação de estados. |
| **Consistência:** Ordem das peças não garantida. | **Consistência:** Peças ordenadas por ID (Representação Canônica). | Garante que o mesmo estado sempre gere o mesmo hash. |

## Slide 4: Otimização 2: Busca Bidirecional (Bi-BFS)

# A Bi-BFS Reduz Exponencialmente a Profundidade de Busca

*   **Conceito:** Duas buscas BFS se expandem simultaneamente: uma do estado inicial e outra do estado final.
*   **Vantagem Matemática:** A complexidade de tempo é reduzida de $O(b^d)$ para $O(b^{d/2} + b^{d/2})$, onde $b$ é o fator de ramificação e $d$ é a profundidade da solução.
*   **Implementação:**
    *   Dois Hash Sets e duas Queues.
    *   Função `create_goal_state` para definir o ponto de partida da busca reversa.
    *   Lógica de **detecção de colisão** para encontrar o estado de encontro.
*   **Resultado:** Aumenta drasticamente a capacidade de encontrar soluções em espaços de estado grandes.

## Slide 5: Otimização 3: Algoritmo A*

# A Busca A* Guia a Exploração com Heurística Admissível

*   **Problema do BFS:** Expande todos os nós na mesma profundidade, mesmo os que estão longe do objetivo.
*   **Solução A\*:** Utiliza uma função de custo total $f(n) = g(n) + h(n)$ para priorizar a expansão.
    *   $g(n)$: Custo real do início ao nó (número de movimentos).
    *   $h(n)$: Custo heurístico estimado do nó ao objetivo.
*   **Heurística Utilizada:** **Distância de Manhattan** da peça alvo ('X') até sua posição final.
*   **Estrutura:** Substituição da Queue (FIFO) por uma **Priority Queue (Min-Heap)** para sempre expandir o nó com o menor $f(n)$.

## Slide 6: O Papel da Inteligência Artificial (IA)

# A IA Acelerou o Ciclo de Desenvolvimento e Otimização

*   **Análise e Planejamento:** A IA interpretou o requisito complexo (`Haikori2025_2.pdf`) e estruturou o projeto em fases lógicas.
*   **Concepção Algorítmica:** A IA analisou o código BFS inicial e **propôs as 3 otimizações** (Otimização 1, Bi-BFS, A\*), justificando a escolha de cada técnica.
*   **Geração e Refatoração de Código:** A IA gerou o código-fonte em C para as novas estruturas de dados (Priority Queue, Hash Set Otimizado) e refatorou as funções de busca.
*   **Depuração Iterativa:** A IA identificou e corrigiu erros de compilação e lógica (ex: erro de *linker*, erro de atribuição de *array* em C) durante a implementação das otimizações.

## Slide 7: Conclusão e Próximos Passos

# O Solucionador Evoluiu para uma Solução Robusta e Eficiente

*   **Conclusão:** O projeto demonstrou a evolução de uma solução de busca ingênua (BFS) para um algoritmo de busca informada e otimizada (A*), capaz de resolver o Klotski de forma eficiente.
*   **Metodologia:** A programação assistida por IA provou ser uma metodologia eficaz para a implementação rápida e correta de algoritmos complexos.
*   **Próximos Passos:**
    *   Implementação de uma heurística mais refinada (ex: Ponderação de Bloqueios).
    *   Testes de desempenho comparativos entre BFS, Bi-BFS e A\* para quantificar o ganho de eficiência.

## Slide 8: Perguntas

# Obrigado!

**Perguntas?**
