// Точка входа: создаёт оболочку, подключает команды и запускает REPL.

#include "commands.hpp"
#include "shell.hpp"

#include <string>

#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#define STDIN_FILENO 0
#else
#include <unistd.h>
#endif

int main(int argc, char* argv[]) {
    // Имя VFS можно передать первым аргументом: ./shell-emulator myvfs
    std::string vfsName = argc > 1 ? argv[1] : "vfs";

    Shell shell(vfsName);
    commands::registerAll(shell);

    // Если ввод не с терминала (пайп/файл), печатаем введённые команды,
    // чтобы вывод выглядел как интерактивная сессия.
    shell.setEcho(!isatty(STDIN_FILENO));
    return shell.run();
}
