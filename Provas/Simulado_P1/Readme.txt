Uma forma de criptografia usa chaves formadas com todas as letras maiúsculas, entre A e Z, vírgula(,), ponto (.) e espaço.

Cada caractere tem um valor: A=1.. Z=26, vírgula=27, ponto=28 e espaço=29.

O usuário dá uma frase que deve ser convertida numa chave criptográfica válida do seguinte modo: 
Todas as letras da frase devem ser convertidas para maiúsculas, 
e todos os caracteres diferentes de letras, vírgula, ponto e espaço devem ser eliminados. 

Só deve permanecer a primeira ocorrência de cada caractere. 
No final da string, devem ser adicionados os caracteres que faltaram.

Assim, a frase “Celacanto provoca maremoto” deve ser convertida numa chave válida como “CELANTO PRVMBDFGHIJKQSUWXYZ,.”

1) Crie um programa que faça essa conversão entre uma frase e uma chave válida. Pontuação:
	a) Função que faz a conversão correta para letras maiúsculas de todas as letras (1pt)
	b) Função que elimina todos os caracteres diferentes de letra, vírgula, ponto e espaço, 
	deixando apenas a primeira ocorrência de cada um desses caracteres (2pts)
	c) Função que completa a chave corretamente com os caracteres que faltam. (3pts)

2) Dada uma chave criptográfica do tipo acima, faça uma função que converta uma frase, 
passada como parâmetro, usando o seguinte algoritmo: comece com um índice apontando para o 
primeiro caractere na chave. Para cada caractere na frase a ser transcrita, o índice é movido 
para a direita pelo valor do caractere (A=1..Z=26, vírgula=27, ponto=28 e espaço=29) a partir da sua posição atual. 

Voltando para o início se o fim da chave é atingido (ou seja, é circular). 
O valor criptografado é aquele dado pelo caractere onde o índice parou. 
Assim, a frase CELACANTO PROVOCA MAREMOTO, com a chave “CELANTO PRVMBDFGHIJKQSUWXYZ,.” 
produz a saída “APQSXYVEHHAS CGJKKANU,MZIA” (3pts)

3) Considere que todas a funções acima estejam funcionando corretamente. 
Sem uso de variável global, escreva a função principal main() 
que receba uma frase para ser convertida em chave válida e em seguida, 
usando a chave recém-criada, criptografe outra frase lida pelo teclado. (1pt)

Vídeo da resolução em: https://www.youtube.com/watch?v=oUStoesJAcY
Comando export para o Makefile:==> export CFLAGS="-std=c99 -Wall -Werror -pedantic"
Comando para compilar no MakeFile:==> make resolveSimulado
