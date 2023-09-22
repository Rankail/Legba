#pragma once

#include <stdexcept>

class ParserError : public std::runtime_error {
public:
	ParserError() = default;
	explicit ParserError(const char* msg) : std::runtime_error(msg) {}
    explicit ParserError(const std::string& arg) : std::runtime_error(arg) {}
};