#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "compile_database_t.hpp"

/// check that compile_commands file is checked  when ctor is invoked
TEST_CASE("compile_database_t", "[object_creation]")
{

    code::analyzer::compile_database_t db(".");

    // total compile command in the file
    REQUIRE(db.m_compile_commands.size() == 38);
}

/// check that it is possible to retrieve all commands related to a file
/// compilation
TEST_CASE("all_compile_commands", "[all_compile_commands]")
{
    code::analyzer::compile_database_t db(".");
    auto cmds = db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/util.cpp");
    REQUIRE(cmds.size() == 4);

    auto cmds2 = db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/idl_parser.cpp");
    REQUIRE(cmds2.size() == 4);
}
