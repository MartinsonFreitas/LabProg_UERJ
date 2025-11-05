# Prova P1 - 2016-1 (FEN06-04049 - Laboratório de Programação I)
**Data:** 1 de Novembro de 2016
**Professor:** João Araújo
**Tempo:** 3 horas

## Questão 1 (2 pts)
Escreva um programa em ANSI C que leia uma linha do teclado e a imprima na tela de trás para frente. Assim a frase "Celacanto Provoca Maremoto" é impressa "otomeraM acovorP otnacaleC" (2pts)

## Questão 2 (8 pts)
Uma forma simples de criptografia (porém ineficiente) usa a transposição de letras do alfabeto segundo o seguinte esquema:

Cada letra que for encontrada é incrementada de **zero** posições na primeira vez que for encontrada, de **uma** posição na segunda vez, de **duas** posições na terceira vez, e assim sucessivamente. Depois do 'z', volta o 'a' (pode-se usar o módulo % em C para evitar letras acima de 'z'). As letras permanecem maiúsculas ou minúsculas, ou seja, uma letra maiúscula será transposta para uma letra maiúscula e uma letra minúscula será transposta para uma minúscula.

**Exemplo:**
"Celacanto Provoca Maremoto" é transposto como "Celadbnto Prpvqec Mdsfnrus"

Fazer um programa que faça esta transposição de letras de um texto. O programa deve ler várias linhas e só deve parar de criptografar quando for digitada uma linha vazia. A contagem de transposição de letras é contínuo, isto é, não reinicia quando entra nova linha, mas continua do ponto onde parou na linha anterior, mas o valor máximo da contagem de vezes que um caractere aparece deve ser igual a 26, voltando para zero quando ultrapassar este valor.

### Sub-questões:
a. O programa faz a transposição de letras corretamente (2 pts)
b. O programa respeita o "case" das letras e implementa corretamente a transposição quando a letra transposta for maior que 'z'. (2 pts)
c. O programa processa várias linhas (2 pts)
d. A contagem de vezes que aparece cada letra é reiniciada corretamente ao atingir 26 (2 pts)

## Observações:
*   Cada variável global desconta um ponto...
*   Envie os arquivos `provap1-q(1|2)-nomeAluno.c`, sem o executável, para `araujo@eng.uerj.br`
*   Não esqueça de colocar seu nome em cada arquivo fonte.
*   Os programas serão corrigidos usando o compilador `gcc` em ambiente Linux.
*   Qualquer tentativa de desonestidade intelectual, conhecida popularmente como cola, será considerada falta grave e punida com nota zero na prova.
*   Provas com erros de sintaxe (ou seja, erros de compilação) receberão nota ZERO. Seu programa deve ser compilável sem erros.
*   Evite também warnings, pois eles podem indicar na realidade erros lógicos de seu programa.
*   Seu programa deve estar bem indentado, documentado e organizado. A indentação deve deixar clara a estrutura de subordinação dos comandos. Os comentários devem ser esclarecedores.
