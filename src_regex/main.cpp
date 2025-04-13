#include "include/lexer.hpp"
#include "include/parser.hpp"
#include <iostream>

int main() {
    std::string input = "(3 + x) * 42";
    Lexer lexer(input);
    Parser parser(lexer);

    try {
        parser.parse_expr();
        std::cout << "Parse success.\n";
    } catch (const std::exception& e) {
        std::cerr << "Parse error: " << e.what() << "\n";
    }

    return 0;
}
