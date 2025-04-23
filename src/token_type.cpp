#include <iostream>
#include "include/token_type.hpp"

namespace compiler::lexer {

/**
 * @brief  TokenType 转 string
 * @param  type enum class TokenType 中的一个
 * @return 对应的 string
 */
const std::string tokenType2str(TokenType type) {
    switch (type) {
        case TokenType::END:
            return std::string{"END"};
        case TokenType::UNKNOWN:
            return std::string{"UNKNOWN"};
        case TokenType::IF:
            return std::string{"IF"};
        case TokenType::ELSE:
            return std::string{"ELSE"};
        case TokenType::WHILE:
            return std::string{"WHILE"};
        case TokenType::ID:
            return std::string{"ID"};
        case TokenType::I32:
            return std::string{"I32"};
        case TokenType::LET:
            return std::string{"LET"};
        case TokenType::RETURN:
            return std::string{"RETURN"};
        case TokenType::MUT:
            return std::string{"MUT"};
        case TokenType::FN:
            return std::string{"FN"};
        case TokenType::FOR:
            return std::string{"FOR"};
        case TokenType::IN:
            return std::string{"IN"};
        case TokenType::LOOP:
            return std::string{"LOOP"};
        case TokenType::BREAK:
            return std::string{"BREAK"};
        case TokenType::CONTINUE:
            return std::string{"CONTINUE"};
        case TokenType::INT:
            return std::string{"INT"};
        case TokenType::LPAREN:
            return std::string{"LPAREN"};
        case TokenType::RPAREN:
            return std::string{"RPAREN"};
        case TokenType::LBRACE:
            return std::string{"LBRACE"};
        case TokenType::RBRACE:
            return std::string{"RBRACE"};
        case TokenType::LBRACK:
            return std::string{"LBRACK"};
        case TokenType::RBRACK:
            return std::string{"RBRACK"};
        case TokenType::SEMICOLON:
            return std::string{"SEMICOLON"};
        case TokenType::COLON:
            return std::string{"COLON"};
        case TokenType::COMMA:
            return std::string{"COMMA"};
        case TokenType::OP_PLUS:
            return std::string{"OP_PLUS"};
        case TokenType::ASSIGN:
            return std::string{"ASSIGN"};
        case TokenType::OP_MINUS:
            return std::string{"OP_MINUS"};
        case TokenType::OP_MUL:
            return std::string{"OP_MUL"};
        case TokenType::OP_DIV:
            return std::string{"OP_DIV"};
        case TokenType::OP_GT:
            return std::string{"OP_GT"};
        case TokenType::OP_LT:
            return std::string{"OP_LT"};
        case TokenType::DOT:
            return std::string{"DOT"};
        case TokenType::OP_EQ:
            return std::string{"OP_EQ"};
        case TokenType::OP_NEQ:
            return std::string{"OP_NEQ"};
        case TokenType::OP_GE:
            return std::string{"OP_GE"};
        case TokenType::OP_LE:
            return std::string{"OP_LE"};
        case TokenType::DOTS:
            return std::string{"DOTS"};
        case TokenType::ARROW:
            return std::string{"ARROW"};
        case TokenType::SIN_COM:
            return std::string{"SIN_COM"};
        case TokenType::LMUL_COM:
            return std::string{"LMUL_COM"};
        case TokenType::RMUL_COM:
            return std::string{"RMUL_COM"};
        default:
            return std::string{"UNKNOWN_TOKEN"};
    } // end switch

    std::cerr << "tokenType2str(): error termination." << std::endl;
    exit(1);
}

} // namespace compiler::lexer
