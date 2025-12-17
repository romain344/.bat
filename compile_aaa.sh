#!/bin/bash

# Script de compilation pour .aaa.c vers Windows
# Nécessite mingw-w64 installé

echo "Compilation de .aaa.c pour Windows..."

x86_64-w64-mingw32-gcc .aaa.c -o .aaa.exe -lnetapi32 -municode

if [ $? -eq 0 ]; then
    echo "✓ Compilation réussie : .aaa.exe créé"
    echo ""
    echo "Pour exécuter sur Windows :"
    echo "  1. Transférez .aaa.exe sur une machine Windows"
    echo "  2. Lancez en tant qu'administrateur"
    echo "  3. Le programme demandera un nom d'utilisateur Windows"
else
    echo "✗ Erreur de compilation"
    exit 1
fi
