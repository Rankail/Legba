#include "ASTNode.h"

#include <sstream>
#include "Error.h"

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
    if (FLAG_IN_FLAGS_AND_FORBIDDEN(SymbolFlag::SF_CONST, flags, forbidden)) {
        found.emplace_back(TokenType::CONST);
    }
    if (FLAG_IN_FLAGS_AND_FORBIDDEN(SymbolFlag::SF_PUBLIC, flags, forbidden)) {
        found.emplace_back(TokenType::PUBLIC);
    }
    if (FLAG_IN_FLAGS_AND_FORBIDDEN(SymbolFlag::SF_PROTECTED, flags, forbidden)) {
        found.emplace_back(TokenType::PROTECTED);
    }
    if (FLAG_IN_FLAGS_AND_FORBIDDEN(SymbolFlag::SF_STATIC, flags, forbidden)) {
        found.emplace_back(TokenType::STATIC);
    }
    if (FLAG_IN_FLAGS_AND_FORBIDDEN(SymbolFlag::SF_VIRTUAL, flags, forbidden)) {
        found.emplace_back(TokenType::VIRTUAL);
    }
    return found;
}

std::string IntegerNode::toString() {
	std::stringstream os;
    os << "IntegerNode(" << getValue() << ')';
    return os.str();
}

std::string DoubleNode::toString() {
	std::stringstream os;
    os << "DoubleNode(" << getValue() << ')';
    return os.str();
}

std::string StringNode::toString() {
	std::stringstream os;
    os << "StringNode(\"" << getValue() << "\")";
    return os.str();
}

std::string CharNode::toString() {
	std::stringstream os;
    os << "CharNode('" << getValue() << "')";
    return os.str();
}

std::string BoolNode::toString() {
	std::stringstream os;
    os << "BoolNode(" << getValue() << ')';
    return os.str();
}

std::string VariableNode::toString() {
	std::stringstream os;
    os << "VariableNode(" << getVar() << ')';
    return os.str();
}

std::string VariableDeclarationNode::toString() {
	std::stringstream os;
    os << "VariableDeclarationNode(\n" << getName() << '\n';
    if (getFlags() != 0) {
        os << "flags: " << symbolFlagsToString(getFlags()) << '\n';
    }
    if (getInitializer() != nullptr) {
        os << "init: " << getInitializer() << '\n';
    }
    os << ')';
    return os.str();
}

std::string OpNode::toString() {
	std::stringstream os;
    os << "OpNode(" << tokenTypeToString(getOp()) << ')';
    return os.str();
}

std::string UnaryNode::toString() {
	std::stringstream os;
    os << "UnaryNode(" << getOp() << ' '
        << (getNode() == nullptr ? "NULL" : getNode()->toString()) << ')';
    return os.str();
}

std::string BinaryNode::toString() {
	std::stringstream os;
    os << "BinaryNode(" << getLeft() << ' ' << getOp() << ' ' << getRight() << ')';
    return os.str();
}

void CallNode::setFunction(FunctionNode* func) {
    this->func = func;
}

std::string CallNode::toString() {
	std::stringstream os;
    os << "CallNode('" << getCallee() << "' { ";
    for (auto const& arg : getArgs()) {
        os << arg << ' ';
    }
    os << "})";
    return os.str();
}

std::string FunctionNode::toString() {
	std::stringstream os;
    os << "FunctionNode(" << getName() << ' ';
    if (getFlags() != 0) {
        os << "flags: " << symbolFlagsToString(getFlags()) << '\n';
    }
    os << " ( ";
    for (auto const& param : getParams()) {
        os << param << ' ';
    };
    os << ") " << getBody() << ")";
    return os.str();
}

std::string ScopeNode::toString() {
	std::stringstream os;
    os << "ScopeNode(\n";
    for (auto const& stmt : getStatements()) {
        os << stmt << '\n';
    }
    os << "})";
    return os.str();
}

std::string IfNode::toString() {
	std::stringstream os;
    os << "IfNode(\ncond: " << getCondition()
        << "\nthen: " << getThenBranch();
    if (getElseBranch() != nullptr) {
        os << "\nelse: " << getElseBranch();
    }
    os << "\n)";
    return os.str();
}

