//
// Created by Denis on 08.07.2025.
//

#include <unordered_map>
#include <utility>
#include "Parser.h"
#include "ExpressionParser.h"

Parser::Parser(const std::vector<Token>& tokens, std::string current_file, Diagnostics& diag)
    : BaseContext(diag, std::move(current_file)), tokens(tokens)
    {
        _file_stack.push_back(_current_file);
    }

Token Parser::peek() const
{
    return position < tokens.size() ? tokens[position] : Token(TokenType::END_OF_FILE, "", position);
}

Token Parser::advance() {return tokens[position++];}

bool Parser::match(TokenType type)
{
    if(peek().type == type) {advance(); return true;}
    return false;
}

std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> statements;

    while(peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) {statements.push_back(std::move(stmt));}
    }

    return statements;
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    if(peek().type == TokenType::LET) {return parse_assignment();}
    if(peek().type == TokenType::IF) {return parse_if_statement();}
    if(peek().type == TokenType::WHILE) {return parse_while_statement();}
    if(peek().type == TokenType::REPEAT) {return parse_repeat_statement();}
    if(peek().type == TokenType::FOR) {return parse_for_statement();}
    if(peek().type == TokenType::FOREACH) {return parse_foreach_statement();}
    if(peek().type == TokenType::ENUM) {return parse_enum_statement();}
    if(peek().type == TokenType::LEFT_BRACE) {return parse_block();}
    if(peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::LEFT_BRACKET) {return parse_index_assignment();}
    if(peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::EQUALS)
    {
        Token id = peek();
        auto a = parse_assignment();
        return std::make_unique<AssignmentStatement>(std::move(a), id.line, id.column);
    }
    if(peek().type == TokenType::PUBLIC || peek().type == TokenType::PRIVATE) {return parse_function_statement();}
    if(peek().type == TokenType::FUNCTION) {return parse_function_statement();}
    if(peek().type == TokenType::RETURN) {return parse_return_statement();}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!expr) {_diag.error("Failed to parse expression", current_file(), peek().line); return nullptr;}

    Token start_token = tokens[position - 1];

    if(!match(TokenType::SEMICOLON))
    {
        const Token& prev = tokens[position - 1];
        _diag.error("Expected ';' after expression", current_file(), prev.line); return nullptr;
    }

    return std::make_unique<ExpressionStatement>(std::move(expr), start_token.line, start_token.column);
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    bool is_let = match(TokenType::LET);

    Token name_token = advance();
    if(name_token.type != TokenType::IDENTIFIER) {_diag.error("Expected variable name", current_file(), name_token.line); return nullptr;}

    if(!match(TokenType::EQUALS)) {_diag.error("Expected '=' after variable name", current_file(), name_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!expr) {_diag.error("Failed to parse expression after '='", current_file(), name_token.line); return nullptr;}

    if(!match(TokenType::SEMICOLON)) {_diag.error("Expected ';' after expression", current_file(), name_token.line); return nullptr;}

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr), name_token.line, name_token.column);
}

std::unique_ptr<Assignment> Parser::parse_assignment_expression()
{
    bool is_let = match(TokenType::LET);

    Token name_token = advance();
    if(name_token.type != TokenType::IDENTIFIER) {_diag.error("Expected variable name", current_file(), name_token.line); return nullptr;}

    if(!match(TokenType::EQUALS)) {_diag.error("Expected '=' after variable name", current_file(), name_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!expr) {_diag.error("Failed to parse expression after '='", current_file(), name_token.line); return nullptr;}

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr), name_token.line, name_token.column);
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    Token if_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '(' after 'if'", current_file(), if_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto condition = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!condition) {_diag.error("Failed to parse condition in 'if' statement", current_file(), if_token.line); return nullptr;}

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after 'if' condition", current_file(), if_token.line); return nullptr;}

    auto then_branch = parse_statement();
    std::unique_ptr<Statement> else_branch = nullptr;

    if(match(TokenType::ELSE)) {else_branch = parse_statement();}

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch), if_token.line, if_token.column);
}

std::unique_ptr<Statement> Parser::parse_while_statement()
{
    Token while_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '(' after 'while'", current_file(), while_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto condition = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!condition) {_diag.error("Failed to parse condition in 'while' statement", current_file(), while_token.line); return nullptr;}

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after 'while' condition", current_file(), while_token.line); return nullptr;}

    auto body = parse_statement();
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body), while_token.line, while_token.column);
}

std::unique_ptr<Statement> Parser::parse_repeat_statement()
{
    Token repeat_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '(' after 'repeat'", current_file(), repeat_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto count_expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!count_expr) {_diag.error("Failed to parse repeat count expression", current_file(), repeat_token.line); return nullptr;}

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after 'repeat' condition", current_file(), repeat_token.line); return nullptr;}

    auto body = parse_statement();
    return std::make_unique<RepeatStatement>(std::move(count_expr), std::move(body), repeat_token.line, repeat_token.column);
}

