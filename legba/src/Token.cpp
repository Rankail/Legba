#include "Token.h"

std::string tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::LEFT_PAREN: return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN: return "RIGHT_PAREN";
        case TokenType::LEFT_BRACE: return "LEFT_BRACE";
        case TokenType::RIGHT_BRACE: return "RIGHT_BRACE";
        case TokenType::LEFT_BRACKET: return "LEFT_BRACKET";
        case TokenType::RIGHT_BRACKET: return "RIGHT_BRACKET";
        case TokenType::COMMA: return "COMMA";
        case TokenType::DOT: return "DOT";
        case TokenType::COLON: return "COLON";
        case TokenType::SEMICOLON: return "SEMICOLON";
        case TokenType::PLUS: return "PLUS";
        case TokenType::MINUS: return "MINUS";
        case TokenType::STAR: return "STAR";
        case TokenType::SLASH: return "SLASH";
        case TokenType::MODULO: return "MODULO";
        case TokenType::RIGHT_ARROW: return "RIGHT_ARROW";
        case TokenType::BANG: return "BANG";
        case TokenType::BANG_EQUAL: return "BANG_EQUAL";
        case TokenType::EQUAL: return "EQUAL";
        case TokenType::EQUAL_EQUAL: return "EQUAL_EQUAL";
        case TokenType::LESS: return "LESS";
        case TokenType::LESS_EQUAL: return "LESS_EQUAL";
        case TokenType::GREATER: return "GREATER";
        case TokenType::GREATER_EQUAL: return "GREATER_EQUAL";
        case TokenType::IDENTIFIER: return "IDENTIFIER";
        case TokenType::STRING: return "STRING";
        case TokenType::CHAR: return "CHAR";
        case TokenType::INTEGER: return "INTEGER";
        case TokenType::DOUBLE: return "DOUBLE";
        case TokenType::AND: return "AND";
        case TokenType::OR: return "OR";
        case TokenType::BIN_AND: return "BIN_AND";
        case TokenType::BIN_OR: return "BIN_OR";
        case TokenType::BIN_XOR: return "BIN_XOR";
        case TokenType::BIN_NOT: return "BIN_NOT";
        case TokenType::CLASS: return "CLASS";
        case TokenType::PUBLIC: return "PUBLIC";
        case TokenType::PROTECTED: return "PROTECTED";
        case TokenType::STATIC: return "STATIC";
        case TokenType::CONST: return "CONST";
        case TokenType::VIRTUAL: return "VIRTUAL";
        case TokenType::IF: return "IF";
        case TokenType::ELSE: return "ELSE";
        case TokenType::FUNCTION: return "FUNCTION";
        case TokenType::FOR: return "FOR";
        case TokenType::WHILE: return "WHILE";
        case TokenType::TRUE: return "TRUE";
        case TokenType::FALSE: return "FALSE";
        case TokenType::SUPER: return "SUPER";
        case TokenType::THIS: return "THIS";
        case TokenType::RETURN: return "RETURN";
        case TokenType::VAR: return "VAR";
        case TokenType::END_OF_FILE: return "END_OF_FILE";
        case TokenType::ERROR: return "ERROR";
    }
    return "Unknown token";
}

std::ostream& operator<<(std::ostream& os, const Token& t) {
    os << t.line << ':' << t.index << ' ' << tokenTypeToString(t.type) << ' ' << t.lexeme;
    return os;
}
