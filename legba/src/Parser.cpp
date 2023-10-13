#include "Parser.h"

#include <format>
#include <algorithm>

#include "Error.h"

Parser::Parser()
    : hadError(false), current(0), tokens(), unresolvedFunctionCalls(), rootScope(nullptr), curScope(nullptr) {
}

bool Parser::parse(const std::vector<Token> &tokens) {
    this->tokens = tokens;
    hadError = false;
    rootScope = new ScopeNode();
    curScope = rootScope;
    
    while (!isAtEnd()) {
        try {
            curScope->addStatement(declaration());
        } catch (ParserError const& e) {
            synchronize();
        }
    }

    for (auto [scope, callee] : unresolvedFunctionCalls) {
        auto func = scope->getFunction(callee);
        if (func == nullptr) {
            std::cout << "Error: No function named '" << callee->getCallee() << "'." << std::endl;
        } else {
            callee->setFunction(func);
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
        e += " at '" + token->lexeme + '\'';
    }

    e += ": " + msg;

    std::cout << e << std::endl;
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
                curScope = rootScope;
                return;
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

ValueType Parser::valueType() {
    Token type = consume(TokenType::IDENTIFIER, "Expected type.");

    return ValueType::fromString(type.lexeme);
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

    return call();
}

Node* Parser::call() {
    auto expr = primary();

    if (match(TokenType::LEFT_PAREN) && expr->getType() == NodeType::IDENTIFIER) {
        expr = finishFunctionCall(expr);
    }

    for (;;) {
        if (match(TokenType::LEFT_PAREN)) {
            expr = finishCall(expr);
        } else if (match(TokenType::DOT)) {
            auto name = consume(TokenType::IDENTIFIER, "Expected attribute name after '.'.").lexeme;
            expr = new BinaryNode(TokenType::DOT, expr, new IdentifierNode(name));
        } else {
            break;
        }
    }

    return expr;
}

Node* Parser::finishCall(Node* callee) {

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

    FunctionCallNode* call = new FunctionCallNode(, args);

    unresolvedCalls.emplace_back(std::make_pair(curScope, call));

    return call;
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

    if (match(TokenType::INTEGER)) {
        return new IntegerNode(previous());
    }

    if (match(TokenType::DOUBLE)) {
        return new DoubleNode(previous());
    }

    if (match(TokenType::CHAR)) {
        return new CharNode(previous());
    }

    if (match(TokenType::IDENTIFIER)) {
        return new IdentifierNode(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN)) {
        Node* node = expression();
        consume(TokenType::RIGHT_PAREN, "Expected ')' after expression");
        return node;
    }

    advance();
    errorAt(&tokens[current - 1], "Malformed expression");
}

// Statements

Node* Parser::declaration() {
    uint16_t flags = qualifiers();
    switch (peek().type) {
        case TokenType::VAR: return varDeclaration(flags);
        case TokenType::FUNCTION: return funcDeclaration(flags);
        case TokenType::CLASS: return classDeclaration(flags);
    }
    if (flags != 0) {
        errorAtCurrent("Expected a variable, function or class declaration after qualifiers.");
    }

    return statement();
}

Node* Parser::varDeclaration(uint16_t flags) {
    advance(); // VAR

    checkQualifiers(flags, SymbolFlag::SF_IN_CLASS | SymbolFlag::SF_MUST_FN, "a variable");

    Token name = consume(TokenType::IDENTIFIER, "Expected variable name.");

    Node* initializer = nullptr;
    if (match(TokenType::EQUAL)) {
        initializer = expression();
    }

    consume(TokenType::SEMICOLON, "Expected ';' after variable declaration.");

    auto var = new VariableDeclarationNode(name.lexeme, flags, initializer);
    
    curScope->addVariable(name.lexeme, var);

    return var;
}

Node* Parser::funcDeclaration(uint16_t flags) {
    advance(); // FN

    if (curScope->getEnclosing() != nullptr) {
        error("Class declarations must be top level.", true);
    }

    checkQualifiers(flags, SymbolFlag::SF_IN_CLASS | SymbolFlag::SF_MUST_VAR, "a function");

    std::string name = consume(TokenType::IDENTIFIER, "Expected function name.").lexeme;

    consume(TokenType::LEFT_PAREN, "Expected '(' after function name.");
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

    ValueType resultType;
    if (match(TokenType::RIGHT_ARROW)) {
        resultType = valueType();
    } else {
        resultType.setType(ValueTypeEnum::VT_VOID);
    }

    if (!check(TokenType::LEFT_BRACE)) {
        error("Expected '{' before function body.");
    }
    Node* body = block();

    auto func = new FunctionNode(name, flags, params, body);
    func->setResultType(resultType);

    curScope->addFunction(name, func);

    return func;
}

Node* Parser::classDeclaration(uint16_t flags) {
    advance(); // CLASS

    if (curScope->getEnclosing() != nullptr) {
        error("Class declarations must be top level.", true);
    }

    std::string className = consume(TokenType::IDENTIFIER, "Expected class name.").lexeme;

    consume(TokenType::LEFT_BRACE, "Expected '{' after class name.");

    ClassNode* klass = new ClassNode(className);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        uint16_t flags = qualifiers();

        switch (peek().type) {
            case TokenType::VAR: {
                advance(); // VAR

                checkQualifiers(flags, SymbolFlag::SF_MUST_FN, "an attribute");

                std::string name = consume(TokenType::IDENTIFIER, "Expected attribute name.").lexeme;

                consume(TokenType::SEMICOLON, "Expected ';' after attribute declaration.");

                klass->addAttribute(name, new VariableDeclarationNode(name, flags, nullptr));

                break;
            }
            case TokenType::FUNCTION: {
                advance(); // FN

                checkQualifiers(flags, SymbolFlag::SF_MUST_VAR, "a method");

                std::string name = consume(TokenType::IDENTIFIER, "Expected method name.").lexeme;

                consume(TokenType::LEFT_PAREN, "Expected '(' after method name.");
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

                ValueType resultType;
                if (match(TokenType::RIGHT_ARROW)) {
                    resultType = valueType();
                } else {
                    resultType.setType(ValueTypeEnum::VT_VOID);
                }

                if (!check(TokenType::LEFT_BRACE)) {
                    error("Expected '{' before method body.");
                }
                Node* body = block();

                auto method = new MethodNode(name, flags, params, body, klass);
                method->setResultType(resultType);
                klass->addMethod(name, method);

                break;
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' to end class declaration.");

    return klass;
}

void Parser::checkQualifiers(uint16_t flags, uint16_t forbiddenFlags, std::string type) {
    if ((flags & forbiddenFlags) == 0) {
        return;
    }

    auto forbiddenQualifiers = qualifiersFromForbiddenFlags(forbiddenFlags, flags);
    auto e = std::string();
    for (auto& t : forbiddenQualifiers) {
        e += toLower(tokenTypeToString(t)) + ", ";
    }
    e.pop_back();
    e.pop_back();
    e += (forbiddenQualifiers.size() == 1) ? " is not a qualifier" : " are not qualifiers";
    e += " for " + type + '.';

    errorAtCurrent(e);
}

uint16_t Parser::qualifiers() {
    uint16_t flags = 0;
    SymbolFlag flag = tokenToSymbolFlag(peek().type);
    while (flag != SymbolFlag::SF_NONE) {
        flags |= flag;
        advance();
        flag = tokenToSymbolFlag(peek().type);
    }

    return flags;
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

    curScope = new ScopeNode(curScope);

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        curScope->addStatement(declaration());
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after block.");

    Node* newScope = curScope;
    
    curScope = curScope->getEnclosing();
    
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

    uint16_t flags = qualifiers();

    if (check(TokenType::VAR)) {
        initializer = varDeclaration(flags);
    } else {
        if (flags != 0) {
            errorAtCurrent("Expected variable declaration after qualifiers.");
        }

        if (match(TokenType::SEMICOLON)) {
            initializer = nullptr;
        } else {
            initializer = expressionStatement();
        }
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