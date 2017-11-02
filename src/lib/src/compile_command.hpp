#pragma once

#include <string>
#include <vector>

// TODO remove utils and use  template split
#include "utils.hpp"

#include "filesystem.hpp"

namespace code::analyzer {
using command_t = std::vector<std::string>;

struct compile_command
{
    compile_command(const std::string_view &file)
        : m_file{std::string(std::filesystem::canonical(file))}
    {
    }

    compile_command(const std::string_view &directory,
                    const std::string_view &command,
                    const std::string_view &file)
        : m_file{std::string(std::filesystem::canonical(file))}
        , m_directory{std::string(std::filesystem::canonical(directory))}
    {
        m_command = utils::split(command);
    }

    ~compile_command() = default;

    // Normalizing command is the action by which all path are canonicalized
    void normalize_command()
    {
        bool absolutize_argument = false;
        // remove the compiler binary name
        // m_command.erase(std::begin(m_command));
        auto canonical = [this](const std::string_view &filename,
                                const std::string_view &directory) {
            auto _filename = std::filesystem::path(filename);
            if (_filename.is_relative())
            {
                // create an absolute path
                _filename = directory / _filename;
                // ensure there is no link directory in the name by
                // canonicalization
                // TODO can throw
                _filename = std::filesystem::canonical(_filename);
            }
            return _filename;
        };

        for (std::size_t i = 0; i < m_command.size(); ++i)
        {
            auto __command = m_command[i];
            if (__command.empty())
            {
                continue;
            }
            if (__command.substr(0, 2) == "-I")
            {
                auto path = std::filesystem::path(__command.substr(2));
                path      = canonical(path.c_str(), m_directory);
                // use canonical path
                __command = "-I" + std::string(path);
            }
            if (absolutize_argument)
            {
                auto path           = std::filesystem::path(__command);
                path                = canonical(path.c_str(), m_directory);
                absolutize_argument = false;
            }
            if (__command.substr(0, 8) == "-isystem")
            {
                // used to make absolute next argument
                absolutize_argument = true;
            }
            if (__command == "-o" || __command == "-c")
            {
                ++i;
                continue;
            }
            m_command[i] = __command;
        }
    }

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
