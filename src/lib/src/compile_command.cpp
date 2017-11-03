#include "compile_command.hpp"
// TODO remove utils and use  template split
#include "utils.hpp"

#include "filesystem.hpp"
namespace code::analyzer {
// declarations
std::string canonical(const std::string_view &directory,
                      const std::string_view &filename);
command_t   normalize_command(const std::string_view &directory,
                              command_t &&            command);
// end of declarations

std::string canonical(const std::string_view &directory,
                      const std::string_view &filename)
{
    auto _filename = std::filesystem::path(filename);
    if (_filename.is_relative())
    {
        // create an absolute path
        _filename = directory / _filename;
    }
    return _filename;
}

// Normalizing command is the action by which all path are canonicalized
command_t normalize_command(const std::string_view &directory,
                            command_t &&            command)
{
    bool absolutize_argument = false;

    command_t __temp;

    // remove the last 4 arguments. They are not used by the parser
    for (std::size_t i = 0; i < command.size() - 4; ++i)
    {
        auto __command = command[i];
        if (__command.empty())
        {
            continue;
        }
        if (__command.substr(0, 2) == "-I")
        {
            __command = canonical(directory, __command.substr(2));
            // use canonical path
            __command = "-I" + __command;
        }
        if (absolutize_argument)
        {
            __command           = canonical(directory, __command);
            absolutize_argument = false;
        }
        if (__command.substr(0, 8) == "-isystem")
        {
            // used to make absolute next argument
            absolutize_argument = true;
        }
        __temp.push_back(__command);
    }
    return std::move(__temp);
}

compile_command::compile_command(const std::string_view &file)
    : m_file{std::string(std::filesystem::canonical(file))}
{
}

compile_command::compile_command(const std::string_view &directory,
                                 const std::string_view &command,
                                 const std::string_view &file)
    : m_file{std::string(std::filesystem::canonical(file))}
    , m_directory{std::string(std::filesystem::canonical(directory))}
{
    m_command = normalize_command(m_directory, utils::split(command));
}
} // namespace code::analyzer
