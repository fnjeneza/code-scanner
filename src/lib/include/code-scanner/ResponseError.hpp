#pragma once

#include <string>

struct ResponseError
{
    std::size_t code = 0;
    std::string message;
};
