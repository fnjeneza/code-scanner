#include "code-scanner/code-scanner.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>

#include <json.hpp>

#include <clang-c/CXCompilationDatabase.h>
#include <clang-c/Index.h>

#include "utils.hpp"

namespace {
std::string to_string(const CXString &cx_str)
{
    auto cstr = clang_getCString(cx_str);

    if (cstr == NULL)
    {
        return std::string();
    }

    std::string str(cstr);
    clang_disposeString(cx_str);
    return str;
}
} // anonymous namespace

namespace code {
namespace analyzer {

class Parser_Impl
{
  public:
    Parser_Impl();
    ~Parser_Impl();

    // Retrieve a cursor from a file/line/column
    CXCursor cursor(const std::string & filename,
                    const unsigned int &line,
                    const unsigned int &column);

    // Retrieve all callers
    std::vector<CXCursor> callers(const CXCursor &cursor) const;

    void initialize(const std::string &             root_uri,
                    const std::vector<std::string> &compile_arguments,
                    const std::vector<std::string> &flags_to_ignore);
    void parse(const std::string &filename);
    std::vector<std::string> get_all_filenames();
    CXCursor find(const std::string &usr);

  private:
    void source_compile_flags(const CXCompileCommands &compile_commands);
    std::vector<std::string> header_compile_flags();
    void set_flags(const std::string &filename);

    // Fetch all include directories
    void find_all_include_directories();

  private:
    std::string              m_root_uri;
    std::vector<std::string> m_flags;
    // TODO read elements from config file
    std::vector<std::string>
                          m_flags_to_ignore; // = {"all", "-pc32", "-restrict", "-debug" };
    CXIndex               m_index;
    CXTranslationUnit     m_unit;
    CXCompilationDatabase m_db;
};

// Retrieve the declaration of a cursor
CXCursor reference(const CXCursor &cursor);

// Retrieve the definition
CXCursor definition(const CXCursor &cursor);

// TODO Retrieve the declaration of a cursor
CXCursor declaration(const CXCursor &cursor);

// Retrieve a type of cursor
std::string type(const CXCursor &cursor);

// Retrieve the location as file, line, column
std::tuple<std::string, unsigned int, unsigned int>
location(const CXCursor &cursor);

Parser_Impl::Parser_Impl()
    : m_root_uri{}
    , m_flags{}
    , m_flags_to_ignore{}
    , m_index{clang_createIndex(1, 1)}
    , m_unit{nullptr}
    , m_db{nullptr}
{
}

Parser_Impl::~Parser_Impl()
{
    clang_disposeTranslationUnit(m_unit);
    clang_disposeIndex(m_index);
    clang_CompilationDatabase_dispose(m_db);
}

void Parser_Impl::parse(const std::string &filename)
{

    set_flags(filename);
    // convert to "const char *" understable by parseTranslationUnit
    std::vector<const char *> flags;
    for (const auto &flag : m_flags)
    {
        flags.push_back(flag.c_str());
    }

    auto error =
        clang_parseTranslationUnit2FullArgv(m_index,
                                            filename.c_str(),
                                            &flags[0],
                                            static_cast<int>(flags.size()),
                                            nullptr,
                                            0,
                                            CXTranslationUnit_None,
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
}

CXCursor Parser_Impl::find(const std::string &usr)
{
    // TODO investigate clang_findReferencesInFile() a Higher level API
    // functions

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    // traverse the AST and check every cursor if it is equal to the
    // cursor declaration
    // ignore the cursor which point to himself

    // will be populate by the cursor definition if found
    using Data = std::tuple<std::string, CXCursor>;
    // USR and CXCursor to pass to the visitor
    Data _user_data{usr, clang_getNullCursor()};
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor /*parent*/, CXClientData client_data) {

            // do not visit if parent is method or function definition
            // if(clang_Kind parent == CXXMethod or Function)
            // TODO do something
            // CXChildVisit_Continue
            Data *       data = static_cast<Data *>(client_data);
            std::string &_usr = std::get<0>(*data);

            if (to_string(clang_getCursorUSR(cursor_)) == _usr)
            {
                if (clang_isCursorDefinition(cursor_))
                {
                    std::get<1>(*data) = cursor_;
                    return CXChildVisit_Break;
                }
            }

            return CXChildVisit_Recurse;
        },
        &_user_data);

