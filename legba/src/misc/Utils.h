#pragma once

#include <algorithm>
#include <string>

#define BIT(x) (1 << x)

#define FLAG_IN_FLAGS(flag, flags) ((flag & flags) == flag)
#define FLAG_IN_FLAGS_AND_FORBIDDEN(flag, flags, forbidden) (FLAG_IN_FLAGS(flag, flags) && ((flag & forbidden) != 0))

std::string toLower(std::string s);