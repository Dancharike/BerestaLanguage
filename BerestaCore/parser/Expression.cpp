//
// Created by Denis on 13.07.2025.
//

#include "Expression.h"

Expression::Expression(ExpressionType type, int line, int column)
    : type(type), line(line), column(column) {}

NumberExpr::NumberExpr(int number, int line, int column)
    : Expression(ExpressionType::NUMBER, line, column), value(number) {}

NumberExpr::NumberExpr(double number, int line, int column)
    : Expression(ExpressionType::NUMBER, line, column), value(number) {}

BinaryExpr::BinaryExpr(std::string op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right, int line, int column)
    : Expression(ExpressionType::BINARY, line, column), op(std::move(op)), left(std::move(left)), right(std::move(right)) {}

VariableExpr::VariableExpr(std::string name, int line, int column)
    : Expression(ExpressionType::VARIABLE, line, column), name(std::move(name)) {}

UnaryExpr::UnaryExpr(char op, std::unique_ptr<Expression> right, int line, int column)
    : Expression(ExpressionType::UNARY, line, column), op(op), right(std::move(right)) {}

StringExpr::StringExpr(std::string val, int line, int column)
    : Expression(ExpressionType::STRING, line, column), value(std::move(val)) {}

BoolExpr::BoolExpr(bool v, int line, int column)
    : Expression(ExpressionType::BOOLEAN, line, column), value(v) {}

FunctionCallExpr::FunctionCallExpr(std::unique_ptr<Expression> expr, std::vector<std::unique_ptr<Expression>> args, int line, int column)
    : Expression(ExpressionType::FUNCTION_CALL, line, column), callee(std::move(expr)), arguments(std::move(args)) {}

ArrayLiteralExpr::ArrayLiteralExpr(std::vector<std::unique_ptr<Expression>> elems, int line, int column)
    : Expression(ExpressionType::ARRAY_LITERAL, line, column), elements(std::move(elems)) {}

IndexExpr::IndexExpr(std::unique_ptr<Expression> arr, std::unique_ptr<Expression> idx, int line, int column)
    : Expression(ExpressionType::INDEX, line, column), array(std::move(arr)), index(std::move(idx)) {}

MemberAccessExpr::MemberAccessExpr(std::unique_ptr<Expression> obj, std::string mem, int line, int column)
    : Expression(ExpressionType::MEMBER_ACCESS, line, column), object(std::move(obj)), member(std::move(mem)) {}

DictionaryLiteralExpr::DictionaryLiteralExpr(std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entr, int line, int column)
    : Expression(ExpressionType::DICTIONARY_LITERAL, line, column), entries(std::move(entr)) {}
