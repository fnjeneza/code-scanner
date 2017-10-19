#include "code-scanner/ErrorCodes.hpp"

namespace {
struct ErrorCodesCategory : std::error_category
{
    const char *name() const noexcept override;
    std::string message(int ev) const noexcept override;
};

const char *ErrorCodesCategory::name() const noexcept
{
    return "lsp error codes";
}

std::string ErrorCodesCategory::message(int ev) const noexcept
{
    switch (static_cast<ErrorCodes>(ev))
    {
    case ErrorCodes::ParserError:
        return "argument can not be parsed";
    case ErrorCodes::InvalidRequest:
        return "Invalid request";
    case ErrorCodes::MethodNotFound:
        return "Method not found";
    case ErrorCodes::InvalidParams:
        return "Invalud parameters";
    case ErrorCodes::InternalError:
        return "Internal error";
    case ErrorCodes::serverErrorStart:
        return "Server can not start";
    case ErrorCodes::serverErrorEnd:
        return "Server can not end";
    case ErrorCodes::serverNotInitialized:
        return "Server is not initialized";
    case ErrorCodes::RequestCancelled:
        return "Request cancelled";
    case ErrorCodes::UnknownErrorCode:
    default:
        return "Unknown error code";
    }
}
}

const ErrorCodesCategory error_codes_category{};

std::error_code make_error_code(ErrorCodes e)
{
    return {static_cast<int>(e), error_codes_category};
}
