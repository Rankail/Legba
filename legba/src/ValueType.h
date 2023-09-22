#pragma once

#include <string>
#include <sstream>

enum class ValueTypeEnum {
	VT_NONE, VT_ERROR, VT_VOID, VT_INTEGER, VT_DOUBLE, VT_STRING, VT_CHAR, VT_BOOL, VT_OBJ
};


class ValueType {
public:
	ValueType() : type(ValueTypeEnum::VT_NONE), name() {}

	ValueType(ValueTypeEnum type, std::string name = "")
		: type(type), name(name) { }

	ValueTypeEnum getType() const { return type; }
	std::string getName() const { return name; }

	void setType(ValueTypeEnum type);

	std::string toString() const;

public:
	static ValueType fromString(const std::string& s);

private:
	ValueTypeEnum type;
	std::string name;
};

std::string valueTypeEnumToString(ValueTypeEnum type);

std::ostream& operator <<(std::ostream& os, ValueType const& type);

