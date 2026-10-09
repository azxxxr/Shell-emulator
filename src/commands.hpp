#pragma once
// Встроенные команды: ls, cd (заглушки) и exit.

#include "shell.hpp"

#include <cstddef>
#include <stdexcept>
#include <string>

namespace commands {

// Заглушка: выводит имя команды и её аргументы.
inline void printStub(Shell& sh, const std::string& name, const Shell::Args& args) {
    sh.out() << "[stub] command: " << name << "\n";
    sh.out() << "[stub] args (" << args.size() << "):";
    for (const auto& a : args) sh.out() << " \"" << a << "\"";
    sh.out() << "\n";
}

inline int ls(Shell& sh, const Shell::Args& args) {
    // Пока проверяем только корректность опций.
    for (const auto& a : args) {
        if (a.size() > 1 && a[0] == '-') {
            for (std::size_t i = 1; i < a.size(); ++i) {
                char o = a[i];
                if (o != 'l' && o != 'a' && o != 'h' && o != 'R' && o != '1') {
                    sh.err() << "ls: invalid option -- '" << o << "'\n";
                    return 2;
                }
            }
        }
    }
    printStub(sh, "ls", args);
    return 0;
}

inline int cd(Shell& sh, const Shell::Args& args) {
    if (args.size() > 1) {
        sh.err() << sh.vfsName() << ": cd: too many arguments\n";
        return 1;
    }
    printStub(sh, "cd", args);
    return 0;
}

inline int exit(Shell& sh, const Shell::Args& args) {
    if (args.size() > 1) {
        sh.err() << sh.vfsName() << ": exit: too many arguments\n";
        return 1;
    }
    sh.out() << "exit\n";  // bash печатает "exit" до проверки аргумента
    int code = sh.lastStatus();
    if (args.size() == 1) {
        try {
            std::size_t pos = 0;
            long v = std::stol(args[0], &pos);
            if (pos != args[0].size()) throw std::invalid_argument("");
            code = static_cast<int>(v & 0xFF);  // как в bash: код по модулю 256
        } catch (const std::exception&) {
            sh.err() << sh.vfsName() << ": exit: " << args[0]
                     << ": numeric argument required\n";
            code = 2;
        }
    }
    sh.requestExit(code);
    return code;
}

// Регистрирует все встроенные команды в оболочке.
inline void registerAll(Shell& sh) {
    sh.addCommand("ls", ls);
    sh.addCommand("cd", cd);
    sh.addCommand("exit", exit);
}

}  // namespace commands
