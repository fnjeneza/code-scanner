#pragma once

#include <string>
#include <vector>

namespace code::analyzer {
using command_t = std::vector<std::string>;
struct compile_command
{
    // TODO no need of default ctor
    compile_command() = default;
    compile_command(const std::string_view &file);

    compile_command(const std::string_view &directory,
                    const std::string_view &command,
                    const std::string_view &file);

    ~compile_command() = default;

    bool operator==(const compile_command &cmd)
    {
        // TODO Compare vector too
        return m_file == cmd.m_file && m_directory == cmd.m_directory;
    }

    bool operator!=(const compile_command &cmd) { return *this == cmd; }

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
        if (lhs.m_file < rhs.m_file || lhs.m_directory < rhs.m_directory ||
            lhs.m_command.size() != rhs.m_command.size())
        {
            return true;
        }
        else
        {

            for (std::size_t i = 0; i < lhs.m_command.size(); ++i)
            {
                if (lhs.m_command[i] < rhs.m_command[i])
                {
                    return true;
                }
            }
        }
        return false;
    }
};
} // namespace std
