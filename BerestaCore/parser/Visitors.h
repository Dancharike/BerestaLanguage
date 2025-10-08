//
// Created by Denis on 08.10.2025.
//

#ifndef BERESTALANGUAGE_VISITORS_H
#define BERESTALANGUAGE_VISITORS_H

#pragma once
#include "../value/Value.h"

struct NumberExpr; struct StringExpr; struct BoolExpr;
struct VariableExpr; struct UnaryExpr; struct BinaryExpr;
struct FunctionCallExpr; struct ArrayLiteralExpr; struct DictionaryLiteralExpr;
struct IndexExpr; struct MemberAccessExpr;

struct Assignment; struct AssignmentStatement; struct ExpressionStatement;
struct IfStatement; struct WhileStatement; struct RepeatStatement;
struct ForStatement; struct ForeachStatement; struct BlockStatement;
struct FunctionStatement; struct ReturnStatement; struct IndexAssignment;
struct EnumStatement;

struct ExprVisitor
{
    virtual ~ExprVisitor() = default;
    virtual Value visit_number(NumberExpr& expr) = 0;
    virtual Value visit_string(StringExpr& expr) = 0;
    virtual Value visit_bool(BoolExpr& expr) = 0;
    virtual Value visit_variable(VariableExpr& expr) = 0;
    virtual Value visit_unary(UnaryExpr& expr) = 0;
    virtual Value visit_binary(BinaryExpr& expr) = 0;
    virtual Value visit_call(FunctionCallExpr& expr) = 0;
    virtual Value visit_array(ArrayLiteralExpr& expr) = 0;
    virtual Value visit_dictionary(DictionaryLiteralExpr& expr) = 0;
    virtual Value visit_index(IndexExpr& expr) = 0;
    virtual Value visit_member(MemberAccessExpr& expr) = 0;
};

struct StmtVisitor
{
    virtual ~StmtVisitor() = default;
    virtual Value visit_assignment(Assignment& stmt) = 0;
    virtual Value visit_assignment_statement(AssignmentStatement& stmt) = 0;
    virtual Value visit_expr_stmt(ExpressionStatement& stmt) = 0;
    virtual Value visit_if(IfStatement& stmt) = 0;
    virtual Value visit_while(WhileStatement& stmt) = 0;
    virtual Value visit_repeat(RepeatStatement& stmt) = 0;
    virtual Value visit_for(ForStatement& stmt) = 0;
    virtual Value visit_foreach(ForeachStatement& stmt) = 0;
    virtual Value visit_block(BlockStatement& stmt) = 0;
    virtual Value visit_function(FunctionStatement& stmt) = 0;
    virtual Value visit_return(ReturnStatement& stmt) = 0;
    virtual Value visit_index_assignment(IndexAssignment& stmt) = 0;
    virtual Value visit_enum(EnumStatement& stmt) = 0;
};


#endif //BERESTALANGUAGE_VISITORS_H