std::string WhileNode::toString() {
	std::stringstream os;
    os << "WhileNode(\ncond: " << getCondition()
        << "\nbody: " << getBody() << "\n)";
    return os.str();
}

std::string ForNode::toString() {
	std::stringstream os;
    os << "ForNode(\n";
    if (getInitializer() != nullptr) {
        os << "init: " << getInitializer() << '\n';
    }
    if (getCondition() != nullptr) {
        os << "cond: " << getCondition() << '\n';
    }
    if (getIncrement() != nullptr) {
        os << "inc: " << getIncrement() << '\n';
    }
    os << "body: " << getBody() << '\n';
    os << ')';
    return os.str();
}

std::string ClassNode::toString() {
    std::stringstream os;
    os << "ClassNode(name: " << getName() << '\n';
    for (auto const& [_, method] : getMethods()) {
        os << method << '\n';
    }
    for (auto const& [_, attribute] : getAttributes()) {
        os << attribute << '\n';
    }
    os << ')';
    return os.str();
}

void ClassNode::addAttribute(std::string name, VariableDeclarationNode* attribute) {
    if (methods.contains(name)) {
        throw ParserError("There already exists a method named '" + name + "' in this class.");
    } else if (attributes.contains(name)) {
        throw ParserError("There already exists a attribute named '" + name + "' in this class.");
    }
    attributes.emplace(name, attribute);
}

void ClassNode::addMethod(std::string name, MethodNode* method) {
    if (attributes.contains(name)) {
        throw ParserError("There already exists a attribute named '" + name + "' in this class.");
    } else if (methods.contains(name)) {
        throw ParserError("There already exists a method named '" + name + "' in this class.");
    }
    methods.emplace(name, method);
}

VariableDeclarationNode* ClassNode::getAttribute(const std::string& name) {
    auto it = attributes.find(name);
    if (it != attributes.end()) {
        return it->second;
    }

    return nullptr;
}

MethodNode* ClassNode::getMethod(CallNode* callee) {
    auto it = methods.find(callee->getCallee());
    if (it != methods.end()) {
        return it->second;
    }

    return nullptr;
}

void ScopeNode::addStatement(Node* node) {
    statements.emplace_back(node);
}

void ScopeNode::addVariable(std::string name, VariableDeclarationNode* var) {
    if (functions.contains(name)) {
        throw ParserError("There already exists a function named '" + name + "' in this scope.");
    }
    variables.emplace(name, var); // allows shadowing; varibales must be resolved instantly!
}

void ScopeNode::addFunction(std::string name, FunctionNode* func) {
    if (variables.contains(name)) {
        throw ParserError("There already exists a variable named '" + name + "' in this scope.");
    } else if (functions.contains(name)) {
        throw ParserError("There already exists a function named '" + name + "' in this scope.");
    }
    functions.emplace(name, func);
}

VariableDeclarationNode* ScopeNode::getVariable(const std::string& name) {
    auto it = variables.find(name);
    if (it != variables.end()) {
        return it->second;
    }
    
    if (enclosing == nullptr) {
        return nullptr;
    }

    return enclosing->getVariable(name);
}

FunctionNode* ScopeNode::getFunction(CallNode* callee) { // functions should be accessible even if declared after the call -> cache unresolved calls
    auto it = functions.find(callee->getCallee()->);
    if (it != functions.end()) {
        return it->second;
    }

    if (enclosing == nullptr) {
        return nullptr;
    }

    return enclosing->getFunction(callee);
}

void Node::setResultType(ValueType resultType) {
    this->resultType = resultType;
}

std::ostream& operator <<(std::ostream& os, Node* const& node) {
    os << node->toString() << " -> " << node->getResultType().toString();
    return os;
}

std::string MethodNode::toString() {
    std::stringstream os;
    os << "MethodNode(" << klass->getName() << (FLAG_IN_FLAGS(SymbolFlag::SF_STATIC, getFlags()) ? "::" : ".") << getName() << ' ';
    if (getFlags() != 0) {
        os << "flags: " << symbolFlagsToString(getFlags()) << '\n';
    }
    os << " ( ";
    for (auto const& param : getParams()) {
        os << param << ' ';
    };
    os << ") " << getBody() << ")";
    return os.str();
}

std::string IdentifierNode::toString() {
    std::stringstream os;
    os << "IdentifierNode(" << getName() << ')';
    return os.str();
}
