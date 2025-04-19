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
        case TokenType::END:        return "END";
        case TokenType::UNKNOWN:    return "UNKNOWN";
        case TokenType::IF:         return "IF";
        case TokenType::ELSE:       return "ELSE";
        case TokenType::WHILE:      return "WHILE";
        case TokenType::ID:         return "ID";
        case TokenType::I32:        return "I32";
        case TokenType::LET:        return "LET";
        case TokenType::RETURN:     return "RETURN";
        case TokenType::MUT:        return "MUT";
        case TokenType::FN:         return "FN";
        case TokenType::FOR:        return "FOR";
        case TokenType::IN:         return "IN";
        case TokenType::LOOP:       return "LOOP";
        case TokenType::BREAK:      return "BREAK";
        case TokenType::CONTINUE:   return "CONTINUE";
        case TokenType::INT:        return "INT";
        case TokenType::LPAREN:     return "LPAREN";
        case TokenType::RPAREN:     return "RPAREN";
        case TokenType::LBRACE:     return "LBRACE";
        case TokenType::RBRACE:     return "RBRACE";
        case TokenType::LBRACK:     return "LBRACK";
        case TokenType::RBRACK:     return "RBRACK";
        case TokenType::SEMICOLON:  return "SEMICOLON";
        case TokenType::COLON:      return "COLON";
        case TokenType::COMMA:      return "COMMA";
        case TokenType::OP_PLUS:    return "OP_PLUS";
        case TokenType::ASSIGN:     return "ASSIGN";
        case TokenType::OP_MINUS:   return "OP_MINUS";
        case TokenType::OP_MUL:     return "OP_MUL";
        case TokenType::OP_DIV:     return "OP_DIV";
        case TokenType::OP_GT:      return "OP_GT";
        case TokenType::OP_LT:      return "OP_LT";
        case TokenType::DOT:        return "DOT";
        case TokenType::OP_EQ:      return "OP_EQ";
        case TokenType::OP_NEQ:     return "OP_NEQ";
        case TokenType::OP_GE:      return "OP_GE";
        case TokenType::OP_LE:      return "OP_LE";
        case TokenType::DOTS:       return "DOTS";
        case TokenType::ARROW:      return "ARROW";
        case TokenType::SIN_COM:    return "SIN_COM";
        case TokenType::LMUL_COM:   return "LMUL_COM";
        case TokenType::RMUL_COM:   return "RMUL_COM";
        default:                    return "UNKNOWN_TOKEN";
    }


    std::cerr << "tokenType2str(): error termination." << std::endl;
    exit(1);
}

} // namespace compiler::lexer
