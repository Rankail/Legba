#ifndef LEGBA_NODE_CLASS_H
#define LEGBA_NODE_CLASS_H

#include <vector>
#include <unordered_map>

#include "ASTNode/Node.h"
#include "Symbol.h"

class ClassNode;

class MethodNode : public Node {
public:
    MethodNode(const std::string& name, uint16_t flags, std::vector<std::string> params, Node* body, ClassNode* klass)
        : Node(NodeType::METHOD), name(name), flags(flags), params(std::move(params)), body(std::move(body)), klass(klass) {
    }

    std::string getName() const { return name; }
    uint16_t getFlags() const { return flags; }
    std::vector<std::string> getParams() const { return params; }
    Node* getBody() const { return body; }
    ClassNode* getClass() const { return klass; }

    virtual std::string toString() override;

private:
    std::string name;
    uint16_t flags;
    std::vector<std::string> params;
    Node* body;
    ClassNode* klass;
};

using MethodCallNode = CallNode<MethodNode>;

class ClassNode : public Node {
public:
    ClassNode(std::string name)
        : Node(NodeType::CLASS), name(name), methods(), attributes() {
    }

    std::string getName() const { return name; }
    std::unordered_map<std::string, MethodNode*> getMethods() const { return methods; }
    std::unordered_map<std::string, VariableDeclarationNode*> getAttributes() const { return attributes; }

    virtual std::string toString() override;

    void addAttribute(std::string name, VariableDeclarationNode* attribute);
    void addMethod(std::string name, MethodNode* method);

    VariableDeclarationNode* getAttribute(const std::string& name);
    MethodNode* getMethod(MethodCallNode* callee);

private:
    std::string name;
    std::unordered_map<std::string, MethodNode*> methods;
    std::unordered_map<std::string, VariableDeclarationNode*> attributes;
};

std::ostream& operator <<(std::ostream& os, Node* const& node);

#endif