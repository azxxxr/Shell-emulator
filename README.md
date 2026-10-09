# Эмулятор командной оболочки (C++)

Учебный эмулятор языка оболочки UNIX-подобной ОС.

## Этап 1. REPL

Минимальный прототип: интерактивный цикл «чтение — выполнение — вывод»,
парсер аргументов с поддержкой кавычек и команды-заглушки.

### Реализовано

| Требование | Где |
|---|---|
| CLI-приложение | `src/main.cpp`, `Shell::run()` в `src/shell.hpp` |
| Приглашение содержит имя VFS: `user@<vfs>:~$ ` | `Shell::prompt()` |
| Парсер с кавычками `'...'`, `"..."`, экранированием `\` | `src/parser.hpp` |
| Ошибки: неизвестная команда, неверные аргументы, незакрытая кавычка | `Shell::execute()`, `src/commands.hpp` |
| Заглушки `ls`, `cd` — выводят имя и аргументы | `commands::ls`, `commands::cd` |
| Команда `exit [N]` | `commands::exit` |

### Правила разбора аргументов

- пробелы/табы разделяют аргументы;
- `'текст'` — берётся буквально;
- `"текст"` — внутри работают `\"`, `\\`, `\$`, `` \` ``;
- `\x` вне кавычек — экранирование символа (`my\ file` → `my file`);
- `""` — пустой аргумент; `a"b c"d` → `ab cd`;
- незакрытая кавычка → `syntax error`.

### Коды возврата (как в bash)

- `127` — команда не найдена;
- `2` — синтаксическая ошибка / неверная опция / нечисловой аргумент `exit`;
- `1` — `cd: too many arguments`;
- `exit N` завершает программу с кодом `N mod 256`; Ctrl+D (EOF) — тоже выход.

## Сборка и запуск

Проект кроссплатформенный: только стандартная библиотека C++17, собирается
на macOS, Linux и Windows.

| ОС | Сборка | Запуск |
|---|---|---|
| macOS / Linux | `./build.sh` | `./shell-emulator myvfs` |
| Windows (MinGW) | `build.bat` | `shell-emulator.exe myvfs` |
| Любая (CMake, CLion, Visual Studio) | `cmake -S . -B build && cmake --build build` | `build/shell-emulator myvfs` |

Имя VFS — первый аргумент (по умолчанию `vfs`).

Сборка под все три ОС автоматически проверяется в GitHub Actions
(`.github/workflows/build.yml`), готовые бинарники доступны в артефактах запуска.

## Демонстрация

```bash
./build/shell-emulator myvfs < demo/demo_input.txt
```

Входные команды — `demo/demo_input.txt`, результат — `demo/demo_output.txt`:

```
nikita@myvfs:~$ ls -la /home "my documents" 'single quoted' a"b c"d
[stub] command: ls
[stub] args (5): "-la" "/home" "my documents" "single quoted" "ab cd"
nikita@myvfs:~$ cd "Program Files"
[stub] command: cd
[stub] args (1): "Program Files"
nikita@myvfs:~$ echo "not implemented"
myvfs: echo: command not found
nikita@myvfs:~$ cd a b
myvfs: cd: too many arguments
nikita@myvfs:~$ ls -z
ls: invalid option -- 'z'
nikita@myvfs:~$ ls "unclosed quote
myvfs: syntax error: unexpected EOF while looking for matching `"'
nikita@myvfs:~$ exit abc
exit
myvfs: exit: abc: numeric argument required
```

## Структура

```
src/
  main.cpp      — точка входа
  parser.hpp    — разбор строки на аргументы (кавычки, экранирование)
  shell.hpp     — ядро: REPL, приглашение, поиск и запуск команд
  commands.hpp  — команды ls, cd, exit
demo/           — пример сессии
```

Код разнесён по заголовочным файлам (header-only), поэтому для сборки
достаточно скомпилировать один `src/main.cpp`:

```bash
g++ -std=c++17 src/main.cpp -o shell-emulator
```