std::unique_ptr<Statement> Parser::parse_for_statement()
{
    Token for_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '(' after 'for'", current_file(), for_token.line); return nullptr;}

    std::unique_ptr<Statement> initializer = nullptr;
    if(peek().type != TokenType::SEMICOLON) {initializer = parse_optional_assignment_or_expression();}

    if(!match(TokenType::SEMICOLON)) {_diag.error("Expected ';' after initializer", current_file(), for_token.line); return nullptr;}

    ExpressionParser cond_parser(tokens, position, _current_file, _diag);
    auto condition = cond_parser.parse_expression();
    position = cond_parser.get_position();

    if(!condition) {_diag.error("Failed to parse condition in 'for' statement", current_file(), for_token.line); return nullptr;}

    if(!match(TokenType::SEMICOLON)) {_diag.error("Expected ';' after condition", current_file(), for_token.line); return nullptr;}

    std::unique_ptr<Statement> increment = nullptr;
    if(peek().type != TokenType::RIGHT_PAREN) {increment = parse_optional_assignment_or_expression();}

    if(peek().type != TokenType::RIGHT_PAREN && !increment) {_diag.error("Failed to parse increment expression in 'for' statement", current_file(), for_token.line); return nullptr;}

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after increment", current_file(), for_token.line); return nullptr;}

    auto body = parse_statement();
    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(increment), std::move(body), for_token.line, for_token.column);
}

std::unique_ptr<Statement> Parser::parse_foreach_statement()
{
    Token foreach_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '(' after 'foreach'", current_file(), foreach_token.line); return nullptr;}

    if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected variable name in foreach", current_file(), foreach_token.line); return nullptr;}

    Token var_token = advance();
    std::string var_name = var_token.value;

    if(!match(TokenType::IN)) {_diag.error("Expected 'in' after foreach variable", current_file(), foreach_token.line); return nullptr;}

    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto iterable = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!iterable) {_diag.error("Failed to parse iterable expression in 'foreach' statement", current_file(), foreach_token.line); return nullptr;}

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after foreach iterable", current_file(), foreach_token.line); return nullptr;}

    auto body = parse_statement();
    return std::make_unique<ForeachStatement>(std::move(var_name), std::move(iterable), std::move(body), foreach_token.line, foreach_token.column);
}

std::unique_ptr<Statement> Parser::parse_block()
{
    Token brace = advance();
    std::vector<std::unique_ptr<Statement>> statements;

    while(peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) {statements.push_back(std::move(stmt));}
    }

    if(!match(TokenType::RIGHT_BRACE)) {_diag.error("Expected '}' at end of block", current_file(), brace.line); return nullptr;}

    return std::make_unique<BlockStatement>(std::move(statements), brace.line, brace.column);
}

std::unique_ptr<Statement> Parser::parse_optional_assignment_or_expression()
{
    if(peek().type == TokenType::LET || (peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::EQUALS))
    {
        Token start = peek();
        auto assignment = parse_assignment_expression();
        return std::make_unique<AssignmentStatement>(std::move(assignment), start.line, start.column);
    }

    if(peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::LEFT_BRACKET) {return parse_index_assignment_expression();}

    Token expr_start = peek();
    ExpressionParser expr_parser(tokens, position, _current_file, _diag);
    auto expr = expr_parser.parse_expression();

    if(!expr) {_diag.error("Failed to parse expression in for-loop clause", current_file(), peek().line); return nullptr;}

    position = expr_parser.get_position();
    return std::make_unique<ExpressionStatement>(std::move(expr), expr_start.line, expr_start.column);
}

std::unique_ptr<Statement> Parser::parse_function_statement()
{
    FunctionVisibility visibility = FunctionVisibility::PUBLIC;

    if(match(TokenType::PUBLIC)) {visibility = FunctionVisibility::PUBLIC;}
    else if(match(TokenType::PRIVATE)) {visibility = FunctionVisibility::PRIVATE;}

    Token func_token = advance();

    if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected function name", current_file(), func_token.line); return nullptr;}

    Token name_token = advance();
    std::string name = name_token.value;

    if(!match(TokenType::LEFT_PAREN)) {_diag.error("Expected '('", current_file(), func_token.line); return nullptr;}

    std::vector<std::string> params;
    if(peek().type != TokenType::RIGHT_PAREN)
    {
        do
        {
            if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected parameter name", current_file(), func_token.line); return nullptr;}
            params.push_back(advance().value);
        } while(match(TokenType::COMMA));
    }

    if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')'", current_file(), func_token.line); return nullptr;}

    auto body = parse_block();
    return std::make_unique<FunctionStatement>(visibility, name, std::move(params), std::move(body), func_token.line, func_token.column);
}

std::unique_ptr<Statement> Parser::parse_return_statement()
{
    Token ret_token = advance();
    std::unique_ptr<Expression> val;

    if(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        ExpressionParser expr_parser(tokens, position, _current_file, _diag);
        val = expr_parser.parse_expression();
    }

    if(!match(TokenType::SEMICOLON)) {_diag.error("Missing ';' after return", current_file(), ret_token.line);}

    return std::make_unique<ReturnStatement>(std::move(val), ret_token.line, ret_token.column);
}

