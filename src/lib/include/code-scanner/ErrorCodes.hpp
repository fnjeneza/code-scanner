#pragma once

#include <system_error>

enum class ErrorCodes
{
    ParserError          = -32700,
    InvalidRequest       = -32600,
    MethodNotFound       = -32601,
    InvalidParams        = -32602,
    InternalError        = -32603,
    serverErrorStart     = -32099,
    serverErrorEnd       = -32000,
    serverNotInitialized = -32002,
    UnknownErrorCode     = -32001,
    RequestCancelled     = -32800,
};

// Give to the user the ability to pass a custom message
std::error_code error(const std::string & message);

namespace std {
template <>
struct is_error_code_enum<ErrorCodes> : true_type
{
};
}
std::error_code make_error_code(ErrorCodes);

