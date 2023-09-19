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
    flags &= 0b1111111111111000;

    std::string result = "";
    if ((flags & SymbolFlag::SF_MUST_FN) != 0) {
        result += "MUST_FN ";
    }
    if ((flags & SymbolFlag::SF_MUST_VAR) != 0) {
        result += "MUST_VAR ";
    }
    if ((flags & SymbolFlag::SF_IN_CLASS) != 0) {
        result += "IN_CLASS ";
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

std::vector<TokenType> qualifiersFromForbiddenFlags(uint16_t forbidden, uint16_t flags) {
    std::vector<TokenType> found;
    if ((forbidden & SymbolFlag::SF_CONST & flags) != 0) {
        found.emplace_back(TokenType::CONST);
    }
    if ((forbidden & SymbolFlag::SF_PUBLIC & flags) != 0) {
        found.emplace_back(TokenType::PUBLIC);
    }
    if ((forbidden & SymbolFlag::SF_PROTECTED & flags) != 0) {
        found.emplace_back(TokenType::PROTECTED);
    }
    if ((forbidden & SymbolFlag::SF_STATIC & flags) != 0) {
        found.emplace_back(TokenType::STATIC);
    }
    if ((forbidden & SymbolFlag::SF_VIRTUAL & flags) != 0) {
        found.emplace_back(TokenType::VIRTUAL);
    }
    return found;
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
    os << "VariableDeclarationNode(\n" << node->getName() << '\n';
    if (node->getFlags() != 0) {
        os << "flags: " << symbolFlagsToString(node->getFlags()) << '\n';
    }
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
    os << "FunctionNode(" << node->getName() << ' ';
    if (node->getFlags() != 0) {
        os << "flags: " << symbolFlagsToString(node->getFlags()) << '\n';
    }
    os << " ( ";
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

std::ostream& operator<<(std::ostream& os, ClassNode* const& node) {
    os << "ClassNode(name: " << node->getName() << '\n';
    for (auto const& [_, in] : node->getInternals()) {
        os << in->toString() << '\n';
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

std::string WhileNode::toString() {
    return (std::stringstream() << this).str();
}

std::string ForNode::toString() {
    return (std::stringstream() << this).str();
}

std::string VariableDeclarationNode::toString() {
    return (std::stringstream() << this).str();
}

std::string ClassNode::toString() {
    return (std::stringstream() << this).str();
}
