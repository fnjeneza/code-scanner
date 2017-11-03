#pragma once

#include <string>
#include <vector>

namespace code::analyzer {
using command_t = std::vector<std::string>;
struct compile_command
{
    compile_command(const std::string_view &file);

    compile_command(const std::string_view &directory,
                    const std::string_view &command,
                    const std::string_view &file);

    ~compile_command() = default;

    std::string m_file;
    // compile directory of the file
    std::string m_directory;
    // compile comand arguments
    command_t m_command;
};

} // namespace code::analyzer

namespace std {
using T = code::analyzer::compile_command;
template <>
struct less<T>
{
    bool operator()(const T &lhs, const T &rhs) const
    {
        return lhs.m_file < rhs.m_file;
    }
};
} // namespace std
