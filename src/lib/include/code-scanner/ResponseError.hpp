#pragma once

#include <string>

namespace code {
namespace analyzer {

struct ResponseError
{
    std::size_t code = 0;
    std::string message;
};
} // namespace analyzer
} // namespace code
