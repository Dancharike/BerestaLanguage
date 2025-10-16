//
// Created by Denis on 13.07.2025.
//

#include "Expression.h"
#include "Visitors.h"
#include "frontend/parser/ExpressionFactory.h"

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


Value NumberExpr::accept(ExprVisitor& val)            {return val.visit_number(*this);}
Value StringExpr::accept(ExprVisitor& val)            {return val.visit_string(*this);}
Value BoolExpr::accept(ExprVisitor& val)              {return val.visit_bool(*this);}
Value VariableExpr::accept(ExprVisitor& val)          {return val.visit_variable(*this);}
Value UnaryExpr::accept(ExprVisitor& val)             {return val.visit_unary(*this);}
Value BinaryExpr::accept(ExprVisitor& val)            {return val.visit_binary(*this);}
Value FunctionCallExpr::accept(ExprVisitor& val)      {return val.visit_call(*this);}
Value ArrayLiteralExpr::accept(ExprVisitor& val)      {return val.visit_array(*this);}
Value DictionaryLiteralExpr::accept(ExprVisitor& val) {return val.visit_dictionary(*this);}
Value IndexExpr::accept(ExprVisitor& val)             {return val.visit_index(*this);}
Value MemberAccessExpr::accept(ExprVisitor& val)      {return val.visit_member(*this);}

double NumberExpr::get_number_value() const
{
    if(value.type == ValueType::DOUBLE)  {return std::get<double>(value.data);}
    if(value.type == ValueType::INTEGER) {return static_cast<double>(std::get<int>(value.data));}
    return 0.0;
}

std::string StringExpr::get_operator_value() const {return value;}

char StringExpr::get_operator_char() const {return value.empty() ? '?' : value[0];}

// этот код нигде не используется, но нужен при компиляции, ещё до момента, как файл .beresta начал читаться, чтобы в нужный момент фабрика получила ссылку на способ сборки объекта
[[maybe_unused]] static bool reg_binary_expr = []
{
    ExpressionFactory::instance().register_type("binary", [](std::vector<std::unique_ptr<Expression>>&& args) -> std::unique_ptr<Expression>
    {
        if(args.size() != 3) {return nullptr;}
        std::string op = args[1] ? args[1]->get_operator_value() : "";
        return std::make_unique<BinaryExpr>(op, std::move(args[0]), std::move(args[2]), 0, 0);
    });
    return true;
}();

[[maybe_unused]] static bool reg_unary_expr = []
{
    ExpressionFactory::instance().register_type("unary", [](std::vector<std::unique_ptr<Expression>>&& args) -> std::unique_ptr<Expression>
    {
        if(args.size() != 2) {return nullptr;}
        char op = args[0] ? args[0]->get_operator_char() : '?';
        return std::make_unique<UnaryExpr>(op, std::move(args[1]), 0, 0);
    });
    return true;
}();
