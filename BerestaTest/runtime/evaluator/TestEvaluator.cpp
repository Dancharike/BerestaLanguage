//
// Created by Denis on 10.11.2025.
//

#include "doctest/doctest.h"
#include "runtime/evaluator/Evaluator.h"
#include "frontend/parser/Expression.h"
#include "frontend/parser/Statement.h"
#include "frontend/diagnostics/Diagnostics.h"
#include "interpreter/FunctionIndex.h"
#include "runtime/environment/Environment.h"

static Evaluator make_eval(Diagnostics& diag, Environment& env)
{
    static FunctionIndex dummy_index;
    return Evaluator(env, dummy_index, "eval_test.beresta", diag);
}

static double as_double(const Value& v)
{
    return v.type == ValueType::DOUBLE ? std::get<double>(v.data) : static_cast<double>(std::get<int>(v.data));
}

static bool as_bool(const Value& v)
{
    return std::get<bool>(v.data);
}

static std::string as_string(const Value& v)
{
    return std::get<std::string>(v.data);
}

TEST_CASE("Evaluator evaluates numeric expressions correctly")
{
    Diagnostics diag;
    Environment env(&diag);
    auto evaluator = make_eval(diag, env);

    auto expr = std::make_unique<BinaryExpr>("+",
        std::make_unique<NumberExpr>(5),
        std::make_unique<NumberExpr>(3)
    );

    Value result = evaluator.eval_expression(expr.get());
    REQUIRE_EQ(result.type, ValueType::DOUBLE);
    CHECK_EQ(as_double(result), doctest::Approx(8.0));
}

TEST_CASE("Evaluator handles unary negation and logical not")
{
    Diagnostics diag;
    Environment env(&diag);
    auto evaluator = make_eval(diag, env);

    auto expr_neg = std::make_unique<UnaryExpr>('-', std::make_unique<NumberExpr>(10));
    Value result_neg = evaluator.eval_expression(expr_neg.get());
    CHECK_EQ(as_double(result_neg), doctest::Approx(-10.0));

    auto expr_not = std::make_unique<UnaryExpr>('!', std::make_unique<BoolExpr>(false));
    Value result_not = evaluator.eval_expression(expr_not.get());
    CHECK_EQ(as_bool(result_not), true);
}

TEST_CASE("Evaluator performs assignment correctly")
{
    Diagnostics diag;
    Environment env(&diag);
    auto evaluator = make_eval(diag, env);

    auto assignment = std::make_unique<Assignment>(
        true, // is_let
        "x",
        std::make_unique<NumberExpr>(42)
    );
    auto stmt = std::make_unique<AssignmentStatement>(std::move(assignment));

    evaluator.eval_statement(stmt.get());
    CHECK_EQ(as_double(env.get("x")), doctest::Approx(42.0));
}

TEST_CASE("Evaluator supports binary comparison")
{
    Diagnostics diag;
    Environment env(&diag);
    auto evaluator = make_eval(diag, env);

    auto expr = std::make_unique<BinaryExpr>(">",
        std::make_unique<NumberExpr>(7),
        std::make_unique<NumberExpr>(3)
    );

    Value result = evaluator.eval_expression(expr.get());
    REQUIRE_EQ(result.type, ValueType::BOOLEAN);
    CHECK(as_bool(result));
}

TEST_CASE("Evaluator concatenates strings correctly")
{
    Diagnostics diag;
    Environment env(&diag);
    auto evaluator = make_eval(diag, env);

    auto expr = std::make_unique<BinaryExpr>("+",
        std::make_unique<StringExpr>("Hello, "),
        std::make_unique<StringExpr>("World!")
    );

    Value result = evaluator.eval_expression(expr.get());
    REQUIRE_EQ(result.type, ValueType::STRING);
    CHECK_EQ(as_string(result), "Hello, World!");
}
