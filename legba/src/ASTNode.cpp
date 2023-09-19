#include "ASTNode.h"

#include <sstream>

SymbolFlag tokenToSymbolFlag(TokenType token) {
    switch (token) {
        case TokenType::CONST: return SymbolFlag::SF_CONST;
        case TokenType::PUBLIC: return SymbolFlag::SF_PUBLIC;
        case TokenType::PROTECTED: return SymbolFlag::SF_PROTECTED;
        case TokenType::STATIC: return SymbolFlag::SF_STATIC;
        case TokenType::VIRTUAL: return SymbolFlag::SF_VIRTUAL;
    }
    return SymbolFlag::SF_NONE;
}

std::string symbolFlagsToString(uint16_t flags) {
    std::string result = "";
    if ((flags & SymbolFlag::SF_NOT_FN) != 0) {
        result += "NOT_FN ";
    }
    if ((flags & SymbolFlag::SF_NOT_VAR) != 0) {
        result += "NOT_VAR ";
    }
    if ((flags & SymbolFlag::SF_IS_CLASS) != 0) {
        result += "IS_CLASS ";
    }
    if ((flags & SymbolFlag::SF_CONST) != 0) {
        result += "CONST ";
    }
    if ((flags & SymbolFlag::SF_PROTECTED) != 0) {
        result += "PROTECTED ";
    }
    if ((flags & SymbolFlag::SF_PUBLIC) != 0) {
        result += "PUBLIC ";
    }
    if ((flags & SymbolFlag::SF_STATIC) != 0) {
        result += "STATIC ";
    }
    if ((flags & SymbolFlag::SF_VIRTUAL) != 0) {
        result += "VIRTUAL ";
    }
    
    if (!result.empty()) {
        result.pop_back();
    }

    return result;
}

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

std::ostream& operator <<(std::ostream& os, VariableDeclarationNode* const& node) {
    os << "VariableDeclarationNode(\n" << node->getName() << '\n'
        << "flags: " << symbolFlagsToString(node->getFlags()) << '\n';
    if (node->getInitializer() != nullptr) {
        os << "init: " << node->getInitializer()->toString() << '\n';
    }
    os << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, OpNode* const& node) {
    os << "OpNode(" << tokenTypeToString(node->getOp()) << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, UnaryNode* const& node) {
    os << "UnaryNode(" << node->getOp()->toString() << ' '
        << (node->getNode() == nullptr ? "NULL" : node->getNode()->toString()) << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, BinaryNode* const& node) {
    os << "BinaryNode(" << node->getLeft()->toString() << ' ' << node->getOp()->toString() << ' ' << node->getRight()->toString() << ')';
    return os;
}

std::ostream& operator <<(std::ostream& os, CallNode* const& node) {
    os << "CallNode('" << node->getCallee() << "' { ";
    for (auto const& arg : node->getArgs()) {
        os << arg->toString() << ' ';
    }
    os << "})";
    return os;
}

std::ostream& operator <<(std::ostream& os, FunctionNode* const& node) {
    os << "FunctionNode(" << node->getName() << "( ";
    for (auto const& param : node->getParams()) {
        os << param << ' ';
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
    os << "IfNode(\ncond: " << node->getCondition()->toString()
        << "\nthen: " << node->getThenBranch()->toString();
    if (node->getElseBranch() != nullptr) {
        os << "\nelse: " << node->getElseBranch()->toString();
    }
    os << "\n)";
    return os;
}

std::ostream& operator<<(std::ostream& os, WhileNode* const& node) {
    os << "WhileNode(\ncond: " << node->getCondition()->toString()
        << "\nbody: " << node->getBody()->toString() << "\n)";
    return os;
}

std::ostream& operator<<(std::ostream& os, ForNode* const& node) {
    os << "ForNode(\n";
    if (node->getInitializer() != nullptr) {
        os << "init: " << node->getInitializer()->toString() << '\n';
    }
    if (node->getCondition() != nullptr) {
        os << "cond: " << node->getCondition()->toString() << '\n';
    }
    if (node->getIncrement() != nullptr) {
        os << "inc: " << node->getIncrement()->toString() << '\n';
    }
    os << "body: " << node->getBody()->toString() << '\n';
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

std::string WhileNode::toString() {
    return (std::stringstream() << this).str();
}

std::string ForNode::toString() {
    return (std::stringstream() << this).str();
}

std::string VariableDeclarationNode::toString() {
    return (std::stringstream() << this).str();
}
