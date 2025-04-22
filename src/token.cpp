#include <sstream>
#include "include/token.hpp"

namespace compiler::lexer {

/* keywords initialization */

const Token Token::END      {TokenType::END,      ""};
const Token Token::I32      {TokenType::I32,      "i32"};
const Token Token::LET      {TokenType::LET,      "let"};
const Token Token::IF       {TokenType::IF,       "if"};
const Token Token::ELSE     {TokenType::ELSE,     "else"};
const Token Token::WHILE    {TokenType::WHILE,    "while"};
const Token Token::RETURN   {TokenType::RETURN,   "return"};
const Token Token::MUT      {TokenType::MUT,      "mut"};
const Token Token::FN       {TokenType::FN,       "fn"};
const Token Token::FOR      {TokenType::FOR,      "for"};
const Token Token::IN       {TokenType::IN,       "in"};
const Token Token::LOOP     {TokenType::LOOP,     "loop"};
const Token Token::BREAK    {TokenType::BREAK,    "break"};
const Token Token::CONTINUE {TokenType::CONTINUE, "continue"};


/* keywords initialization */

/* constructor */

Token::Token(TokenType type, std::string value) : type(type), value(value) {
}

/* constructor */

/* function member definition */

/**
 * @brief  将 token 格式化为一个 string
 * @return 格式化后的字符串
 */
const std::string Token::toString(void) {
    std::ostringstream oss;

    oss << "<type: " << tokenType2str(type) << ", value: "
        << value << ">" << std::endl;

    return oss.str();
}

/* function member definition */

} // namespace compiler::lexer
