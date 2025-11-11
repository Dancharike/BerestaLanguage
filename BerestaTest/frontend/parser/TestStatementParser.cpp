//
// Created by Denis on 10.11.2025.
//

#include "doctest/doctest.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/parser/StatementParser.h"
#include "frontend/diagnostics/Diagnostics.h"

static std::vector<std::unique_ptr<Statement>> parse_code(const std::string& code, Diagnostics& diag)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    StatementParser parser(tokens, "test.beresta", diag);
    return parser.parse();
}

TEST_CASE("StatementParser: Assignments")
{
    Diagnostics diag;

    auto s1 = parse_code("let x = 5 + 3;", diag);
    REQUIRE_EQ(s1.size(), 1);
    CHECK_EQ(s1[0]->type, StatementType::ASSIGNMENT_STATEMENT);

    auto s2 = parse_code("x = 10;", diag);
    REQUIRE_EQ(s2.size(), 1);
    CHECK_EQ(s2[0]->type, StatementType::ASSIGNMENT_STATEMENT);
}

TEST_CASE("StatementParser: Conditionals")
{
    Diagnostics diag;

    auto s1 = parse_code("if (x > 5) let y = 10;", diag);
    REQUIRE_EQ(s1.size(), 1);
    CHECK_EQ(s1[0]->type, StatementType::IF);

    auto s2 = parse_code("if (x) let y = 1; else let y = 2;", diag);
    REQUIRE_EQ(s2.size(), 1);
    CHECK_EQ(s2[0]->type, StatementType::IF);
}

TEST_CASE("StatementParser: Loops")
{
    Diagnostics diag;

    auto s1 = parse_code("while (x < 10) x = x + 1;", diag);
    CHECK_EQ(s1[0]->type, StatementType::WHILE);

    auto s2 = parse_code("repeat (5) { let i = 0; }", diag);
    CHECK_EQ(s2[0]->type, StatementType::REPEAT);

    auto s3 = parse_code("for (let i = 0; i < 10; i = i + 1) { }", diag);
    CHECK_EQ(s3[0]->type, StatementType::FOR);

    auto s4 = parse_code("foreach (item in [1,2,3]) { }", diag);
    CHECK_EQ(s4[0]->type, StatementType::FOREACH);
}

TEST_CASE("StatementParser: Functions and returns")
{
    Diagnostics diag;

    auto s1 = parse_code("function add(a, b) { return a + b; }", diag);
    CHECK_EQ(s1[0]->type, StatementType::FUNCTION);

    auto s2 = parse_code("private function foo() { }", diag);
    CHECK_EQ(s2[0]->type, StatementType::FUNCTION);

    auto s3 = parse_code("return 5 + 2;", diag);
    CHECK_EQ(s3[0]->type, StatementType::RETURN);
}

TEST_CASE("StatementParser: Blocks and control statements")
{
    Diagnostics diag;

    auto s1 = parse_code("{ break; continue; }", diag);
    auto block = dynamic_cast<BlockStatement*>(s1[0].get());
    REQUIRE(block);
    CHECK_EQ(block->statements[0]->type, StatementType::BREAK);
    CHECK_EQ(block->statements[1]->type, StatementType::CONTINUE);

    auto s2 = parse_code("{ }", diag);
    CHECK_EQ(s2[0]->type, StatementType::BLOCK);

    auto s3 = parse_code("{ let a = 1; let b = 2; }", diag);
    auto block2 = dynamic_cast<BlockStatement*>(s3[0].get());
    REQUIRE(block2);
    CHECK_EQ(block2->statements.size(), 2);
}

TEST_CASE("StatementParser: Indexes, enums, macros, switch")
{
    Diagnostics diag;

    auto s1 = parse_code("arr[0] = 5;", diag);
    CHECK_EQ(s1[0]->type, StatementType::INDEX_ASSIGNMENT);

    auto s2 = parse_code("arr[1][2] = 3;", diag);
    CHECK_EQ(s2[0]->type, StatementType::INDEX_ASSIGNMENT);

    auto s3 = parse_code("enum Colors { Red, Green, Blue }", diag);
    CHECK_EQ(s3[0]->type, StatementType::ENUM);

    auto s4 = parse_code("enum Numbers { One = 1, Two = 2 }", diag);
    CHECK_EQ(s4[0]->type, StatementType::ENUM);

    auto s5 = parse_code("#macros MAX = 10;", diag);
    CHECK_EQ(s5[0]->type, StatementType::MACROS);

    auto s6 = parse_code("switch(x) { case 1: break; default: break; }", diag);
    CHECK_EQ(s6[0]->type, StatementType::SWITCH);
}

TEST_CASE("StatementParser: Error handling")
{
    Diagnostics diag;
    parse_code("let x = 5", diag);
    CHECK(diag.has_error());
}
