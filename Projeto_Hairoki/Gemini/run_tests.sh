#!/bin/bash
# Script de Teste para o Klotski Solver

echo "========================================"
echo "Compilando o projeto..."
gcc -O3 -o klotskti_opt klotski_opt.c
if [ $? -ne 0 ]; then
    echo "Erro na compilação!"
    exit 1
fi
echo "Compilação OK."
echo "========================================"

# Teste 1: Caso Simples
echo "Teste 1: 18passos.txt"
# Usamos o comando 'time' para medir performance
time ./klotski_opt 18passos.txt > saida_18passos.txt
if grep -q "SOLUCAO ENCONTRADA" saida_18passos.txt; then
    echo "✅ [SUCESSO] 18passos resolvido!"
    head -n 5 saida_18passos.txt # Mostra o início da saída
else
    echo "❌ [FALHA] 18passos não resolvido."
fi
echo "----------------------------------------"

# Teste 2: Caso Complexo
echo "Teste 2: engarrafamento.txt"
time ./klotski_opt engarrafamento.txt > saida_engarrafamento.txt
if grep -q "SOLUCAO ENCONTRADA" saida_engarrafamento.txt; then
    echo "✅ [SUCESSO] Engarrafamento resolvido!"
    # Exibe as últimas linhas para ver o número de passos
    grep "SOLUCAO" saida_engarrafamento.txt
else
    echo "❌ [FALHA] Engarrafamento não resolvido."
fi
echo "========================================"
