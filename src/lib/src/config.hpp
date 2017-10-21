#pragma once

#include <experimental/optional>
#include <string>
#include <system_error>
#include <vector>

struct config
{
    static std::vector<std::string> flags_to_ignore();
    static std::vector<std::string> compile_commands();
    static std::string              build_uri();
    static std::experimental::optional<std::error_code>
    builder(const std::string &             build_uri,
            const std::vector<std::string> &compile_commands,
            const std::vector<std::string> &flags_to_ignore);
};
