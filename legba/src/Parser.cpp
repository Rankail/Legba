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

void Parser::errorAtCurrent(const std::string &msg, bool noThrow) {
    errorAt(&tokens[current], msg, noThrow);
}

void Parser::error(const std::string &msg, bool noThrow) {
    errorAt(&tokens[current-1], msg, noThrow);
}

void Parser::errorAt(Token *token, const std::string &msg, bool noThrow) {
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

    if (!noThrow) {
        throw ParserError(e.c_str());
    }
}

void Parser::synchronize() {
    while (peek().type != TokenType::END_OF_FILE) {
        if (previous().type == TokenType::SEMICOLON) return;
        switch (peek().type) {
            case TokenType::CLASS:
            case TokenType::FUNCTION:
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
        return call();
    }

    if (match(TokenType::LEFT_PAREN)) {
        Node* node = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return node;
    }

    advance();
    errorAt(&tokens[current - 1], "Malformed expression");

}

Node* Parser::call() {
    auto name = previous().lexeme;

    if (!match(TokenType::LEFT_PAREN)) {
        return new VariableNode(name);
    }

    auto args = std::vector<Node*>();
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (args.size() >= 255) {
                errorAtCurrent("No more than 255 arguments are allowed.", true);
            }
            args.emplace_back(expression());
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expected ')' after arguments.");

    return new CallNode(name, args);
}

// Statements

Node* Parser::declaration() {
    switch (peek().type) {
        case TokenType::VAR: return varDeclaration();
        case TokenType::FUNCTION: return funcDeclaration("function");
        default: return statement();
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

Node* Parser::funcDeclaration(std::string kind) {
    advance(); // FN

    auto name = consume(TokenType::IDENTIFIER, "Expected " + kind + " name.").lexeme;

    consume(TokenType::LEFT_PAREN, "Expected '(' after " + kind + " name.");
    auto params = std::vector<std::string>();
    if (!check(TokenType::RIGHT_PAREN)) {
        do {
            if (params.size() >= 255) {
                errorAtCurrent("No more than 255 parameters are allowed.", true);
            }

            params.emplace_back(consume(TokenType::IDENTIFIER, "Expected parameter name").lexeme);
        } while (match(TokenType::COMMA));
    }

    consume(TokenType::RIGHT_PAREN, "Expected ')' after parameters.");

    if (!check(TokenType::LEFT_BRACE)) {
        error("Expected '{' before " + kind + " body.");
    }
    Node* body = block();

    return new FunctionNode(name, params, body);
}

Node* Parser::statement() {
    switch (peek().type) {
        case TokenType::LEFT_BRACE: return block();
        case TokenType::IF: return ifStatement();
        case TokenType::WHILE: return whileStatement();
        case TokenType::FOR: return forStatement();
        case TokenType::RETURN: return returnStatement();
        default: return expressionStatement();
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

Node* Parser::whileStatement() {
    advance(); // WHILE

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'while'.");
    Node* condition = expression();
    consume(TokenType::RIGHT_PAREN, "Expected ')' after while condition.");

    Node* body = statement();

    return new WhileNode(condition, body);
}

Node* Parser::forStatement() {
    advance(); // FOR

    consume(TokenType::LEFT_PAREN, "Expected '(' after 'for'.");
    Node* initializer;
    if (match(TokenType::SEMICOLON)) {
        initializer = nullptr;
    } else if (check(TokenType::VAR)) {
        initializer = varDeclaration();
    } else {
        initializer = expressionStatement();
    }

    Node* condition = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        condition = expression();
    }
    consume(TokenType::SEMICOLON, "Expected ';' after for loop condition");

    Node* increment = nullptr;
    if (!check(TokenType::RIGHT_PAREN)) {
        increment = expression();
    }
    consume(TokenType::RIGHT_PAREN, "Expected ')' aft for clause.");

    Node* body = statement();

    return new ForNode(initializer, condition, increment, body);
}

Node* Parser::returnStatement() {
    advance(); // RETURN

    Node* value = nullptr;
    if (!check(TokenType::SEMICOLON)) {
        value = expression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after return value.");
    return new UnaryNode(new OpNode(TokenType::RETURN), value);
}

Node* Parser::expressionStatement() {
    auto expr = expression();
    consume(TokenType::SEMICOLON, "Expected ';' after expression.");
    return expr;
}