#include <iostream>
#include "../src/include/token.hpp"

int main(void) {
    using compiler::lexer::Token;
    using compiler::lexer::TokenType;
    Token token {TokenType::INT, "123"};

    std::cout << token.toString();

    return 0;
}
