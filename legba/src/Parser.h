#ifndef LEGBA_PARSER_H
#define LEGBA_PARSER_H

#include <vector>
#include <functional>

#include "Token.h"
#include "ASTNode.h"

class Parser {
public:
    Parser();

    bool parse(std::vector<Token> const& tokens);

    // Error
    void errorAtCurrent(const std::string& msg, bool noThrow = false);
    void error(const std::string& msg, bool noThrow = false);
    void errorAt(Token* token, const std::string& msg, bool noThrow = false);
    void synchronize();

    // Utility
    bool match(TokenType type);
    Token consume(TokenType type, const std::string& msg);
    bool check(TokenType type);
    void advance();
    bool isAtEnd();
    Token peek();
    Token previous();
    void printEnv();

    // Expression
    Node* expression();
    Node* assignement();
    Node* logical_or();
    Node* logical_and();
    Node* equality();
    Node* comparison();
    Node* term();
    Node* factor();
    Node* unary();
    Node* primary();
    Node* call();

    // Statement
    Node* declaration();

    Node* varDeclaration();
    Node* funcDeclaration(std::string kind);

    Node* statement();

    Node* block();
    Node* ifStatement();
    Node* whileStatement();
    Node* forStatement();
    Node* returnStatement();
    Node* expressionStatement();


private:
    std::vector<Token> tokens;
    int current;
    bool hadError;
    ScopeNode* rootScope;
    ScopeNode* scope;
};


#endif //LEGBA_PARSER_H
