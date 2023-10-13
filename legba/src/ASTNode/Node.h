#ifndef LEGBA_NODE_H
#define LEGBA_NODE_H
#include "ValueType.h"

enum class NodeType {
    INTEGER, DOUBLE, STRING, CHAR, BOOL,
    VARIABLE, VARIABLE_DECL, IDENTIFIER,
    OP, UNARY, BINARY,
    SCOPE, IF, WHILE, FOR,
    CALL, FUNCTION, CLASS, METHOD
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

#endif