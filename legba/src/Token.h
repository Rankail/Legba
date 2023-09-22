#ifndef LEGBA_TOKEN_H
#define LEGBA_TOKEN_H

#include <string>
#include <iostream>

enum class TokenType {
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE, LEFT_BRACKET, RIGHT_BRACKET,
    COMMA, DOT, COLON, SEMICOLON, PLUS, MINUS, STAR, SLASH, MODULO,
    RIGHT_ARROW,

    BANG, BANG_EQUAL,
    EQUAL, EQUAL_EQUAL,
    LESS, LESS_EQUAL,
    GREATER, GREATER_EQUAL,

    IDENTIFIER, STRING, CHAR, INTEGER, DOUBLE,

    AND, OR,
    BIN_AND, BIN_OR, BIN_XOR, BIN_NOT,

    CLASS, PUBLIC, PROTECTED, STATIC, CONST, VIRTUAL,
    
    IF, ELSE, FUNCTION, FOR, WHILE,
    TRUE, FALSE, SUPER, THIS, RETURN, VAR,

    END_OF_FILE, ERROR
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int index;

    friend std::ostream& operator <<(std::ostream& os, Token const& t);
};

std::string tokenTypeToString(TokenType type);


#endif //LEGBA_TOKEN_H
