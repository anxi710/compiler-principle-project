#include <iostream>
#include "include/lexer.hpp"

Lexer::Lexer(const std::string& src) : source(src), pos(0) {}

Token Lexer::next() {
    static const std::vector<std::pair<TokenType, std::regex>> patterns = {
        {TokenType::NUMBER, std::regex(R"(^\d+)")},
        {TokenType::IDENT, std::regex(R"(^[a-zA-Z_]\w*)")},
        {TokenType::PLUS, std::regex(R"(^\+)")},
        {TokenType::MINUS, std::regex(R"(^-)")},
        {TokenType::STAR, std::regex(R"(^\*)")},
        {TokenType::SLASH, std::regex(R"(^/)")},
        {TokenType::LPAREN, std::regex(R"(^\()")},
        {TokenType::RPAREN, std::regex(R"(^\))")},
    };

    while (pos < source.size() && isspace(source[pos])) pos++;

    if (pos >= source.size()) return {TokenType::END, ""};

    std::string view = source.substr(pos);
    for (const auto& [type, regex] : patterns) {
        std::smatch match;
        if (std::regex_search(view, match, regex)) {
            std::cout << match.str(0) << std::endl;
            pos += match.length(0);
            return {type, match.str(0)};
        }
    }

    return {TokenType::INVALID, std::string(1, source[pos++])};
}
