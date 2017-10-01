#include "Parser_Impl.hpp"

#include <algorithm>

#include "compile_database_t.hpp"
#include "config.hpp"
#include "repository.hpp"
#include "task_system.hpp"
#include "translation_unit_t.hpp"

namespace code::analyzer {

void Parser_Impl::initialize(const std::string &             root_uri,
                             const std::vector<std::string> &compile_commands,
                             const std::vector<std::string> &flags_to_ignore)
{
    config::builder(root_uri, compile_commands, flags_to_ignore);
    task_system task;

    // if there are files that timestamp has expired rescan them
    // TODO remove the constant whic force the scan
    if (/*repository.check_file_timestamp().size()*/ 1 != 0)
    {
        auto all_filenames = compile_database_t::source_filenames();
        auto filenames     = m_repository.check_file_timestamp(all_filenames);

        for (auto &file : filenames)
        {
            task.async([file, this]() {
                auto usrs =
                    translation_unit_t(file).retrieve_all_identifier_usr();
                m_repository.emplace(file, usrs);
            });
        }
    }
}

Location Parser_Impl::definition(const TextDocumentPositionParams &params)
{
    auto     tu       = translation_unit_t(params.textDocument.uri);
    Location location = tu.definition(params.position);
    if (!location.is_valid())
    {
        auto usr = tu.usr(params.position);
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
    Location location =
        translation_unit_t(params.textDocument.uri).reference(params.position);
    return location;
}

} // namespace code::analyzer
