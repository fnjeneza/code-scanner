#include "translation_unit_t.hpp"
#include "compile_database_t.hpp"
#include "utils.hpp"
#include <iostream>
#include <tuple>

namespace code {
namespace analyzer {

namespace {
CXTranslationUnit_Flags option(const translation_unit_flag &flag)
{
    switch (flag)
    {
    case translation_unit_flag::skip_function_bodies:
        return CXTranslationUnit_SkipFunctionBodies;
    case translation_unit_flag::none:
    default:
        return CXTranslationUnit_None;
    }
}
}

translation_unit_t::translation_unit_t(const std::string &filename)
    : m_unit{nullptr}
    , m_filename{filename}
{
}

translation_unit_t::~translation_unit_t()
{
    clang_disposeTranslationUnit(m_unit);
}

void translation_unit_t::parse(const translation_unit_flag &opt)
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

    auto error =
        clang_parseTranslationUnit2FullArgv(index,
                                            m_filename.c_str(),
                                            flags.data(),
                                            static_cast<int>(flags.size()),
                                            nullptr,
                                            0,
                                            option(opt),
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

Location translation_unit_t::definition(const Position &position)
{
    parse();
    auto _cursor = cursor(position);
    // search for definition
    return utils::location(clang_getCursorDefinition(_cursor));
}

Location translation_unit_t::definition(const std::string &usr)
{
    parse();
    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);
    using Data           = std::tuple<const std::string &, Location &>;
    Location location;
    Data     data{usr, location};

    // search definition
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            auto        __data = static_cast<Data *>(client_data);
            auto &      __usr  = std::get<0>(*__data);
            std::string current_cursor_usr =
                utils::to_string(clang_getCursorUSR(cursor_));
            if (current_cursor_usr == __usr)
            {
                auto &__location = std::get<1>(*__data);
                __location =
                    utils::location(clang_getCursorDefinition(cursor_));
                return CXChildVisit_Break;
            }
            return CXChildVisit_Recurse;
        },
        &data);
    return location;
}

Location translation_unit_t::reference(const Position &position)
{
    parse();
    auto _cursor = cursor(position);
    return utils::location(clang_getCursorReferenced(_cursor));
}

std::string translation_unit_t::usr(const Position &position)
{
    parse();
    auto _cursor = cursor(position);
    return utils::to_string(
        clang_getCursorUSR(clang_getCursorReferenced(_cursor)));
}

std::set<std::string> translation_unit_t::retrieve_all_identifier_usr()
{
    parse(translation_unit_flag::skip_function_bodies);

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
            std::string str = utils::to_string(clang_getCursorUSR(cursor_));
            if (!str.empty() && utils::is_identifier(cursor_))
            {
                auto __location = utils::location(cursor_);
                if (__filename == __location.uri &&
                    utils::is_declaration_locate_in_other_file(cursor_))
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

CXCursor translation_unit_t::cursor(const Position &position)
{
    CXFile file = clang_getFile(m_unit, m_filename.c_str());
    if (file == nullptr)
    {
        return clang_getNullCursor();
    }
    CXSourceLocation __location =
        clang_getLocation(m_unit, file, position.line, position.character);
    return clang_getCursor(m_unit, __location);
}
}
}
