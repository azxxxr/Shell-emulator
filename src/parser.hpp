#pragma once
// Разбор командной строки на аргументы.

#include <cstddef>
#include <stdexcept>
#include <string>
#include <vector>

// Ошибка синтаксического разбора (например, незакрытая кавычка).
class ParseError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

// Разбивает строку на аргументы по правилам, близким к POSIX sh:
//  - пробелы и табуляции разделяют аргументы;
//  - '...'  — всё внутри берётся буквально;
//  - "..."  — внутри работает экранирование \" \\ \$ \`;
//  - \x вне кавычек — символ x берётся буквально;
//  - "" и '' дают пустой аргумент;
//  - кавычки можно склеивать с текстом: a"b c"d -> "ab cd".
inline std::vector<std::string> parse(const std::string& line) {
    enum class State { Normal, Single, Double };

    std::vector<std::string> args;
    std::string current;
    bool inToken = false;  // нужно, чтобы "" давало пустой аргумент
    State state = State::Normal;

    for (std::size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        switch (state) {
        case State::Normal:
            if (c == ' ' || c == '\t') {
                if (inToken) {
                    args.push_back(current);
                    current.clear();
                    inToken = false;
                }
            } else if (c == '\'') {
                state = State::Single;
                inToken = true;
            } else if (c == '"') {
                state = State::Double;
                inToken = true;
            } else if (c == '\\') {
                if (i + 1 >= line.size())
                    throw ParseError("unexpected end of line after '\\'");
                current += line[++i];
                inToken = true;
            } else {
                current += c;
                inToken = true;
            }
            break;

        case State::Single:
            if (c == '\'') state = State::Normal;
            else current += c;
            break;

        case State::Double:
            if (c == '"') {
                state = State::Normal;
            } else if (c == '\\' && i + 1 < line.size() &&
                       (line[i + 1] == '"' || line[i + 1] == '\\' ||
                        line[i + 1] == '$' || line[i + 1] == '`')) {
                current += line[++i];
            } else {
                current += c;
            }
            break;
        }
    }

    if (state == State::Single)
        throw ParseError("unexpected EOF while looking for matching `''");
    if (state == State::Double)
        throw ParseError("unexpected EOF while looking for matching `\"'");

    if (inToken) args.push_back(current);
    return args;
}
