#ifndef LEGBA_NODE_H
#define LEGBA_NODE_H

#include <string>
#include <vector>
#include <memory>

#include "Token.h"

enum class NodeType {
    NUMBER, STRING, BOOL, VARIABLE, OP, UNARY, BINARY, CALL, FUNCTION, SCOPE, IF
};

class Node {
public:
    Node(NodeType type) : type(type) {}
    virtual ~Node() = default;

    virtual NodeType getType() const { return type; };
    virtual std::string toString() = 0;

protected:
    NodeType type;
};

class NumberNode : public Node {
public:
    NumberNode(Token token) : Node(NodeType::NUMBER), value(std::stod(token.lexeme)) {}

    double getValue() const { return value; }

    virtual std::string toString() override;

private:
    double value;
};

class StringNode : public Node {
public:
    StringNode(Token token) : Node(NodeType::STRING), value(token.lexeme) {}

    std::string getValue() const { return value; }

    virtual std::string toString() override;

private:
    std::string value;
};

class BoolNode : public Node {
public:
    BoolNode(bool boolean) : Node(NodeType::BOOL), value(boolean) {}

    bool getValue() const { return value; }

    virtual std::string toString() override;

private:
    bool value;
};

class VariableNode : public Node {
public:
    VariableNode(const std::string& name) : Node(NodeType::VARIABLE), name(name) {}

    std::string getName() const { return name; }

    virtual std::string toString() override;

private:
    std::string name;
};

class OpNode : public Node {
public:
    OpNode(TokenType op) : Node(NodeType::OP), op(op) {}


    TokenType getOp() const { return op; }

    virtual std::string toString() override;

private:
    TokenType op;
};

class UnaryNode : public Node {
public:
    UnaryNode(OpNode* op, Node* node)
            : Node(NodeType::UNARY), op(std::move(op)), node(std::move(node)) {}

    OpNode* getOp() const { return op; }
    Node* getNode() const { return node; }

    virtual std::string toString() override;

private:
    OpNode* op;
    Node* node;
};

class BinaryNode : public Node {
public:
    BinaryNode(OpNode* op, Node* left, Node* right)
        : Node(NodeType::BINARY), op(std::move(op)), left(std::move(left)), right(std::move(right)) {}

    OpNode* getOp() const { return op; }
    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }

    virtual std::string toString() override;

private:
    OpNode* op;
    Node* left;
    Node* right;
};

class CallNode : public Node {
public:
    CallNode(const std::string& callee, std::vector<Node*> args)
        : Node(NodeType::CALL), callee(callee), args(std::move(args)) {}

    std::string getCallee() const { return callee; }
    std::vector<Node*> getArgs() const { return args; }

    virtual std::string toString() override;

private:
    std::string callee;
    std::vector<Node*> args;
};

class FunctionNode : public Node {
public:
    FunctionNode(const std::string& name, std::vector<std::string> args, Node* body)
        : Node(NodeType::FUNCTION), name(name), args(std::move(args)), body(std::move(body)) {}

    std::string getName() const { return name; }
    std::vector<std::string> getArgs() const { return args; }
    Node* getBody() const { return body; }

    virtual std::string toString() override;

private:
    std::string name;
    std::vector<std::string> args;
    Node* body;
};

class ScopeNode : public Node {
public:
    ScopeNode(ScopeNode* enclosing = nullptr)
        : Node(NodeType::SCOPE), enclosing(enclosing), statements() {
    }

    ScopeNode* getEnclosing() const { return enclosing; }
    std::vector<Node*> getStatements() const { return statements; }

    void addStatement(Node* node);

    virtual std::string toString() override;

private:
    ScopeNode* enclosing;
    std::vector<Node*> statements;
};

class IfNode : public Node {
public:
    IfNode(Node* condition, Node* thenBranch, Node* elseBranch)
        : Node(NodeType::IF), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    Node* getCondition() const { return condition; }
    Node* getThenBranch() const { return thenBranch; }
    Node* getElseBranch() const { return elseBranch; }

    virtual std::string toString() override;

private:
    Node* condition;
    Node* thenBranch;
    Node* elseBranch;
};

std::ostream& operator <<(std::ostream& os, NumberNode* const& node);
std::ostream& operator <<(std::ostream& os, StringNode* const& node);
std::ostream& operator <<(std::ostream& os, BoolNode* const& node);
std::ostream& operator <<(std::ostream& os, VariableNode* const& node);
std::ostream& operator <<(std::ostream& os, OpNode* const& node);
std::ostream& operator <<(std::ostream& os, UnaryNode* const& node);
std::ostream& operator <<(std::ostream& os, BinaryNode* const& node);
std::ostream& operator <<(std::ostream& os, CallNode* const& node);
std::ostream& operator <<(std::ostream& os, FunctionNode* const& node);
std::ostream& operator <<(std::ostream& os, ScopeNode* const& node);
std::ostream& operator <<(std::ostream& os, IfNode* const& node);


#endif //LEGBA_NODE_H
