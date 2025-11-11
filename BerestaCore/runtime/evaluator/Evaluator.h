//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EVALUATOR_H
#define BERESTALANGUAGE_EVALUATOR_H

#pragma once
#include "api/Export.h"
#include "frontend/parser/Visitors.h"
#include "frontend/parser/Expression.h"
#include "frontend/parser/Statement.h"
#include "frontend/diagnostics/Diagnostics.h"
#include "frontend/diagnostics/BaseContext.h"
#include "runtime/value/Value.h"
#include "runtime/environment/Environment.h"
#include <unordered_map>
#include <string>
#include <stack>

class FunctionIndex;

class BERESTA_API Evaluator : public BaseContext, public ExprVisitor, public StmtVisitor
{
    public:
        Evaluator(Environment& env, FunctionIndex& index, std::string current_file, Diagnostics& _diag);

        Value eval_expression(Expression* expr);
        Value eval_statement(Statement* stmt);

    private:
        Environment& _env;
        FunctionIndex& _index;
        std::vector<std::string> _file_stack;

        [[nodiscard]] static bool is_truthy(const Value& val) ;
        void type_error(int line, const char* msg);

        Value visit_number(NumberExpr& expr) override;
        Value visit_string(StringExpr& expr) override;
        Value visit_bool(BoolExpr& expr) override;
        Value visit_variable(VariableExpr& expr) override;
        Value visit_unary(UnaryExpr& expr) override;
        Value visit_binary(BinaryExpr& expr) override;
        Value visit_call(FunctionCallExpr& expr) override;
        Value visit_array(ArrayLiteralExpr& expr) override;
        Value visit_dictionary(DictionaryLiteralExpr& expr) override;
        Value visit_struct(StructLiteralExpr& expr) override;
        Value visit_index(IndexExpr& expr) override;
        Value visit_member(MemberAccessExpr& expr) override;

        Value visit_assignment(Assignment& stmt) override;
        Value visit_assignment_statement(AssignmentStatement& stmt) override;
        Value visit_expr_stmt(ExpressionStatement& stmt) override;
        Value visit_if(IfStatement& stmt) override;
        Value visit_while(WhileStatement& stmt) override;
        Value visit_repeat(RepeatStatement& stmt) override;
        Value visit_for(ForStatement& stmt) override;
        Value visit_foreach(ForeachStatement& stmt) override;
        Value visit_block(BlockStatement& stmt) override;
        Value visit_function(FunctionStatement& stmt) override;
        Value visit_return(ReturnStatement& stmt) override;
        Value visit_index_assignment(IndexAssignment& stmt) override;
        Value visit_enum(EnumStatement& stmt) override;
        Value visit_macros(MacrosStatement& stmt) override;
        Value visit_continue(ContinueStatement& stmt) override;
        Value visit_break(BreakStatement& stmt) override;
        Value visit_switch(SwitchStatement& stmt) override;
};


#endif
