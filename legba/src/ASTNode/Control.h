#ifndef LEGBA_NODE_CONTROL_H
#define LEGBA_NODE_CONTROL_H

#include "ASTNode/Node.h"
#include "ASTNode/Symbol.h"

#include <vector>
#include <unordered_map>

class ScopeNode : public Node {
public:
    ScopeNode(ScopeNode* enclosing = nullptr)
        : Node(NodeType::SCOPE, ValueType(ValueTypeEnum::VT_VOID)), enclosing(enclosing), statements() {
    }

    ScopeNode* getEnclosing() const { return enclosing; }
    std::vector<Node*> getStatements() const { return statements; }

    void addStatement(Node* node);
    void addVariable(std::string name, VariableDeclarationNode* var);
    void addFunction(std::string name, FunctionNode* func);

    VariableDeclarationNode* getVariable(const std::string& name);
    FunctionNode* getFunction(FunctionCallNode* callee);

    virtual std::string toString() override;

private:
    ScopeNode* enclosing;
    std::vector<Node*> statements;
    std::unordered_map<std::string, VariableDeclarationNode*> variables;
    std::unordered_map<std::string, FunctionNode*> functions;
};

class IfNode : public Node {
public:
    IfNode(Node* condition, Node* thenBranch, Node* elseBranch)
        : Node(NodeType::IF, ValueType(ValueTypeEnum::VT_VOID)), condition(condition), thenBranch(thenBranch), elseBranch(elseBranch) {
    }

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

#endif