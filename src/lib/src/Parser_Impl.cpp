#include "Parser_Impl.hpp"

#include <algorithm>
#include <iostream>
#include <tuple>
#include <unordered_set>

#include "compile_database_t.hpp"
#include "config.hpp"
#include "repository.hpp"
#include "translation_unit_t.hpp"

namespace code {
namespace analyzer {

void Parser_Impl::initialize(const std::string &             root_uri,
                             const std::vector<std::string> &compile_commands,
                             const std::vector<std::string> &flags_to_ignore)
{
    config::builder(root_uri, compile_commands, flags_to_ignore);

    // if there are files that has expired timestamp
    // rescan them
    // TODO remove the constant whic force the scan
    if(/*repository.check_file_timestamp().size()*/ 1 != 0)
    {
        for (const auto &f : compile_database_t::source_filenames())
        {
            // USR defined in the file
            auto usrs = translation_unit_t(f).retrieve_all_identifier_usr();
            m_repository.emplace(f, usrs);
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

} // namespace analyzer
} // namespace code
