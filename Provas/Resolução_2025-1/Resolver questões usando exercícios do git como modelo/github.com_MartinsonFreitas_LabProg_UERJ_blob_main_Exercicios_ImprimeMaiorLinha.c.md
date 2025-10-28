# LabProg_UERJ/Exercicios/ImprimeMaiorLinha.c at main · MartinsonFreitas/LabProg_UERJ · GitHub

**URL:** https://github.com/MartinsonFreitas/LabProg_UERJ/blob/main/Exercicios/ImprimeMaiorLinha.c

---

Skip to content
Navigation Menu
Platform
Solutions
Resources
Open Source
Enterprise
Pricing
Sign in
Sign up
MartinsonFreitas
/
LabProg_UERJ
Public

Couldn't load subscription status. 
Retry

Fork 0
 Star 1
Code
Issues
Pull requests
Actions
Projects
Security
Insights
Files
 main
Exercicios
ImprimeMaiorLinha.c
atof.c
atoi.c
atoi.o
calc_primitiva.c
celsius2fahr.c
conta.c
contaEspacos.c
contabits.c
contabits_switch.c
contabits_switch.o
copia-v1.c
ex_comprime.c
ex_htoi.c
ex_setabits.c
ex_setabits2.c
exercicio1.c
exercicio2-2.c
exercicio2-3.c
exercicio2.c
f2c4
fahr2celsius-v2.c
fahr2celsius-v3.c
fahr2celsius-v4.c
fahr2celsius.c
hello.c
hello.o
histograma.c
histograma2.c
histograma3.c
histograma4.c
histograma5.c
histograma6.c
histograma7.c
htoi.c
impdec.c
imprimeMaiorLinha.c
imprimeMaiorLinha2.c
imprimeMaiorLinha3.c
incrementa.c
incrementa.o
itob.c
pesquisabits.c
potencia.c
remove_caracter.c
Breadcrumbs
LabProg_UERJ/Exercicios
/ImprimeMaiorLinha.c
Latest commit
MartinsonFreitas
Atualização de arquivos em 01-10-2025
73ee4fd
 · 
History
History
File metadata and controls
Code
Blame
Raw
1
2
3
4
5
6
7
8
9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39
40
41
42
43
44
45
46
47
48
49
50
51
52
53
54
55
56
57
#include <stdio.h>


#define MAXLINHA 10 //tamanho máximo da linha entrada


int lelinha (char [], int);
void copia (char [], char []);


/* imprime maior linha entrada */
int main(void)
{
    int tam;               // tamanho atual da linha
    int max = 0;           // tamanho máximo visto até agora
    char linha [MAXLINHA]; // linha atual
    char maior [MAXLINHA]; // maior linha guardada


    while ((tam = lelinha (linha, MAXLINHA)) > 0) {
        if (tam > max) {
            max = tam;
            copia (maior, linha);
        }
    }


    if (max > 0) { // entrada tinha uma linha
        printf ("%s", maior);
    }


    return 0;
}


/* lelinha: lê uma linha em s, retorna tamanho */
int lelinha (char s[], int lim)
{
    int c, i;


    for(i = 0; i < lim - 1 && (c = getchar()) != '\n' && c != EOF; ++i) {
        s[i] = c;
    }


    if (c == '\n') {
        s[i] = c;
        ++i;
    }


    s[i] = '\0';
    return i;
}


/* copia: copia 'de' -> 'para';
   presume que 'para' é grande o suficiente */
void copia (char para[], char de[])
{
    int i = 0;


    while ((para[i] = de[i]) != '\0') {
        ++i;
    }
}
While the code is focused, press Alt+F1 for a menu of operations.