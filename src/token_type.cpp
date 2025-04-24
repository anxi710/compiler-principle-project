#include <iostream>
#include "include/token_type.hpp"

namespace lexer::token {

/**
 * @brief  token::Type 转 string
 * @param  type enum class token::Type 中的一个
 * @return 对应的 string
 */
const std::string tokenType2str(Type type) {
    switch (type) {
        case Type::END:
            return std::string{"END"};
        case Type::IF:
            return std::string{"IF"};
        case Type::ELSE:
            return std::string{"ELSE"};
        case Type::WHILE:
            return std::string{"WHILE"};
        case Type::ID:
            return std::string{"ID"};
        case Type::I32:
            return std::string{"I32"};
        case Type::LET:
            return std::string{"LET"};
        case Type::RETURN:
            return std::string{"RETURN"};
        case Type::MUT:
            return std::string{"MUT"};
        case Type::FN:
            return std::string{"FN"};
        case Type::FOR:
            return std::string{"FOR"};
        case Type::IN:
            return std::string{"IN"};
        case Type::LOOP:
            return std::string{"LOOP"};
        case Type::BREAK:
            return std::string{"BREAK"};
        case Type::CONTINUE:
            return std::string{"CONTINUE"};
        case Type::INT:
            return std::string{"INT"};
        case Type::LPAREN:
            return std::string{"LPAREN"};
        case Type::RPAREN:
            return std::string{"RPAREN"};
        case Type::LBRACE:
            return std::string{"LBRACE"};
        case Type::RBRACE:
            return std::string{"RBRACE"};
        case Type::LBRACK:
            return std::string{"LBRACK"};
        case Type::RBRACK:
            return std::string{"RBRACK"};
        case Type::SEMICOLON:
            return std::string{"SEMICOLON"};
        case Type::COLON:
            return std::string{"COLON"};
        case Type::COMMA:
            return std::string{"COMMA"};
        case Type::OP_PLUS:
            return std::string{"OP_PLUS"};
        case Type::ASSIGN:
            return std::string{"ASSIGN"};
        case Type::OP_MINUS:
            return std::string{"OP_MINUS"};
        case Type::OP_MUL:
            return std::string{"OP_MUL"};
        case Type::OP_DIV:
            return std::string{"OP_DIV"};
        case Type::OP_GT:
            return std::string{"OP_GT"};
        case Type::OP_LT:
            return std::string{"OP_LT"};
        case Type::DOT:
            return std::string{"DOT"};
        case Type::OP_EQ:
            return std::string{"OP_EQ"};
        case Type::OP_NEQ:
            return std::string{"OP_NEQ"};
        case Type::OP_GE:
            return std::string{"OP_GE"};
        case Type::OP_LE:
            return std::string{"OP_LE"};
        case Type::DOTS:
            return std::string{"DOTS"};
        case Type::ARROW:
            return std::string{"ARROW"};
        case Type::SIN_COM:
            return std::string{"SIN_COM"};
        case Type::LMUL_COM:
            return std::string{"LMUL_COM"};
        case Type::RMUL_COM:
            return std::string{"RMUL_COM"};
        default:
            return std::string{"UNKNOWN_TOKEN"};
    } // end switch

    std::cerr << "tokenType2str(): error termination." << std::endl;
    exit(1);
}

} // namespace lexer::token
