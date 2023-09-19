#include "Lexer.h"

#include <format>

std::vector<Token> Lexer::lex(const std::string& source) {
    this->source = source;
    current = 0;
    start = 0;
    indexOffset = current;
    line = 1;

    std::vector<Token> tokens = std::vector<Token>();

    for(;;) {
        Token token = scanToken();

        tokens.push_back(token);

        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
    }

    return tokens;
}

Token Lexer::scanToken() {
    skipWhitespace();

    start = current;

    if (isAtEnd()) return makeToken(TokenType::END_OF_FILE);

    char c = advance();

    if (isAlpha(c)) return identifier();
    if (isDigit(c)) return number();

    switch (c) {
        case '(': return makeToken(TokenType::LEFT_PAREN);
        case ')': return makeToken(TokenType::RIGHT_PAREN);
        case '{': return makeToken(TokenType::LEFT_BRACE);
        case '}': return makeToken(TokenType::RIGHT_BRACE);
        case '[': return makeToken(TokenType::LEFT_BRACKET);
        case ']': return makeToken(TokenType::RIGHT_BRACKET);
        case '+': return makeToken(TokenType::PLUS);
        case '-': return makeToken(TokenType::MINUS);
        case '*': return makeToken(TokenType::STAR);
        case ';': return makeToken(TokenType::SEMICOLON);
        case ':': return makeToken(TokenType::COLON);
        case '.': return makeToken(TokenType::DOT);
        case ',': return makeToken(TokenType::COMMA);
        case '!': return makeToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
        case '=': return makeToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
        case '<': return makeToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
        case '>': return makeToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
        case '"': return string();
        case '&': return makeToken(match('&') ? TokenType::AND : TokenType::BIN_AND);
        case '|': return makeToken(match('|') ? TokenType::OR : TokenType::BIN_OR);
        case '~': return makeToken(TokenType::BIN_NOT);
        case '^': return makeToken(TokenType::BIN_XOR);
    }

    return errorToken(std::format("Unexpected character '{}'", c));
}

Token Lexer::makeToken(TokenType type) {
    Token token;
    token.type = type;
    token.lexeme = source.substr(start, current-start);
    token.line = line;
    token.index = current - indexOffset;
    return token;
}

Token Lexer::errorToken(const std::string& msg) const {
    Token token;
    token.type = TokenType::ERROR;
    token.lexeme = msg;
    token.line = line;
    token.index = current - indexOffset;
    return token;
}

void Lexer::skipWhitespace() {
    for (;;) {
        char c = peek();
        switch (c) {
            case ' ':
            case '\r':
            case '\t':
                advance();
                break;
            case '\n':
                line++;
                indexOffset = current;
                advance();
                break;
            case '/':
                if (peekNext() == '/') {
                    while (peek() != '\n' && !isAtEnd()) advance();
                } else if (peekNext() == '*') {
                    while (!(peek() == '*' && peekNext() == '/') && !isAtEnd()) {
                        if (peek() == '\n') {
                            line++;
                            indexOffset = current;
                        }
                        advance();
                    }
                    advance();
                    advance();
                } else {
                    return;
                }
                break;
            default:
                return;
        }
    }
}

Token Lexer::string() {
    while (peek() != '"' && !isAtEnd()) {
        if (peek() == '\n') line++;
        advance();
    }

    if (isAtEnd()) return errorToken("Unterminated string.");

    advance();
    return makeToken(TokenType::STRING);
}

Token Lexer::number() {
    while (isDigit(peek())) advance();

    if (peek() != '.' || !isDigit(peekNext())) {
        return makeToken(TokenType::NUMBER);
    }

    advance();

    while (isDigit(peek())) advance();

    return makeToken(TokenType::NUMBER);
}

Token Lexer::identifier() {
    while (isAlpha(peek()) || isDigit(peek())) advance();

    return makeToken(identifierType());
}

TokenType Lexer::identifierType() {
    switch(source[start]) {
        case 'c':
            if (current - start > 1) {
                switch (source[start + 1]) {
                case 'l': return checkKeyword(2, 3, "ass", TokenType::CLASS);
                case 'o': return checkKeyword(2, 3, "nst", TokenType::CONST);
                }
            }
            break;
        case 'e': return checkKeyword(1, 3, "lse", TokenType::ELSE);
        case 'f':
            if (current - start > 1) {
                switch (source[start+1]) {
                    case 'a': return checkKeyword(2, 3, "lse", TokenType::FALSE);
                    case 'o': return checkKeyword(2, 1, "r", TokenType::FOR);
                    case 'n': return TokenType::FUNCTION;
                }
            }
            break;
        case 'i': return checkKeyword(1, 1, "f", TokenType::IF);
        case 'p':
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 'r': return checkKeyword(2, 7, "otected", TokenType::PUBLIC);
                    case 'u': return checkKeyword(2, 4, "blic", TokenType::PUBLIC);
                }
            }
            break;
        case 'r': return checkKeyword(1, 5, "eturn", TokenType::RETURN);
        case 's': 
            if (current - start > 1) {
                switch (source[start + 1]) {
                    case 't': return checkKeyword(2, 4, "atic", TokenType::STATIC);
                    case 'u': return checkKeyword(2, 3, "per", TokenType::SUPER);
                }
            }
        case 't':
            if (current - start > 1) {
                switch (source[start+1]) {
                    case 'h': return checkKeyword(2, 2, "is", TokenType::THIS);
                    case 'r': return checkKeyword(2, 2, "ue", TokenType::TRUE);
                }
            }
            break;
        case 'v':
            if (current - start > 1) {
                switch (source[start + 1]) {
                case 'a': return checkKeyword(2, 1, "r", TokenType::VAR);
                case 'i': return checkKeyword(2, 5, "rtual", TokenType::VIRTUAL);
                }
            }
            break;
        case 'w': return checkKeyword(1, 4, "hile", TokenType::WHILE);
    }
    return TokenType::IDENTIFIER;
}

char Lexer::advance() {
    current++;
    return source[current-1];
}

bool Lexer::match(char c) {
    if (isAtEnd()) return false;
    if (source[current] != c) return false;
    advance();
    return true;
}

bool Lexer::isAtEnd() {
    return source.length() <= current;
}

char Lexer::peek() {
    return source[current];
}

char Lexer::peekNext() {
    if (isAtEnd()) return '\0';
    return source[current+1];
}

bool Lexer::isDigit(char c) {
    return '0' <= c && c <= '9';
}

bool Lexer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z')
        || (c >= 'A' && c <= 'Z')
        || c == '_';
}

TokenType Lexer::checkKeyword(int start, int length, const char* rest, TokenType type) {
    if (this->current - this->start == length + start
        && source.substr(this->start + start, length) == rest) {
        return type;
    }

    return TokenType::IDENTIFIER;
}
