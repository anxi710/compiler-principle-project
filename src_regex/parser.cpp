#include "include/parser.hpp"
#include <stdexcept>
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
}

void Parser::advance() {
    current = lexer.next();
}

void Parser::match(TokenType expected) {
    if (current.type == expected) {
        advance();
    } else {
        throw std::runtime_error("Unexpected token: " + current.text);
    }
}

void Parser::parse_expr() {
    parse_term();
    while (current.type == TokenType::PLUS || current.type == TokenType::MINUS) {
        advance();
        parse_term();
    }
}

void Parser::parse_term() {
    parse_factor();
    while (current.type == TokenType::STAR || current.type == TokenType::SLASH) {
        advance();
        parse_factor();
    }
}

void Parser::parse_factor() {
    if (current.type == TokenType::NUMBER || current.type == TokenType::IDENT) {
        advance();
    } else if (current.type == TokenType::LPAREN) {
        advance();
        parse_expr();
        match(TokenType::RPAREN);
    } else {
        throw std::runtime_error("Unexpected factor: " + current.text);
    }
}
