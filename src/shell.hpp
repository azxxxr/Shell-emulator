#pragma once
// Ядро эмулятора: цикл REPL, приглашение, поиск и запуск команд.

#include "parser.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <map>
#include <string>
#include <utility>
#include <vector>

class Shell {
public:
    using Args = std::vector<std::string>;
    // Команда получает оболочку и аргументы, возвращает код возврата.
    using Command = std::function<int(Shell&, const Args&)>;

    explicit Shell(std::string vfsName, std::istream& in = std::cin,
                   std::ostream& out = std::cout, std::ostream& err = std::cerr)
        : vfsName_(std::move(vfsName)), in_(in), out_(out), err_(err) {
        // Имя пользователя: USER (macOS/Linux), USERNAME (Windows).
        const char* u = std::getenv("USER");
        if (!u || !*u) u = std::getenv("USERNAME");
        user_ = (u && *u) ? u : "user";
    }

    // Регистрирует команду под именем name.
    void addCommand(const std::string& name, Command cmd) {
        commands_[name] = std::move(cmd);
    }

    // Цикл REPL. Возвращает код завершения программы.
    int run() {
        std::string line;
        while (!finished_) {
            out_ << prompt() << std::flush;
            if (!std::getline(in_, line)) {  // Ctrl+D (EOF) — выход, как в bash
                out_ << "exit\n";
                return lastStatus_;
            }
            if (echo_) out_ << line << "\n";
            lastStatus_ = execute(line);
        }
        return exitCode_;
    }

    // Выполняет одну строку. Возвращает код возврата команды.
    int execute(const std::string& line) {
        Args args;
        try {
            args = parse(line);
        } catch (const ParseError& e) {
            err_ << vfsName_ << ": syntax error: " << e.what() << "\n";
            return 2;
        }
        if (args.empty()) return lastStatus_;  // пустая строка

        auto it = commands_.find(args[0]);
        if (it == commands_.end()) {
            err_ << vfsName_ << ": " << args[0] << ": command not found\n";
            return 127;
        }
        return it->second(*this, Args(args.begin() + 1, args.end()));
    }

    // Просит завершить REPL с указанным кодом (используется командой exit).
    void requestExit(int code) {
        exitCode_ = code;
        finished_ = true;
    }

    // Печатать введённую строку (нужно при вводе из файла/пайпа).
    void setEcho(bool on) { echo_ = on; }

    const std::string& vfsName() const { return vfsName_; }
    int lastStatus() const { return lastStatus_; }
    std::ostream& out() { return out_; }
    std::ostream& err() { return err_; }

private:
    // Формат как в bash: user@host:cwd$ — вместо host выводится имя VFS.
    std::string prompt() const { return user_ + "@" + vfsName_ + ":~$ "; }

    std::string vfsName_;
    std::string user_;
    std::istream& in_;
    std::ostream& out_;
    std::ostream& err_;
    std::map<std::string, Command> commands_;
    int lastStatus_ = 0;
    int exitCode_ = 0;
    bool finished_ = false;
    bool echo_ = false;
};
