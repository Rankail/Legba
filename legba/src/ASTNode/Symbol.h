#ifndef LEGBA_NODE_SYMBOL_H
#define LEGBA_NODE_SYMBOL_H

#include "ASTNode/Node.h"
#include "Token.h"
#include "misc/Utils.h"

enum SymbolFlag : uint16_t {
    SF_NONE = 0,
    SF_MUST_FN = BIT(0),
    SF_MUST_VAR = BIT(1),
    SF_IN_CLASS = BIT(2),

    SF_CONST = BIT(7) | BIT(1),
    SF_PUBLIC = BIT(8) | BIT(2),
    SF_PROTECTED = BIT(9) | BIT(2),
    SF_STATIC = BIT(10) | BIT(2),
    SF_VIRTUAL = BIT(11) | BIT(0) | BIT(2)
};

SymbolFlag tokenToSymbolFlag(TokenType token);
std::string symbolFlagsToString(uint16_t flags);
std::vector<TokenType> qualifiersFromForbiddenFlags(uint16_t x, uint16_t flags);

class VariableDeclarationNode : public Node {
public:
    VariableDeclarationNode(const std::string& name, uint16_t flags, Node* initializer) :
        Node(NodeType::VARIABLE_DECL), name(name), flags(flags), initializer(initializer) {
    }

    std::string getName() const { return name; }
    uint16_t getFlags() const { return flags; }
    Node* getInitializer() const { return initializer; }

    virtual std::string toString() override;

private:
    std::string name;
    uint16_t flags;
    Node* initializer;
};

class VariableNode : public Node {
public:
    VariableNode(VariableDeclarationNode* var) : Node(NodeType::VARIABLE), var(var) {}

    VariableDeclarationNode* getVar() const { return var; }

    virtual std::string toString() override;

private:
    VariableDeclarationNode* var;
};

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

template<typename T>
class CallNode : public Node {
public:
    CallNode(std::vector<Node*> args, T* func = nullptr)
        : Node(NodeType::CALL), args(std::move(args)), func(func) {
    }

    std::vector<Node*> getArgs() const { return args; }

    T* getFunction() const { return func; }
    void setFunction(T* func);

    virtual std::string toString() override;

private:
    std::vector<Node*> args;
    T* func;
};

using FunctionCallNode = CallNode<FunctionNode>;

#endif