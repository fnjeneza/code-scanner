#include <catch.hpp>

#include <array>

#include "compile_database_t.hpp"

/// check that compile_commands file is checked  when ctor is invoked
TEST_CASE("compile_database_t", "[compile_database_t]")
{

    code::analyzer::compile_database_t db(".");

    // total compile command in the file
    REQUIRE(db.all_compile_commands().size() == 28);
}

/// check that it is possible to retrieve all commands related to a file
/// compilation
TEST_CASE("all_compile_commands", "[compile_database_t]")
{
    code::analyzer::compile_database_t db(".");
    auto cmds = db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/util.cpp");
    REQUIRE(cmds.size() == 2);

    auto cmds2 =
        db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/idl_parser.cpp");
    REQUIRE(cmds2.size() == 2);
}

/// check that there is no empty string command
TEST_CASE("no_empty_command", "[compile_database_t]")
{
    code::analyzer::compile_database_t db(".");
    auto cmds = db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/util.cpp");
    REQUIRE(cmds.size() == 2);

    for (const auto &cmd : cmds)
    {
        for (const auto &argument : cmd.m_command)
        {
            REQUIRE(argument.empty() == false);
        }
    }
}

TEST_CASE("flags_to_ignore", "[compile_database_t]")
{
    code::analyzer::compile_database_t db(".");
    auto                               cmds = db.compile_commands2(
        "/tmp/cpp-lsp/flatbuffers/grpc/src/compiler/cpp_generator.cc");
    REQUIRE(cmds.size() == 1);

    std::vector<std::string> flags_to_ignore{"-Wall", "-pedantic"};

    auto it = std::find(std::cbegin(cmds[0].m_command),
                        std::cend(cmds[0].m_command),
                        flags_to_ignore[0]);
    REQUIRE(it != std::cend(cmds[0].m_command));

    auto it2 = std::find(std::cbegin(cmds[0].m_command),
                         std::cend(cmds[0].m_command),
                         flags_to_ignore[1]);
    REQUIRE(it2 != std::cend(cmds[0].m_command));

    code::analyzer::compile_database_t db2(".", "", flags_to_ignore);
    auto                               cmds2 = db2.compile_commands2(
        "/tmp/cpp-lsp/flatbuffers/grpc/src/compiler/cpp_generator.cc");
    REQUIRE(cmds2.size() == 1);

    auto it3 = std::find(std::cbegin(cmds2[0].m_command),
                         std::cend(cmds2[0].m_command),
                         flags_to_ignore[0]);
    REQUIRE(it3 == std::cend(cmds2[0].m_command));

    auto it4 = std::find(std::cbegin(cmds2[0].m_command),
                         std::cend(cmds2[0].m_command),
                         flags_to_ignore[1]);
    REQUIRE(it4 == std::cend(cmds2[0].m_command));
}

TEST_CASE("prefix_compile_command", "[compile_database_t")
{
    code::analyzer::compile_database_t db(".", "clang -x c++");
    auto                               cmds =
        db.compile_commands2("/tmp/cpp-lsp/flatbuffers/src/flatc_main.cpp");
    REQUIRE(cmds.size() == 1);

    std::vector<std::string> flags{"clang", "-x", "c++"};

    auto it = std::find(
        std::cbegin(cmds[0].m_command), std::cend(cmds[0].m_command), flags[0]);
    REQUIRE(it != std::cend(cmds[0].m_command));
}

TEST_CASE("all_includes", "[compile_database_t")
{
    code::analyzer::compile_database_t db(
        ".", "clang -x c++ -isystem /tmp/cpp-lsp/flatbuffers");
    auto all_includes = db.all_includes();
    REQUIRE(all_includes.size() == 6);

    std::array<std::string, 6> values{
        "-I/tmp/cpp-lsp/flatbuffers/build/../grpc",
        "-I/tmp/cpp-lsp/flatbuffers/build/../include",
        "-I/tmp/cpp-lsp/flatbuffers/build/samples",
        "-I/tmp/cpp-lsp/flatbuffers/build/tests",
        "-isystem",
        "/tmp/cpp-lsp/flatbuffers"};

    for (const auto &value : values)
    {
        auto it = std::find(
            std::cbegin(all_includes), std::cend(all_includes), value);
        REQUIRE(it != std::cend(all_includes));
    }
}
