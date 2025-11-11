//
// Created by Denis on 10.11.2025.
//

#include "doctest/doctest.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/parser/ExpressionParser.h"
#include "frontend/diagnostics/Diagnostics.h"

static std::unique_ptr<Expression> parse_expr(const std::string& code, Diagnostics& diag)
{
    Lexer lexer(code);
    auto tokens = lexer.tokenize();
    size_t pos = 0;
    ExpressionParser parser(tokens, pos, "expr_test.beresta", diag);
    return parser.parse_expression();
}

TEST_CASE("ExpressionParser parses numeric literal")
{
    Diagnostics diag;
    auto expr = parse_expr("42", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::NUMBER);
}

TEST_CASE("ExpressionParser parses string literal")
{
    Diagnostics diag;
    auto expr = parse_expr("\"Hello\"", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::STRING);
}

TEST_CASE("ExpressionParser parses boolean literals")
{
    Diagnostics diag_true;
    auto expr_true = parse_expr("true", diag_true);
    REQUIRE_NE(expr_true, nullptr);
    CHECK_EQ(expr_true->type, ExpressionType::BOOLEAN);

    Diagnostics diag_false;
    auto expr_false = parse_expr("false", diag_false);
    REQUIRE_NE(expr_false, nullptr);
    CHECK_EQ(expr_false->type, ExpressionType::BOOLEAN);
}

TEST_CASE("ExpressionParser parses simple binary addition")
{
    Diagnostics diag;
    auto expr = parse_expr("5 + 3", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser parses nested arithmetic expression")
{
    Diagnostics diag;
    auto expr = parse_expr("1 + 2 * 3 - 4 / 2", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser handles parentheses correctly")
{
    Diagnostics diag;
    auto expr = parse_expr("(5 + 3) * 2", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser parses unary negation")
{
    Diagnostics diag;
    auto expr = parse_expr("-x", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::UNARY);
}

TEST_CASE("ExpressionParser parses logical and/or expressions")
{
    Diagnostics diag;
    auto expr = parse_expr("true and false or true", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser parses comparison expressions")
{
    Diagnostics diag;
    auto expr = parse_expr("a > 5", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);

    auto expr2 = parse_expr("b <= 10", diag);
    REQUIRE_NE(expr2, nullptr);
    CHECK_EQ(expr2->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser parses equality expressions")
{
    Diagnostics diag;
    auto expr = parse_expr("x == y", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::BINARY);

    auto expr2 = parse_expr("x != y", diag);
    REQUIRE_NE(expr2, nullptr);
    CHECK_EQ(expr2->type, ExpressionType::BINARY);
}

TEST_CASE("ExpressionParser parses array literals")
{
    Diagnostics diag;
    auto expr = parse_expr("[1, 2, 3]", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::ARRAY_LITERAL);
}

TEST_CASE("ExpressionParser parses index access")
{
    Diagnostics diag;
    auto expr = parse_expr("arr[0]", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::INDEX);
}

TEST_CASE("ExpressionParser parses nested index access")
{
    Diagnostics diag;
    auto expr = parse_expr("arr[1][2]", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::INDEX);
}

TEST_CASE("ExpressionParser parses function call without args")
{
    Diagnostics diag;
    auto expr = parse_expr("foo()", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::FUNCTION_CALL);
}

TEST_CASE("ExpressionParser parses function call with args")
{
    Diagnostics diag;
    auto expr = parse_expr("foo(1, 2, 3)", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::FUNCTION_CALL);
}

TEST_CASE("ExpressionParser parses member access")
{
    Diagnostics diag;
    auto expr = parse_expr("player.position.x", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::MEMBER_ACCESS);
}

TEST_CASE("ExpressionParser parses struct literal")
{
    Diagnostics diag;
    auto expr = parse_expr("{x, y, z}", diag);
    REQUIRE_NE(expr, nullptr);
    CHECK_EQ(expr->type, ExpressionType::STRUCT_LITERAL);
}

TEST_CASE("ExpressionParser reports error for missing closing parenthesis")
{
    Diagnostics diag;
    auto expr = parse_expr("(5 + 2", diag);
    CHECK_EQ(expr, nullptr);
    CHECK(diag.has_error());
}

TEST_CASE("ExpressionParser reports error for unexpected token")
{
    Diagnostics diag;
    auto expr = parse_expr("@oops", diag);
    CHECK_EQ(expr, nullptr);
    CHECK(diag.has_error());
}
