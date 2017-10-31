#pragma once
#include <string>

namespace code::analyzer
{
struct compile_command
{
    compile_command(const std::string_view &directory,
                    const std::string_view &command,
                    const std::string_view &file)
        : m_directory{directory}
        , m_command{command}
        , m_file{file}
    {
    }
    ~compile_command() = default;

    std::string_view m_directory;
    std::string_view m_command;
    std::string_view m_file;
};
}

namespace std
{
    using T = code::analyzer::compile_command;
    template<> struct less<T>
    {
        bool operator()(const T &lhs, const T &rhs) const
        {
            return lhs.m_file < rhs.m_file;
        }
    };
}
