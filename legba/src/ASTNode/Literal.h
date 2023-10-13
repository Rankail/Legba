#pragma once

#include "ASTNode/Node.h"

#include "Token.h"

class IntegerNode : public Node {
public:
    IntegerNode(Token token) : Node(NodeType::INTEGER, ValueType(ValueTypeEnum::VT_INTEGER)), value(std::stoi(token.lexeme)) {}

    int getValue() const { return value; }

    virtual std::string toString() override;

private:
    int value;
};

class DoubleNode : public Node {
public:
    DoubleNode(Token token) : Node(NodeType::DOUBLE, ValueType(ValueTypeEnum::VT_DOUBLE)), value(std::stod(token.lexeme)) {}

    double getValue() const { return value; }

    virtual std::string toString() override;

private:
    double value;
};

class StringNode : public Node {
public:
    StringNode(Token token) : Node(NodeType::STRING, ValueType(ValueTypeEnum::VT_STRING)), value(token.lexeme) {}

    std::string getValue() const { return value; }

    virtual std::string toString() override;

private:
    std::string value;
};

class CharNode : public Node {
public:
    CharNode(Token token) : Node(NodeType::CHAR, ValueType(ValueTypeEnum::VT_CHAR)), value(token.lexeme[0]) {}

    char getValue() const { return value; }

    virtual std::string toString() override;

private:
    char value;
};

class BoolNode : public Node {
public:
    BoolNode(bool boolean) : Node(NodeType::BOOL, ValueType(ValueTypeEnum::VT_BOOL)), value(boolean) {}

    bool getValue() const { return value; }

    virtual std::string toString() override;

private:
    bool value;
};