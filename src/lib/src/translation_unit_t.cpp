#include "translation_unit_t.hpp"
#include "compile_database_t.hpp"
#include "functional.hpp"
#include <iostream>
#include <tuple>

namespace code {
namespace analyzer {

translation_unit_t::translation_unit_t(const std::string &filename)
    : m_unit{nullptr}
    , m_filename{filename}
{
}

translation_unit_t::~translation_unit_t()
{
    clang_disposeTranslationUnit(m_unit);
}

void translation_unit_t::parse()
{
    auto __flags = compile_database_t::compile_commands(m_filename);
    // convert to "const char *" understable by parseTranslationUnit
    std::vector<const char *> flags;
    for (const auto &flag : __flags)
    {
        flags.push_back(flag.c_str());
    }

    // create index
    auto index = clang_createIndex(1, 1);

    auto error = clang_parseTranslationUnit2FullArgv(
        index,
        m_filename.c_str(),
        &flags[0],
        static_cast<int>(flags.size()),
        nullptr,
        0,
        CXTranslationUnit_SkipFunctionBodies,
        &m_unit);

    switch (error)
    {
    case CXError_Success:
        std::cout << "Succeeded\n";
        break;
    case CXError_InvalidArguments:
        std::cout << "Invalid argument\n";
        break;
    case CXError_ASTReadError:
        std::cout << "ASTRead Error\n";
        break;
    default:
        std::cout << "Error default\n";
        break;
    }

    // release index
    clang_disposeIndex(index);
}

std::set<std::string> translation_unit_t::retrieve_all_identifier_usr()
{
    parse();

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    std::set<std::string> identifiers;
    using data = std::tuple<const std::string &, std::set<std::string> &>;
    data _data{m_filename, identifiers};

    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            data *__data = static_cast<data *>(client_data);
            // retrieve the filename
            const auto &__filename    = std::get<0>(*__data);
            auto &      __identifiers = std::get<1>(*__data);
            std::string str           = to_string(clang_getCursorUSR(cursor_));
            if (!str.empty() && is_identifier(cursor_))
            {
                auto loc = location(cursor_);
                if (__filename == std::get<0>(loc) &&
                    is_declaration_locate_in_other_file(cursor_))
                {
                    // append the USR
                    __identifiers.emplace(str);
                }
            }

            return CXChildVisit_Recurse;
        },
        &_data);
    return identifiers;
}
}
}
