#ifndef LEGBA_LEXER_H
#define LEGBA_LEXER_H

#include <vector>
#include <string>

#include "Token.h"

class Lexer {
public:
    Lexer() 
        : current(0), start(0), line(1), indexOffset(0) { }

    std::vector<Token> lex(const std::string& source);

private:
    Token scanToken();

    Token makeToken(TokenType type);
    Token errorToken(const std::string& msg) const;

    void skipWhitespace();

    Token string();
    Token char_();
    Token number();
    Token identifier();

    TokenType identifierType();
    TokenType checkKeyword(int start, int length, const char* rest, TokenType type);

    char advance();
    bool match(char c);
    bool isAtEnd();
    char peek();
    char peekNext();

    bool isDigit(char c);
    bool isAlpha(char c);

private:
    std::string source;
    int current;
    int start;
    int line;
    int indexOffset;
};


#endif //LEGBA_LEXER_H
