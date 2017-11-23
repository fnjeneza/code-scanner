#pragma once

#include <set>
#include <string>
#include <vector>

#include "compile_command.hpp"

namespace code::analyzer {

using command_t = std::vector<std::string>;
struct compile_database_t
{
    compile_database_t() = delete;
    compile_database_t(const std::string_view &directory,
                       const std::string_view &prefix_compile_command = "",
                       const command_t &       flags_to_ignore = command_t());

    ~compile_database_t() = default;

    // static std::vector<std::string>
    // compile_commands(const std::string_view &filename);

    // static std::vector<std::string> source_filenames();

    // retrieve the compile commands for a given filename
    // a file can have multiple compile command
    std::vector<compile_command>
    compile_commands2(const std::string_view &filename);

    // return all compile command available
    // One file can have multiple compile command
    std::set<compile_command> all_compile_commands() const noexcept
    {
        return m_compile_commands;
    }

    void                            parse_compile_commands() noexcept;
    void                            extract_includes();
    void                            update();
    const std::vector<std::string> &all_includes() const noexcept
    {
        return m_all_includes;
    }
    void merge(std::set<compile_command> &&commands) noexcept
    {
        m_compile_commands.merge(commands);
    }

  private:
    // build dir filename
    std::string               m_compile_commands_json_db;
    long int                  m_timestamp = 0;
    std::set<compile_command> m_compile_commands;
    std::string               m_flags_to_ignore;
    std::string               m_prefix_compile_command;
    std::vector<std::string>  m_all_includes;
};
} // namespace code::analyzer
