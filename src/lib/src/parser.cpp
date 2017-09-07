#include "code-scanner/code-scanner.hpp"

#include <algorithm>
#include <array>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_set>
#include <vector>
#include <experimental/filesystem>

#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>

namespace std {
namespace filesystem = std::experimental::filesystem;
}

namespace {
std::string to_string(const CXString &cx_str)
{
    auto cstr= clang_getCString(cx_str);

    if (cstr == NULL)
    {
        return std::string();
    }

    std::string str(cstr);
    clang_disposeString(cx_str);
    return str;
}

bool is_header_file(const std::string & filename)
{
    std::string extension = std::filesystem::path(filename).extension();
    std::array<std::string, 3> header_extensions = {".h",".hpp",".hxx"};
    auto found = std::find(std::begin(header_extensions), std::end(header_extensions), extension);
    return found != std::end(header_extensions);
}
} // anonymous namespace

namespace code {
namespace analyzer {

class Parser_Impl
{
  public:
    Parser_Impl() = delete;
    Parser_Impl(const std::string &build_dir, const std::string &filename, const std::vector<std::string> & compile_arguments);
    ~Parser_Impl();

    // Retrieve a cursor from a file/line/column
    CXCursor cursor(const unsigned long &line, const unsigned long &column);

    // Retrieve all callers
    std::vector<CXCursor> callers(const CXCursor &cursor) const;

    //.Retrieve name of the file being processed
    std::string filename() const;

    // Fetch all include directories
    void find_all_include_directories();

