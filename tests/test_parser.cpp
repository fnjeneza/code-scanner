#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "parser.hpp"

TEST_CASE("cpp parser", "[cpp_parser]")
{
    std::string            filename("data/index.cpp");
    code::analyzer::Parser parser(filename);

    SECTION("retrieve cursor")
    {
    unsigned long          line   = 11;
    unsigned long          column = 10;
    CXCursor               cursor = parser.cursor(line, column);

    auto loc          = code::analyzer::location(cursor);
    auto filename_ret = std::get<0>(loc);
    auto line_ret     = std::get<1>(loc);
    auto column_ret   = std::get<2>(loc);
    REQUIRE(filename_ret == filename);
    REQUIRE(line_ret == line);
    REQUIRE(column_ret == column);
    }

    SECTION("locate implementation/definition")
    {
        auto cursor = parser.cursor(35, 10);
        cursor = code::analyzer::definition(cursor);

        auto cursor_expected = parser.cursor(17,13);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));
    }

    SECTION("locate declaration")
    {
        auto cursor = parser.cursor(26,5);
        cursor = code::analyzer::declaration(cursor);
        auto cursor_expected = parser.cursor(11,10);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));

        cursor = parser.cursor(35,11);
        cursor = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(11,10);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));

        cursor = parser.cursor(35,5);
        cursor = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(32,11);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));

        cursor = parser.cursor(32,6);
        cursor = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(5,7);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));

        cursor = parser.cursor(37,13);
        cursor = code::analyzer::reference(cursor);
        cursor_expected = parser.cursor(29,22);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));
    }
}