    return std::get<1>(_user_data);
}

void Parser_Impl::source_compile_flags(const CXCompileCommand &compile_command)
{
    unsigned number_args = clang_CompileCommand_getNumArgs(compile_command);

    for (unsigned i = 1; i < number_args; ++i)
    {
        std::string str(
            to_string(clang_CompileCommand_getArg(compile_command, i)));
        if (str == "-o" || str == "-c")
        {
            ++i;
            continue;
        }
        m_flags.push_back(str);
    }
}

std::vector<std::string> Parser_Impl::header_compile_flags()
{
    find_all_include_directories();
    std::vector<std::string> flags = m_flags;
    return flags;
}

void Parser_Impl::set_flags(const std::string &filename)
{
    CXCompileCommands compile_commands =
        clang_CompilationDatabase_getCompileCommands(m_db, filename.c_str());

    unsigned size = clang_CompileCommands_getSize(compile_commands);

    if (size != 0) // is source file
    {
        // compile command of the source file
        CXCompileCommand compile_command =
            clang_CompileCommands_getCommand(compile_commands, 0);

        // flags applied to the source file
        source_compile_flags(compile_command);
        clang_CompileCommands_dispose(compile_commands);
    }
    else // is header file
    {
        // TODO better handle errors
        std::cout << "compile command has size 0" << std::endl;
        // flags applied to the header file
        header_compile_flags();
    }

    // remove flags that can lead to an ASTRead Error
    for (const auto &value : m_flags_to_ignore)
    {
        std::remove(std::begin(m_flags), std::end(m_flags), value);
    }
}

void Parser_Impl::initialize(const std::string &             root_uri,
                             const std::vector<std::string> &compile_commands,
                             const std::vector<std::string> &flags_to_ignore)
{
    m_root_uri        = root_uri;
    m_flags           = compile_commands;
    m_flags_to_ignore = flags_to_ignore;

    CXCompilationDatabase_Error c_error = CXCompilationDatabase_NoError;
    m_db =
        clang_CompilationDatabase_fromDirectory(m_root_uri.c_str(), &c_error);

    if (c_error == CXCompilationDatabase_CanNotLoadDatabase)
    {
        // TODO Handle errors in ctor
        std::cout << "compilation database can not be loaded" << std::endl;
        return;
    }
}

// Retrieve a cursor from a file/line/column
CXCursor Parser_Impl::cursor(const std::string & filename,
                             const unsigned int &line,
                             const unsigned int &column)
{
    CXFile           file     = clang_getFile(m_unit, filename.c_str());
    CXSourceLocation location = clang_getLocation(m_unit, file, line, column);
    return clang_getCursor(m_unit, location);
}

std::vector<CXCursor> Parser_Impl::callers(const CXCursor &cursor) const
{
    // get cursor declaration
    CXCursor              cursor_decl = declaration(cursor);
    std::vector<CXCursor> cursors;
    std::tuple<CXCursor *, std::vector<CXCursor> *> cursor_data = {&cursor_decl,
                                                                   &cursors};

    // get translation unit cursor
    CXCursor unit_cursor = clang_getTranslationUnitCursor(m_unit);

    // traverse the AST and check every cursor if it is equal to the
    // cursor declaration
    // ignore the cursor which point to himself
    CXClientData user_data = static_cast<CXClientData>(&cursor_data);
    clang_visitChildren(
        unit_cursor,
        // visitor
        [](CXCursor cursor_, CXCursor, CXClientData client_data) {
            CXCursorKind cursor_kind = clang_getCursorKind(cursor_);
            if (cursor_kind != CXCursor_CallExpr)
            {
                return CXChildVisit_Recurse;
            }
            // current cursor declaration
            CXCursor current_cursor_decl = declaration(cursor_);
            using UserData = std::tuple<CXCursor *, std::vector<CXCursor> *>;
            UserData *             data = static_cast<UserData *>(client_data);
            CXCursor *             cursor_decl_ = std::get<0>(*data);
            std::vector<CXCursor> *cursors_     = std::get<1>(*data);
            unsigned               equal =
                clang_equalCursors(current_cursor_decl, *cursor_decl_);
            if (equal)
            {
                (*cursors_).push_back(cursor_);
            }
            // Continue to search more
            return CXChildVisit_Recurse;
        },
        user_data);

    return cursors;
}

void Parser_Impl::find_all_include_directories()
{
    auto all_compile_commands =
        clang_CompilationDatabase_getAllCompileCommands(m_db);
    auto size = clang_CompileCommands_getSize(all_compile_commands);
    std::unordered_set<std::string> local_include_dirs;
    std::unordered_set<std::string> system_include_dirs;
    for (std::size_t i = 0; i < size; ++i)
    {
        auto command = clang_CompileCommands_getCommand(
            all_compile_commands, static_cast<unsigned int>(i));
        auto number_args = clang_CompileCommand_getNumArgs(command);
        for (std::size_t pos = 1; pos < number_args; ++pos)
        {
            std::string str(to_string(clang_CompileCommand_getArg(
                command, static_cast<unsigned int>(pos))));
            if (str == "-o" || str == "-c")
            {
                ++pos;
                continue;
            }
            if (str.substr(0, 2) == "-I")
            {
                local_include_dirs.emplace(str);
                continue;
            }
            if (str == "-isystem")
            {
                // read the next argument and increment the index in same time
                std::string dir_path(to_string(clang_CompileCommand_getArg(
                    command, static_cast<unsigned int>(++pos))));
                // TODO try std::move on emplace
                system_include_dirs.emplace(dir_path);
                continue;
            }
        }
    }

    // free compile commands
    clang_CompileCommands_dispose(all_compile_commands);

    // TODO try std::move on pus_back
    for (const auto &include_dir : system_include_dirs)
    {
        m_flags.push_back("-isystem");
        m_flags.push_back(include_dir);
    }
    for (const auto &include_dir : local_include_dirs)
    {
        m_flags.push_back(include_dir);
    }
}

CXCursor declaration(const CXCursor &cursor)
{
    auto cur = clang_getCursorDefinition(cursor);
    return clang_getCanonicalCursor(cur);
}

CXCursor reference(const CXCursor &cursor)
{
    return clang_getCursorReferenced(cursor);
}

CXCursor definition(const CXCursor &cursor)
{
    // search for definition in current translation unit
    CXCursor cursor_ = clang_getCursorDefinition(cursor);
    return cursor_;
}

std::vector<std::string> Parser_Impl::get_all_filenames()
{

    // all files associated to the compile commands
    std::vector<std::string> files;
    auto                     all_compile_commands =
        clang_CompilationDatabase_getAllCompileCommands(m_db);
    auto size = clang_CompileCommands_getSize(all_compile_commands);
    // parse each file
    for (unsigned i = 0; i < size; ++i)
    {
        auto command =
            clang_CompileCommands_getCommand(all_compile_commands, i);
        auto filename = to_string(clang_CompileCommand_getFilename(command));
        files.push_back(filename);
    }
    return files;
}

// Retrieve a type of cursor
std::string type(const CXCursor &cursor)
{
    // cursor type
    CXType type = clang_getCursorType(cursor);
    // cursor type spelling
    std::string spelling = to_string(clang_getTypeSpelling(type));
    return spelling;
}

std::tuple<std::string, unsigned int, unsigned int>
location(const CXCursor &cursor)
{
    CXSourceLocation location = clang_getCursorLocation(cursor);

    CXFile   file;
    unsigned line;
    unsigned column;
    clang_getSpellingLocation(location, &file, &line, &column, nullptr);

    // filename
    std::string _filename = to_string(clang_getFileName(file));
    return std::make_tuple(_filename, line, column);
}

Parser::Parser()
    : pimpl{std::make_unique<Parser_Impl>()}
{
}

Parser::~Parser() = default;

void Parser::initialize(const InitializeParams &params)
{
    const std::string root_uri = params.rootUri;
    // compile arguments from initializationOptions
    std::vector<std::string> compile_arguments;

    using json = nlohmann::json;
    if (params.initializationOptions.empty())
    {
        std::cerr << "Missing initialization options" << std::endl;
        // TODO return error code
        return;
    }

    json conf             = json::parse(params.initializationOptions);
    auto compile_commands = conf["compile_commands"];

    if (!compile_commands.empty())
    {
        compile_arguments = utils::split(compile_commands);
    }

    auto flags_to_ignore = conf["ignore_flags"];
    pimpl->initialize(root_uri, compile_arguments, flags_to_ignore);
}

Location Parser::definition(const TextDocumentPositionParams &params)
{
    pimpl->parse(params.textDocument.uri);
    auto cursor = pimpl->cursor(params.textDocument.uri,
                                params.position.line,
                                params.position.character);
    CXCursor found = code::analyzer::definition(cursor);

    // retrieve location from CXCursor
    auto get_location = [](CXCursor &_cursor) {

        auto loc = location(_cursor);

        Position position;
        position.line      = std::get<1>(loc);
        position.character = std::get<2>(loc);

        Range range;
        range.start = position;
        range.end   = position;

        Location _location;
        _location.uri   = std::get<0>(loc);
        _location.range = range;
        return _location;
    };

    // if a cursor has been found
    if (!clang_Cursor_isNull(found))
    {
        return get_location(found);
    }

    auto              cu        = clang_getCursorReferenced(cursor);
    const std::string usr       = to_string(clang_getCursorUSR(cu));
    auto              filenames = pimpl->get_all_filenames();
    for (auto f : filenames)
    {
        pimpl->parse(f);
        found = pimpl->find(usr);
        if (clang_Cursor_isNull(found))
        {
            continue;
        }
        return get_location(found);
    }

    Location location;
    return location;
}

Location Parser::references(const ReferenceParams &params)
{
    pimpl->parse(params.textDocument.uri);
    auto cursor = pimpl->cursor(params.textDocument.uri,
                                params.position.line,
                                params.position.character);
    cursor   = code::analyzer::reference(cursor);
    auto loc = location(cursor);

    Position position;
    position.line      = std::get<1>(loc);
    position.character = std::get<2>(loc);

    Range range;
    range.start = position;
    range.end   = position;

    Location _location;
    _location.uri   = std::get<0>(loc);
    _location.range = range;
    return _location;
}

} // namespace analyzer
} // namespace code
