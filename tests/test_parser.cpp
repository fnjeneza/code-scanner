#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "parser.hpp"
#include "string_array.hpp"

TEST_CASE("cpp parser", "[cpp_parser]")
{
    std::string            filename("data/index.cpp");
    code::analyzer::Parser parser(filename);

    SECTION("retrieve cursor")
    {
        unsigned long line   = 11;
        unsigned long column = 10;
        CXCursor      cursor = parser.cursor(line, column);

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
        auto cursor = parser.cursor(29, 10);
        cursor      = code::analyzer::definition(cursor);

        auto cursor_expected = parser.cursor(17, 13);
        REQUIRE(clang_equalCursors(cursor, cursor_expected));
    }

    SECTION("locate declaration")
    {
        auto cursor          = parser.cursor(21, 24);
        cursor               = code::analyzer::declaration(cursor);
        auto cursor_expected = parser.cursor(10, 10);
        CHECK(clang_equalCursors(cursor, cursor_expected));

        cursor          = parser.cursor(29, 10);
        cursor          = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(10, 10);
        CHECK(clang_equalCursors(cursor, cursor_expected));

        cursor          = parser.cursor(29, 5);
        cursor          = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(26, 17);
        CHECK(clang_equalCursors(cursor, cursor_expected));

        cursor          = parser.cursor(26, 6);
        cursor          = code::analyzer::declaration(cursor);
        cursor_expected = parser.cursor(4, 7);
        CHECK(clang_equalCursors(cursor, cursor_expected));

        cursor          = parser.cursor(31, 13);
        cursor          = code::analyzer::reference(cursor);
        cursor_expected = parser.cursor(23, 22);
        CHECK(clang_equalCursors(cursor, cursor_expected));
    }

    SECTION("all function callers")
    {
        auto cursor  = parser.cursor(10, 12);
        auto callers = parser.callers(cursor);

        CHECK(callers.size() == 2);

        auto caller = code::analyzer::location(callers[0]);
        int  line   = std::get<1>(caller);
        int  column = std::get<2>(caller);
        CHECK(line == 21);
        CHECK(column == 24);

        caller = code::analyzer::location(callers[1]);
        line   = std::get<1>(caller);
        column = std::get<2>(caller);
        CHECK(line == 29);
        CHECK(column == 5);
    }
}

TEST_CASE("string array", "srting_array")
{
    string_array sa("hello world");
    auto         data = sa.data();
    CHECK(std::string(data[0]) == "hello");
    CHECK(std::string(data[1]) == "world");
    CHECK(sa.size() == 2);
}

TEST_CASE("Parser file with argument", "[arg_parser]")
{
    SECTION("retrieve cursor")
    {
        std::string filename =
            "/home/njeneza/workspace/cpp-parser/tests/data/test_arg.cpp";
        std::string argument =
            "-std=c++11 -I/home/njeneza/workspace/cpp-parser/tests/data -c "
            "/home/njeneza/workspace/cpp-parser/tests/data/test_arg.cpp";
        code::analyzer::Parser parser(filename, argument);

    }
}

