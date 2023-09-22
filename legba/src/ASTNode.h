#ifndef LEGBA_NODE_H
#define LEGBA_NODE_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#include "misc/Utils.h"
#include "Token.h"
#include "ValueType.h"

enum class NodeType {
    INTEGER, DOUBLE, STRING, CHAR, BOOL,
    VARIABLE, VARIABLE_DECL,
    OP, UNARY, BINARY,
    SCOPE, IF, WHILE, FOR,
    CALL, FUNCTION, CLASS
};

class Node {
public:
    Node(NodeType type, ValueType resultType = ValueType()) : type(type), resultType(resultType) {}
    virtual ~Node() = default;

    NodeType getType() const { return type; };
    virtual std::string toString() = 0;

    void setResultType(ValueType resultType);
    ValueType getResultType() const { return resultType; }

protected:
    ValueType resultType;
    NodeType type;
};

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

class VariableNode : public Node {
public:
    VariableNode(const std::string& name) : Node(NodeType::VARIABLE), name(name) {}

    std::string getName() const { return name; }

    virtual std::string toString() override;

private:
    std::string name;
};

class VariableDeclarationNode : public Node {
public:
    VariableDeclarationNode(const std::string& name, uint16_t flags, Node* initializer) :
        Node(NodeType::VARIABLE_DECL), name(name), flags(flags), initializer(initializer) {}

    std::string getName() const { return name; }
    uint16_t getFlags() const { return flags; }
    Node* getInitializer() const { return initializer; }

    virtual std::string toString() override;

private:
    std::string name;
    uint16_t flags;
    Node* initializer;
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

class ScopeNode : public Node {
public:
    ScopeNode(ScopeNode* enclosing = nullptr)
        : Node(NodeType::SCOPE, ValueType(ValueTypeEnum::VT_VOID)), enclosing(enclosing), statements() {
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
        : Node(NodeType::IF, ValueType(ValueTypeEnum::VT_VOID)), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {}

    Node* getCondition() const { return condition; }
    Node* getThenBranch() const { return thenBranch; }
    Node* getElseBranch() const { return elseBranch; }

    virtual std::string toString() override;

private:
    Node* condition;
    Node* thenBranch;
    Node* elseBranch;
};

class WhileNode : public Node {
public:
    WhileNode(Node* condition, Node* body)
        : Node(NodeType::WHILE, ValueType(ValueTypeEnum::VT_VOID)), condition(condition), body(body) {
    }

    Node* getCondition() const { return condition; }
    Node* getBody() const { return body; }

    virtual std::string toString() override;

private:
    Node* condition;
    Node* body;
};

class ForNode : public Node {
public:
    ForNode(Node* initializer, Node* condition, Node* increment, Node* body)
        : Node(NodeType::FOR, ValueType(ValueTypeEnum::VT_VOID)), initializer(initializer), condition(condition), increment(increment), body(body) {
    }

    Node* getInitializer() const { return initializer; }
    Node* getCondition() const { return condition; }
    Node* getIncrement() const { return increment; }
    Node* getBody() const { return body; }

    virtual std::string toString() override;

private:
    Node* initializer;
    Node* condition;
    Node* increment;
    Node* body;
};

class CallNode : public Node {
public:
    CallNode(const std::string& callee, std::vector<Node*> args)
        : Node(NodeType::CALL), callee(callee), args(std::move(args)) {
    }

    std::string getCallee() const { return callee; }
    std::vector<Node*> getArgs() const { return args; }

    virtual std::string toString() override;

private:
    std::string callee;
    std::vector<Node*> args;
};

enum SymbolFlag : uint16_t {
    SF_NONE =       0,
    SF_MUST_FN =    BIT(0),
    SF_MUST_VAR =   BIT(1),
    SF_IN_CLASS =   BIT(2),

    SF_CONST =      BIT(7) | BIT(1),
    SF_PUBLIC =     BIT(8) | BIT(2),
    SF_PROTECTED =  BIT(9) | BIT(2),
    SF_STATIC =     BIT(10) | BIT(2),
    SF_VIRTUAL =    BIT(11) | BIT(0) | BIT(2)
};

SymbolFlag tokenToSymbolFlag(TokenType token);
std::string symbolFlagsToString(uint16_t flags);
std::vector<TokenType> qualifiersFromForbiddenFlags(uint16_t x, uint16_t flags);

class FunctionNode : public Node {
public:
    FunctionNode(const std::string& name, uint16_t flags, std::vector<std::string> params, Node* body)
        : Node(NodeType::FUNCTION), name(name), flags(flags), params(std::move(params)), body(std::move(body)) {
    }

    std::string getName() const { return name; }
    uint16_t getFlags() const { return flags; }
    std::vector<std::string> getParams() const { return params; }
    Node* getBody() const { return body; }

    virtual std::string toString() override;

private:
    std::string name;
    uint16_t flags;
    std::vector<std::string> params;
    Node* body;
};

class ClassNode : public Node {
public:
    ClassNode(std::string name, std::unordered_map<std::string, Node*> internals)
        : Node(NodeType::CLASS), name(name), internals(std::move(internals)) { }

    std::string getName() const { return name; }
    std::unordered_map<std::string, Node*> getInternals() const { return internals; }

    virtual std::string toString() override;

private:
    std::string name;
    std::unordered_map<std::string, Node*> internals;
};

std::ostream& operator <<(std::ostream& os, Node* const& node);


#endif //LEGBA_NODE_H
