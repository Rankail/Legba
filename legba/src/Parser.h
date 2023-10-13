#ifndef LEGBA_PARSER_H
#define LEGBA_PARSER_H

#include <vector>
#include <functional>

#include "Token.h"
#include "ASTNode/Node.h"
#include "ASTNode/Class.h"
#include "ASTNode/Control.h"
#include "ASTNode/Expression.h"
#include "ASTNode/Literal.h"
#include "ASTNode/Symbol.h"

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

    ValueType valueType();

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
    Node* finishCall(Node* callee);

    // Statement
    Node* declaration();

    Node* varDeclaration(uint16_t flags);
    Node* funcDeclaration(uint16_t flags);
    Node* classDeclaration(uint16_t flags);
    uint16_t qualifiers();
    void checkQualifiers(uint16_t flags, uint16_t forbiddenFlags, std::string type);

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
    ScopeNode* curScope;
    std::vector<std::pair<ScopeNode*, FunctionCallNode*>> unresolvedFunctionCalls;
};


#endif //LEGBA_PARSER_H
