#include <iostream>
#include <unordered_map>
#include "include/token_type.hpp"

namespace lexer::token {

/**
 * @brief  token::Type 转 string
 * @param  type enum class token::Type 中的一个
 * @return 对应的 string
 */
const std::string tokenType2str(Type type) {
    static const std::unordered_map<lexer::token::Type, std::string> map {
        {Type::END,       "END"},
        {Type::IF,        "IF"},
        {Type::ELSE,      "ELSE"},
        {Type::WHILE,     "WHILE"},
        {Type::ID,        "ID"},
        {Type::I32,       "I32"},
        {Type::LET,       "LET"},
        {Type::RETURN,    "RETURN"},
        {Type::MUT,       "MUT"},
        {Type::FN,        "FN"},
        {Type::FOR,       "FOR"},
        {Type::IN,        "IN"},
        {Type::LOOP,      "LOOP"},
        {Type::BREAK,     "BREAK"},
        {Type::CONTINUE,  "CONTINUE"},
        {Type::INT,       "INT"},
        {Type::LPAREN,    "LPAREN"},
        {Type::RPAREN,    "RPAREN"},
        {Type::LBRACE,    "LBRACE"},
        {Type::RBRACE,    "RBRACE"},
        {Type::LBRACK,    "LBRACK"},
        {Type::RBRACK,    "RBRACK"},
        {Type::SEMICOLON, "SEMICOLON"},
        {Type::COLON,     "COLON"},
        {Type::COMMA,     "COMMA"},
        {Type::OP_PLUS,   "OP_PLUS"},
        {Type::ASSIGN,    "ASSIGN"},
        {Type::OP_MINUS,  "OP_MINUS"},
        {Type::OP_MUL,    "OP_MUL"},
        {Type::OP_DIV,    "OP_DIV"},
        {Type::OP_GT,     "OP_GT"},
        {Type::OP_LT,     "OP_LT"},
        {Type::DOT,       "DOT"},
        {Type::OP_EQ,     "OP_EQ"},
        {Type::OP_NEQ,    "OP_NEQ"},
        {Type::OP_GE,     "OP_GE"},
        {Type::OP_LE,     "OP_LE"},
        {Type::DOTS,      "DOTS"},
        {Type::ARROW,     "ARROW"},
        {Type::SIN_COM,   "SIN_COM"},
        {Type::LMUL_COM,  "LMUL_COM"},
        {Type::RMUL_COM,  "RMUL_COM"}
    };

    if (auto iter = map.find(type);
        iter == map.end()) {
        std::cerr << "tokenType2str(): unknown token type." << std::endl;
        exit(1);
    } else {
        return iter->second;
    }
}

} // namespace lexer::token