  private:
    std::string       m_filename;
    std::vector<std::string> include_directories;
    CXIndex           m_index;
    CXTranslationUnit m_unit;
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
std::tuple<std::string, unsigned long, unsigned long>
location(const CXCursor &cursor);

Parser_Impl::Parser_Impl(const std::string &build_dir, const std::string &filename, const std::vector<std::string> & compile_arguments)
    : m_filename{filename}
    , m_index{clang_createIndex(1, 1)}
    , m_unit{nullptr}
    , m_db{nullptr}
{
    std::string _filename =         std::filesystem::absolute(filename);
    CXCompilationDatabase_Error c_error = CXCompilationDatabase_NoError;
    m_db =
        clang_CompilationDatabase_fromDirectory(build_dir.c_str(), &c_error);

    if (c_error == CXCompilationDatabase_CanNotLoadDatabase)
    {
        // TODO Handle errors in ctor
        std::cout << "compilation database can not be loaded" << std::endl;
        return;
    }

    CXCompileCommands compile_commands =
        clang_CompilationDatabase_getCompileCommands(m_db, _filename.c_str());

    unsigned size = clang_CompileCommands_getSize(compile_commands);
    if (size == 0)
    {
        // TODO better handle errors
        std::cout << "compile command has size 0" << std::endl;
        return;
    }

    CXCompileCommand compile_command =
        clang_CompileCommands_getCommand(compile_commands, 0);
    unsigned number_args = clang_CompileCommand_getNumArgs(compile_command);
    std::vector<std::string> arguments = compile_arguments;

    for (unsigned i = 1; i < number_args; ++i)
    {
        std::string str(
            to_string(clang_CompileCommand_getArg(compile_command, i)));
        if (str == "-o" || str == "-c")
        {
            ++i;
            continue;
        }
        arguments.push_back(str);
    }

    // TODO read elements from config file
    std::vector<std::string> flags_to_ignore = {"all", "-pc32", "-restrict", "-debug" };

    for(const auto & value : flags_to_ignore)
    {
        std::remove(std::begin(arguments), std::end(arguments), value);
    }

    std::vector<const char *> flags;
    for (const auto &argument : arguments)
    {
        flags.push_back(argument.c_str());
    }

    clang_CompileCommands_dispose(compile_commands);
    auto error = clang_parseTranslationUnit2FullArgv(m_index,
                                                     _filename.c_str(),
                                                     &flags[0],
                                                     flags.size(),
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

Parser_Impl::~Parser_Impl()
{
    clang_disposeTranslationUnit(m_unit);
    clang_disposeIndex(m_index);
    clang_CompilationDatabase_dispose(m_db);
}

// Retrieve a cursor from a file/line/column
CXCursor Parser_Impl::cursor(const unsigned long &line, const unsigned long &column)
{
    CXFile           file     = clang_getFile(m_unit, m_filename.c_str());
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
        [](CXCursor cursor, CXCursor, CXClientData client_data) {
            CXCursorKind cursor_kind = clang_getCursorKind(cursor);
            if (cursor_kind != CXCursor_CallExpr)
            {
                return CXChildVisit_Recurse;
            }
            // current cursor declaration
            CXCursor current_cursor_decl = declaration(cursor);
            using UserData = std::tuple<CXCursor *, std::vector<CXCursor> *>;
            UserData *             data = static_cast<UserData *>(client_data);
            CXCursor *             cursor_decl = std::get<0>(*data);
            std::vector<CXCursor> *cursors     = std::get<1>(*data);
            unsigned               equal =
                clang_equalCursors(current_cursor_decl, *cursor_decl);
            if (equal)
            {
                (*cursors).push_back(cursor);
            }
            // Continue to search more
            return CXChildVisit_Recurse;
        },
        user_data);

    return cursors;
}

void Parser_Impl::find_all_include_directories()
{
  auto all_compile_commands = clang_CompilationDatabase_getAllCompileCommands(m_db);
  auto size = clang_CompileCommands_getSize(all_compile_commands);
  std::unordered_set<std::string> local_include_dirs;
  std::unordered_set<std::string> system_include_dirs;
  for(std::size_t i = 0; i< size; ++i)
  {
    auto command = clang_CompileCommands_getCommand(all_compile_commands, i);
    auto number_args = clang_CompileCommand_getNumArgs(command);
    for (std::size_t pos = 1; pos < number_args; ++pos)
    {
      std::string str(
          to_string(clang_CompileCommand_getArg(command, pos)));
      if (str == "-o" || str == "-c")
      {
          ++pos;
          continue;
      }
      if(str.substr(0, 2) == "-I")
      {
        local_include_dirs.emplace(str);
        continue;
      }
      if(str == "-isystem")
      {
        // read the next argument and increment the index in same time
        std::string dir_path(to_string(clang_CompileCommand_getArg(command, ++pos)));
        // TODO try std::move on emplace
        system_include_dirs.emplace(dir_path);
        continue;
      }
    }
  }

  // free compile commands
  clang_CompileCommands_dispose(all_compile_commands);

  // remove all previously findings
  include_directories.clear();
  // TODO try std::move on pus_back
  for(const auto & include_dir : system_include_dirs)
  {
      include_directories.push_back("-isystem");
      include_directories.push_back(include_dir);
  }
  for(const auto & include_dir : local_include_dirs)
  {
      include_directories.push_back(include_dir);
  }
}

std::string Parser_Impl::filename() const
{
    CXFile      file     = clang_getFile(m_unit, m_filename.c_str());
    std::string spelling = to_string(clang_getFileName(file));
    return spelling;
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
    return clang_getCursorDefinition(cursor);
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

std::tuple<std::string, unsigned long, unsigned long>
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

Parser::Parser() = default;
Parser::~Parser() = default;

Location Parser::definition(const TextDocumentPositionParams & params)
{
    Location location;
    return location;
}

Location Parser::references(const ReferenceParams & params)
{
    pimpl = std::make_unique<Parser_Impl>(params.build_dir, params.textDocument.uri, params.compile_arguments);
    auto cursor = pimpl->cursor(params.position.line, params.position.character);
    cursor = code::analyzer::reference(cursor);
    auto loc = location(cursor);

    Position position;
    position.line = std::get<1>(loc);
    position.character = std::get<2>(loc);

    Range range;
    range.start = position;
    range.end = position;

    Location _location;
    _location.uri = std::get<0>(loc);
    _location.range = range;
    return _location;
}

} // namespace analyzer
} // namespace code
