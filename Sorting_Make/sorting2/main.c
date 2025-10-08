#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "sort.h"
#include "utils.h"

double medir_tempo_bubble(int* v, int n) {
    clock_t inicio = clock();
    bubble_sort(v, n);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

double medir_tempo_shell(int* v, int n) {
    clock_t inicio = clock();
    shell_sort(v, n);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

double medir_tempo_quick(int* v, int n) {
    clock_t inicio = clock();
    quick_sort(v, 0, n - 1);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

double medir_tempo_insertion(int* v, int n) {
    clock_t inicio = clock();
    insertion_sort(v, n);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

double medir_tempo_selection(int* v, int n) {
    clock_t inicio = clock();
    selection_sort(v, n);
    clock_t fim = clock();
    return (double)(fim - inicio) / CLOCKS_PER_SEC;
}

int main(void) {
        int tamanhos[] = {1000, 10000, 100000 /*, 1000000*/};
    int num_tamanhos = sizeof(tamanhos) / sizeof(tamanhos[0]);

    printf("==== Teste de Algoritmos de Ordenação ====\n\n");

    for (int t = 0; t < num_tamanhos; t++) {
        int n = tamanhos[t];
        printf("Tamanho do vetor: %d\n", n);

        int* original = malloc(n * sizeof(int));
        if (!original) {
            fprintf(stderr, "Erro: não foi possível alocar memória.\n");
            return 1;
        }

        fill_random(original, n, 1000000);

        int* v_bubble = malloc(n * sizeof(int));
        int* v_shell = malloc(n * sizeof(int));
        int* v_quick = malloc(n * sizeof(int));
        int* v_insertion = malloc(n * sizeof(int));
        int* v_selection = malloc(n * sizeof(int));

        if (!v_bubble || !v_shell || !v_quick || !v_insertion || !v_selection) {
            fprintf(stderr, "Erro: não foi possível alocar memória.\n");
            return 1;
        }

        // Bubble Sort
        for (int i = 0; i < n; i++) v_bubble[i] = original[i];
        double tempo_bubble = medir_tempo_bubble(v_bubble, n);
        printf("Bubble Sort:   %.4f s  %s\n", tempo_bubble,
               is_sorted(v_bubble, n) ? "(OK)" : "(Erro)");

        // Shell Sort
        for (int i = 0; i < n; i++) v_shell[i] = original[i];
        double tempo_shell = medir_tempo_shell(v_shell, n);
        printf("Shell Sort:    %.4f s  %s\n", tempo_shell,
               is_sorted(v_shell, n) ? "(OK)" : "(Erro)");

        // Quick Sort
        for (int i = 0; i < n; i++) v_quick[i] = original[i];
        double tempo_quick = medir_tempo_quick(v_quick, n);
        printf("Quick Sort:    %.4f s  %s\n", tempo_quick,
               is_sorted(v_quick, n) ? "(OK)" : "(Erro)");

        // Insertion Sort
        for (int i = 0; i < n; i++) v_insertion[i] = original[i];
        double tempo_insertion = medir_tempo_insertion(v_insertion, n);
        printf("Insertion Sort:%.4f s  %s\n", tempo_insertion,
               is_sorted(v_insertion, n) ? "(OK)" : "(Erro)");

        // Selection Sort
        for (int i = 0; i < n; i++) v_selection[i] = original[i];
        double tempo_selection = medir_tempo_selection(v_selection, n);
        printf("Selection Sort:%.4f s  %s\n", tempo_selection,
               is_sorted(v_selection, n) ? "(OK)" : "(Erro)");

        printf("\n");

        free(original);
        free(v_bubble);
        free(v_shell);
        free(v_quick);
        free(v_insertion);
        free(v_selection);
    }

    return 0;
}

