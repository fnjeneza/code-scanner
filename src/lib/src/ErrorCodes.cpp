#include "code-scanner/ErrorCodes.hpp"

namespace {
struct ErrorCodesCategory : std::error_category
{
    const char *name() const noexcept override;
    std::string message(int ev) const noexcept override;
    // contains user custom message or details
    mutable std::string details;
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
        return "Argument can not be parsed";
    case ErrorCodes::InvalidRequest:
        return "Invalid request";
    case ErrorCodes::MethodNotFound:
        return "Method not found";
    case ErrorCodes::InvalidParams:
        return "Invalid parameters";
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
        if(!details.empty())
        {
            auto __details = details;
            // clear details to avoid eventual misusage
            details.clear();
          return __details;
        }
        return "Unknown error code";
    }
}
}

ErrorCodesCategory error_codes_category{};

std::error_code make_error_code(ErrorCodes e)
{
    return {static_cast<int>(e), error_codes_category};
}

std::error_code error(const std::string & message)
{
    error_codes_category.details = message;
    // create and copy the error code
    auto ec = make_error_code(ErrorCodes::UnknownErrorCode);
    return ec;
}
