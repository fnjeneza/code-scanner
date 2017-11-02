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
    m_compile_db = std::make_unique<compile_database_t>(build_uri);
    if (ec)
    {
        return ec;
    }

    task_system task;

    {
        auto all_filenames = compile_database_t::source_filenames();
        // std::vector<std::string> _all_filenames;
        // for (const auto &command : m_compile_db->m_compile_commands)
        // {
        //     std::cout << command.m_file << std::endl;
        //     _all_filenames.push_back(command.m_file);
        // }
        // std::cout << "***********************\n";
        // for(auto & file: all_filenames)
        // {
        //     std::cout << file << std::endl;
        // }
        // std::cout << all_filenames.size() << std::endl;
        // std::cout << _all_filenames.size() << std::endl;
        auto filenames = m_repository.check_file_timestamp(all_filenames);

        for (auto &file : filenames)
        {
            task.async([file, this]() {
                auto usrs = translation_unit_t(file, true)
                                .retrieve_all_identifier_usr();
                m_repository.emplace(file, usrs);
            });
        }
    }

    // serialize the repository
    m_repository.serialize();

    return std::experimental::nullopt;
}

Location Parser_Impl::definition(const TextDocumentPositionParams &params)
{
    // {
    //     compile_database_t cdb(config::build_uri());
    //     auto               commands = cdb.compile_commands2(
    //         "/tmp/cpp-lsp/flatbuffers/src/idl_parser.cpp");
    //     std::cout << commands.size() << std::endl;
    //     for (auto &i : commands)
    //     {
    //         std::cout << i << std::endl;
    //     }
    // }
    m_tu.filename(params.textDocument.uri);
    Location location = m_tu.definition(params.position);
    if (!location.is_valid())
    {
        auto usr = m_tu.usr(params.position);
        // search in repository
        auto defs = m_repository.definitions(usr);

        for (auto def : defs)
        {
            location = translation_unit_t(def).definition(usr);
            if (location.is_valid())
            {
                return location;
            }
        }
    }
    return location;
}

Location Parser_Impl::reference(const TextDocumentPositionParams &params)
{
    m_tu.filename(params.textDocument.uri);
    Location location = m_tu.reference(params.position);
    return location;
}

} // namespace code::analyzer
