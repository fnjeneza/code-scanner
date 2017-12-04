#include "Parser_Impl.hpp"

#include <algorithm>

#include "compile_database_t.hpp"
#include "config.hpp"
#include "repository.hpp"
#include "task_system.hpp"
#include <iostream>

namespace code::analyzer {

Parser_Impl::Parser_Impl()  = default;
Parser_Impl::~Parser_Impl() = default;

std::experimental::optional<std::error_code>
Parser_Impl::initialize(const std::string &             build_uri,
                        const std::vector<std::string> &compile_commands,
                        const std::vector<std::string> &flags_to_ignore)
{
    auto ec = config::builder(build_uri, compile_commands, flags_to_ignore);
    std::string prefix;
    for (const auto &compile_command : compile_commands)
    {
        prefix += compile_command + " ";
    }
    m_compile_db = std::make_unique<compile_database_t>(
        build_uri, prefix, flags_to_ignore);
    if (ec)
    {
        return ec;
    }

    std::set<compile_command> headers_command;

    {
        task_system task;
        // auto all_filenames = compile_database_t::source_filenames();
        auto acc = m_compile_db->all_compile_commands();
        // acc      = m_repository.check_file_timestamp(acc);

        for (auto &cmd : acc)
        {
            translation_unit_t(cmd).index_source(m_index);
            // translation_unit_t(cmd).index_symbols(headers_command, m_index);
            // task.async([cmd, &headers_command, this]() {
            //     translation_unit_t(cmd).index_source(m_index);
            // });
        }
    }

    m_compile_db->merge(std::move(headers_command));

    return std::experimental::nullopt;
}

Location Parser_Impl::definition(const TextDocumentPositionParams &params)
{
    // TODO when cursor has multiple definition according to the compilations
    // flags
    auto found = std::cend(m_index);
    for (auto it = std::cbegin(m_index); it != std::cend(m_index); ++it)
    {
        const auto &sym = *it;
        if (params.textDocument.uri == sym.m_location.uri &&
            params.position.line == sym.m_location.range.start.line &&
            sym.m_location.range.start.character <= params.position.character &&
            params.position.character <= sym.m_location.range.end.character)
        {
            std::cout << "found " << sym.m_location.uri << " "
                      << sym.m_location.range.start.character
                      << "<=" << params.position.character
                      << "<=" << sym.m_location.range.end.character << " line "
                      << sym.m_location.range.start.line << " " << sym.m_usr
                      << std::endl;
            found = it;
            break;
        }
    }
    if (found != std::cend(m_index))
    {
        // search for definition in the index
        auto it_definition = std::find_if(
            std::cbegin(m_index), std::cend(m_index), [&found](auto &sym) {
                return sym.m_usr == found->m_usr &&
                       sym.m_kind == kind::decl_definition;
            });
        if (it_definition != std::cend(m_index))
        {
            return it_definition->m_location;
        }
    }
    return Location();
}

Location Parser_Impl::reference(const TextDocumentPositionParams &params)
{
    auto cmds = m_compile_db->compile_commands2(params.textDocument.uri);

    // TODO handle all compile cmds
    m_tu.compile_cmd(cmds[0]);
    Location location = m_tu.reference(params.position);
    return location;
}

} // namespace code::analyzer
