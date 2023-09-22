#include "ValueType.h"

void ValueType::setType(ValueTypeEnum type) {
    this->type = type;
}

std::string ValueType::toString() const {

    return valueTypeEnumToString(type);
}

ValueType ValueType::fromString(const std::string& s) {
    if (s == "bool") {
        return ValueType(ValueTypeEnum::VT_BOOL);
    }
    if (s == "char") {
        return ValueType(ValueTypeEnum::VT_CHAR);
    }
    if (s == "void") {
        return ValueType(ValueTypeEnum::VT_VOID);
    }
    if (s == "int") {
        return ValueType(ValueTypeEnum::VT_INTEGER);
    }
    if (s == "double") {
        return ValueType(ValueTypeEnum::VT_DOUBLE);
    }
    if (s == "string") {
        return ValueType(ValueTypeEnum::VT_STRING);
    }

    return ValueType(ValueTypeEnum::VT_ERROR);
}

std::string valueTypeEnumToString(ValueTypeEnum type) {
    switch (type) {
        case ValueTypeEnum::VT_NONE: return "NONE";
        case ValueTypeEnum::VT_BOOL: return "BOOL";
        case ValueTypeEnum::VT_CHAR: return "CHAR";
        case ValueTypeEnum::VT_STRING: return "STRING";
        case ValueTypeEnum::VT_DOUBLE: return "DOUBLE";
        case ValueTypeEnum::VT_INTEGER: return "INT";
        case ValueTypeEnum::VT_VOID: return "VOID";
        case ValueTypeEnum::VT_OBJ: return "OBJ";
        case ValueTypeEnum::VT_ERROR: return "ERROR";
    }
    return "ERROR2";
}

std::ostream& operator<<(std::ostream& os, ValueType const& type) {
    os << type.toString();
    return os;
}
