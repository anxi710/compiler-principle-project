#include "include/token.hpp"

namespace compiler::lexer {

/* keywords initialization */

const Token Token::END     {TokenType::END, ""};
const Token Token::UNKNOWN {TokenType::UNKNOWN, ""};
const Token Token::IF      {TokenType::IF, "if"};

/* keywords initialization */

/* constructor */

Token::Token(TokenType type, std::string value) : type(type), value(value) {
}

/* constructor */

} // namespace compiler::lexer
