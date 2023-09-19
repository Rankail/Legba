#include "Utils.h"

std::string toLower(std::string s) {
	std::string result = std::string(s.length(), ' ');
	std::transform(s.begin(), s.end(), result.begin(), [](unsigned char c) { return std::tolower(c); });
	return result;
}