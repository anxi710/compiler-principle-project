#include <sstream>
#include "include/token.hpp"

namespace lexer::token {

/* keywords initialization */

const Token Token::IF       {Type::IF,       "if"};
const Token Token::FN       {Type::FN,       "fn"};
const Token Token::IN       {Type::IN,       "in"};
const Token Token::END      {Type::END,      "#"};
const Token Token::I32      {Type::I32,      "i32"};
const Token Token::LET      {Type::LET,      "let"};
const Token Token::MUT      {Type::MUT,      "mut"};
const Token Token::FOR      {Type::FOR,      "for"};
const Token Token::LOOP     {Type::LOOP,     "loop"};
const Token Token::ELSE     {Type::ELSE,     "else"};
const Token Token::WHILE    {Type::WHILE,    "while"};
const Token Token::BREAK    {Type::BREAK,    "break"};
const Token Token::RETURN   {Type::RETURN,   "return"};
const Token Token::CONTINUE {Type::CONTINUE, "continue"};

/* keywords initialization */

/* function member definition */

/**
 * @brief  将 token 格式化为一个 string
 * @return 格式化后的字符串
 */
const std::string Token::toString() const {
    std::ostringstream oss;
    oss << "<type: " << tokenType2str(type) << ", value: \"" << value << "\">";
    return oss.str();
}

/* function member definition */

} // namespace lexer::token