std::unique_ptr<Statement> Parser::parse_index_assignment()
{
    Token name_token = advance();
    std::unique_ptr<Expression> target = std::make_unique<VariableExpr>(name_token.value, name_token.line, name_token.column);

    do
    {
        if(!match(TokenType::LEFT_BRACKET)) {break;}

        Token lb = tokens[position - 1];
        ExpressionParser ep(tokens, position, _current_file, _diag);
        auto idx = ep.parse_expression();
        position = ep.get_position();

        if(!idx) {_diag.error("Failed to parse index expression", current_file(), name_token.line); return nullptr;}

        if(!match(TokenType::RIGHT_BRACKET)) {_diag.error("Expected ']' after index expression", current_file(), name_token.line); return nullptr;}

        target = std::make_unique<IndexExpr>(std::move(target), std::move(idx), lb.line, lb.column);
    } while(peek().type == TokenType::LEFT_BRACKET);

    if(!match(TokenType::EQUALS)) {_diag.error("Expected '=' after indexed value", current_file(), name_token.line); return nullptr;}

    ExpressionParser epv(tokens, position, _current_file, _diag);
    auto val = epv.parse_expression();
    position = epv.get_position();

    if(!val) {_diag.error("Failed to parse assigned value after index", current_file(), peek().line); return nullptr;}

    if(!match(TokenType::SEMICOLON)) {_diag.error("Expected ';' after assignment", current_file(), name_token.line); return nullptr;}

    if(dynamic_cast<IndexExpr*>(target.get()) == nullptr) {_diag.error("Indexed assignment requires at least one []", current_file(), name_token.line); return nullptr;}

    return std::make_unique<IndexAssignment>(std::move(target), std::move(val), name_token.line, name_token.column);
}

std::unique_ptr<Statement> Parser::parse_index_assignment_expression()
{
    Token name_token = advance();
    std::unique_ptr<Expression> target = std::make_unique<VariableExpr>(name_token.value, name_token.line, name_token.column);

    do
    {
        if(!match(TokenType::LEFT_BRACKET)) {break;}

        Token lb = tokens[position - 1];
        ExpressionParser ep(tokens, position, _current_file, _diag);
        auto idx = ep.parse_expression();
        position = ep.get_position();

        if(!idx) {_diag.error("Failed to parse index expression", current_file(), name_token.line); return nullptr;}

        if(!match(TokenType::RIGHT_BRACKET)) {_diag.error("Expected ']' after index expression", current_file(), name_token.line); return nullptr;}

        target = std::make_unique<IndexExpr>(std::move(target), std::move(idx), lb.line, lb.column);
    } while(peek().type == TokenType::LEFT_BRACKET);

    if(!match(TokenType::EQUALS)) {_diag.error("Expected '=' after indexed value", current_file(), name_token.line); return nullptr;}

    ExpressionParser epv(tokens, position, _current_file, _diag);
    auto val = epv.parse_expression();
    position = epv.get_position();

    if(!val) {_diag.error("Failed to parse assigned value after index", current_file(), peek().line); return nullptr;}

    if(dynamic_cast<IndexExpr*>(target.get()) == nullptr) {_diag.error("Indexed assignment requires at least one []", current_file(), name_token.line); return nullptr;}

    return std::make_unique<IndexAssignment>(std::move(target), std::move(val), name_token.line, name_token.column);
}

std::unique_ptr<Statement> Parser::parse_enum_statement()
{
    Token enum_token = advance();

    if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected enum name", current_file(), enum_token.line); return nullptr;}

    Token enum_name_token = advance();
    std::string enum_name = enum_name_token.value;

    if(!match(TokenType::LEFT_BRACE)) {_diag.error("Expected '{' after enum name", current_file(), enum_token.line); return nullptr;}

    std::unordered_map<std::string, int> members;
    int auto_value = 0;

    while(peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected enum member name", current_file(), enum_token.line); return nullptr;}

        Token member_token = advance();
        std::string member_name = member_token.value;
        int member_value = auto_value;

        if(match(TokenType::EQUALS))
        {
            if(peek().type != TokenType::NUMBER) {_diag.error("Expected number after '=' in enum member", current_file(), member_token.line); return nullptr;}

            Token num_token = advance();
            const std::string& num = num_token.value;
            member_value = (num.find('.') != std::string::npos) ? static_cast<int>(std::stod(num)) : std::stoi(num);
        }

        members[member_name] = member_value;
        auto_value = member_value + 1;

        if(peek().type == TokenType::COMMA) {advance();}
    }

    if(!match(TokenType::RIGHT_BRACE)) {_diag.error("Expected '}' after enum members", current_file(), enum_token.line); return nullptr;}
    return std::make_unique<EnumStatement>(enum_name, std::move(members), enum_token.line, enum_token.column);
}
