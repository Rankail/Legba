#include "Parser.h"

#include "Error.h"

#include <format>

Parser::Parser()
    : hadError(false), current(0), tokens(), rootScope(nullptr), scope(nullptr) {
}

bool Parser::parse(const std::vector<Token> &tokens) {
    this->tokens = tokens;
    hadError = false;
    rootScope = new ScopeNode();
    scope = rootScope;
    
    while (!isAtEnd()) {
        try {
            scope->addStatement(declaration());
        } catch (ParserError const& e) {
            synchronize();
        }
    }

    return !hadError;
}

// Error

void Parser::errorAtCurrent(const std::string &msg) {
    errorAt(&tokens[current], msg);
}

void Parser::error(const std::string &msg) {
    errorAt(&tokens[current-1], msg);
}

void Parser::errorAt(Token *token, const std::string &msg) {
    auto e = std::format("[{}:{}] Error", token->line, token->index);

    if (token->type == TokenType::END_OF_FILE) {
        e += " at end";
    } else if (token->type == TokenType::ERROR) {

    } else {
        e += std::format(" at '{}'", token->lexeme);
    }

    e += std::format(": {}\n", msg);
    fprintf(stderr, e.c_str());
    hadError = true;

    throw ParserError(e.c_str());
}

void Parser::synchronize() {
    while (peek().type != TokenType::END_OF_FILE) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUNC:
            case TokenType::VAR:
            case TokenType::FOR:
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::RETURN:
                return;
            default:;
        }

        advance();
    }
}

// Utility

bool Parser::match(TokenType type) {
    if (!check(type)) return false;
    advance();
    return true;
}

Token Parser::consume(TokenType type, const std::string& msg) {
    if (peek().type == type) {
        advance();
        return previous();
    }

    errorAtCurrent(msg);
}

bool Parser::check(TokenType type) {
    return peek().type == type;
}

void Parser::advance() {
    for (;;) {
        current++;
        if (peek().type != TokenType::ERROR) break;

        errorAtCurrent(peek().lexeme);
    }
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return this->tokens[current - 1];
}

void Parser::printEnv() {
    std::cout << rootScope->toString() << std::endl;
}

// Expression

Node* Parser::expression() {
    return assignement();
}

Node* Parser::assignement() {
    Node* expr = logical_or();

    if (!match(TokenType::EQUAL)) {
        return expr;
    }

    Token equals = previous();
    Node* value = assignement();

    if (expr->getType() == NodeType::VARIABLE) {
        return new BinaryNode(new OpNode(TokenType::EQUAL), expr, value);
    }

    errorAt(&equals, "Invalid assignement target");
}

Node* Parser::logical_or() {
    Node* expr = logical_and();

    while (match(TokenType::OR)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = logical_and();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::logical_and() {
    Node* expr = equality();

    while (match(TokenType::AND)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = equality();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::equality() {
    Node* expr = comparison();

    while (match(TokenType::BANG_EQUAL) || match(TokenType::EQUAL_EQUAL)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = comparison();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::comparison() {
    Node* expr = term();

    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = term();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::term() {
    Node* expr = factor();

    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = factor();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::factor() {
    Node* expr = primary();

    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = primary();
        expr = new BinaryNode(op, expr, right);
    }

    return expr;
}

Node* Parser::unary() {
    if (match(TokenType::BANG) || match(TokenType::MINUS)) {
        OpNode* op = new OpNode(previous().type);
        Node* right = unary();
        return new UnaryNode(op, right);
    }

    return primary();
}

Node* Parser::primary() {
    if (match(TokenType::FALSE)) {
        return new BoolNode(false);
    }
    if (match(TokenType::TRUE)) {
        return new BoolNode(true);
    }
    
    if (match(TokenType::STRING)) {
        return new StringNode(previous());
    }

    if (match(TokenType::NUMBER)) {
        return new NumberNode(previous());
    }

    if (match(TokenType::IDENTIFIER)) {
        return new VariableNode(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN)) {
        Node* node = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return node;
    }

    error("Malformed expression");
}

// Statements

Node* Parser::declaration() {
    switch (peek().type) {
        case TokenType::VAR: return varDeclaration();
        default: return statement();
    }
}

Node* Parser::statement() {
    switch (peek().type) {
        case TokenType::LEFT_BRACE: return block();
        case TokenType::IF: return ifStatement();
        default: return expressionStatement();
    }
}

Node* Parser::varDeclaration() {
    advance(); // VAR

    Token name = consume(TokenType::IDENTIFIER, "Expected variable name.");

    Node* initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");
    if (initializer != nullptr) {
        return new BinaryNode(new OpNode(TokenType::VAR), new VariableNode(name.lexeme), initializer);
    } else {
        return new UnaryNode(new OpNode(TokenType::VAR), new VariableNode(name.lexeme));
    }
}

Node* Parser::block() {
    advance(); // {

    scope = new ScopeNode(scope);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        scope->addStatement(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");

    Node* newScope = scope;
    
    scope = scope->getEnclosing();
    
    return newScope;
}

Node* Parser::ifStatement() {
    advance(); // IF

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'if'.");
    Node* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after if condition.");

    Node* thenBranch = statement();
    Node* elseBranch = nullptr;
    if (match(TokenType::ELSE)) {
        elseBranch = statement();
    }

    return new IfNode(condition, thenBranch, elseBranch);
}

Node* Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return expr;
}