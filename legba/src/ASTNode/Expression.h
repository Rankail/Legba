#ifndef LEGBA_NODE_EXPRESSION_H
#define LEGBA_NODE_EXPRESSION_H

#include "ASTNode/Node.h"

#include "Token.h"

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
        : Node(NodeType::UNARY), op(std::move(op)), node(std::move(node)) {
    }

    OpNode* getOp() const { return op; }
    Node* getNode() const { return node; }

    virtual std::string toString() override;

private:
    OpNode* op;
    Node* node;
};

class BinaryNode : public Node {
public:
    BinaryNode(TokenType opToken, Node* left, Node* right)
        : Node(NodeType::BINARY), op(new OpNode(opToken)), left(std::move(left)), right(std::move(right)) {
    }

    BinaryNode(OpNode* op, Node* left, Node* right)
        : Node(NodeType::BINARY), op(std::move(op)), left(std::move(left)), right(std::move(right)) {
    }

    OpNode* getOp() const { return op; }
    Node* getLeft() const { return left; }
    Node* getRight() const { return right; }

    virtual std::string toString() override;

private:
    OpNode* op;
    Node* left;
    Node* right;
};

#endif