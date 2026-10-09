#!/bin/sh
# Сборка на macOS / Linux: ./build.sh
set -e
cd "$(dirname "$0")"
CXX="${CXX:-c++}"
"$CXX" -std=c++17 -O2 -Wall -Wextra src/main.cpp -o shell-emulator
echo "Готово. Запуск: ./shell-emulator myvfs"
