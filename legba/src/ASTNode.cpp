#include "ASTNode.h"

#include <sstream>

std::ostream& operator <<(std::ostream& os, NumberNode* const& node) {
    os << "NumberNode(" << node->getValue() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, StringNode* const& node) {
    os << "StringNode(" << node->getValue() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, BoolNode* const& node) {
    os << "BoolNode(" << node->getValue() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, VariableNode* const& node) {
    os << "VariableNode(" << node->getName() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, OpNode* const& node) {
    os << "OpNode(" << tokenTypeToString(node->getOp()) << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, UnaryNode* const& node) {
    os << "UnaryNode(" << node->getOp()->toString() << ' ' << node->getNode()->toString() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, BinaryNode* const& node) {
    os << "BinaryNode(" << node->getLeft()->toString() << ' ' << node->getOp()->toString() << ' ' << node->getRight()->toString() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, CallNode* const& node) {
    os << "CallNode(" << node->getCallee() << ": { ";
    for (auto const& arg : node->getArgs()) {
        os << arg->toString() << ' ';
    }
    os << "})";
    return os;
}

std::ostream& operator <<(std::ostream& os, FunctionNode* const& node) {
    os << "FunctionNode(" << node->getName() << "( ";
    for (auto const& arg : node->getArgs()) {
        os << arg << ' ';
    };
    os << ") " << node->getBody()->toString() << ")";
    return os;
}

std::ostream& operator <<(std::ostream& os, ScopeNode* const& node) {
    os << "ScopeNode(\n";
    for (auto const& stmt : node->getStatements()) {
        os << stmt->toString() << '\n';
    }
    os << "})";
    return os;
}

std::ostream& operator<<(std::ostream& os, IfNode* const& node) {
    os << "IfNode(cond: " << node->getCondition()->toString()
        << " then: " << node->getThenBranch()->toString();
    if (node->getElseBranch() != nullptr) {
        os << " else: " << node->getElseBranch()->toString();
    }
    os << ')';
    return os;
}

std::string NumberNode::toString() {
    return (std::stringstream() << this).str();
}

std::string StringNode::toString() {
    return (std::stringstream() << this).str();
}

std::string BoolNode::toString() {
    return (std::stringstream() << this).str();
}

std::string VariableNode::toString() {
    return (std::stringstream() << this).str();
}

std::string OpNode::toString() {
    return (std::stringstream() << this).str();
}

std::string UnaryNode::toString() {
    return (std::stringstream() << this).str();
}

std::string BinaryNode::toString() {
    return (std::stringstream() << this).str();
}

std::string CallNode::toString() {
    return (std::stringstream() << this).str();
}

std::string FunctionNode::toString() {
    return (std::stringstream() << this).str();
}

void ScopeNode::addStatement(Node* node) {
    statements.emplace_back(node);
}

std::string ScopeNode::toString() {
    return (std::stringstream() << this).str();
}

std::string IfNode::toString() {
    return (std::stringstream() << this).str();
}
