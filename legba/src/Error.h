#pragma once

#include <exception>

class ParserError : public std::exception {
public:
	ParserError() = default;
	ParserError(const char* msg) : std::exception(msg) {}
};