@echo off
rem Сборка на Windows (нужен g++ из MinGW в PATH): build.bat
cd /d "%~dp0"
g++ -std=c++17 -O2 -Wall -Wextra -static src\main.cpp -o shell-emulator.exe
if errorlevel 1 exit /b 1
echo Done. Run: shell-emulator.exe myvfs
