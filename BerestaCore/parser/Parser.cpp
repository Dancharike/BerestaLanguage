//
// Created by Denis on 08.07.2025.
//

#include <unordered_map>
#include "Parser.h"
#include "ExpressionParser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

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

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!expr) {std::cerr << "[ERROR] Failed to parse expression\n"; return nullptr;}

    Token start_token = tokens[position - 1];
    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after expression\n"; return nullptr;}

    return std::make_unique<ExpressionStatement>(std::move(expr), start_token.line, start_token.column);
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    bool is_let = match(TokenType::LET);

    Token name_token = advance();
    if(name_token.type != TokenType::IDENTIFIER) {std::cerr << "Expected variable name\n"; return nullptr;}

    if(!match(TokenType::EQUALS)) {std::cerr << "Expected '=' after variable name\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after expression\n"; return nullptr;}

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr), name_token.line, name_token.column);
}

std::unique_ptr<Assignment> Parser::parse_assignment_expression()
{
    bool is_let = match(TokenType::LET);

    Token name_token = advance();
    if(name_token.type != TokenType::IDENTIFIER) {std::cerr << "Expected variable name\n"; return nullptr;}

    if(!match(TokenType::EQUALS)) {std::cerr << "Expected '=' after variable name\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();
    position = expr_parser.get_position();

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr), name_token.line, name_token.column);
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    Token if_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'if'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto condition = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'if' condition\n"; return nullptr;}

    auto then_branch = parse_statement();
    std::unique_ptr<Statement> else_branch = nullptr;

    if(match(TokenType::ELSE)) {else_branch = parse_statement();}

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch), if_token.line, if_token.column);
}

std::unique_ptr<Statement> Parser::parse_while_statement()
{
    Token while_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'while'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto condition = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'while' condition\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body), while_token.line, while_token.column);
}

std::unique_ptr<Statement> Parser::parse_repeat_statement()
{
    Token repeat_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'repeat'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto count_expr = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'repeat' condition\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<RepeatStatement>(std::move(count_expr), std::move(body), repeat_token.line, repeat_token.column);
}

std::unique_ptr<Statement> Parser::parse_for_statement()
{
    Token for_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'for'\n"; return nullptr;}

    std::unique_ptr<Statement> initializer = nullptr;
    if(peek().type != TokenType::SEMICOLON) {initializer = parse_optional_assignment_or_expression();}

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after initializer\n"; return nullptr;}

    ExpressionParser cond_parser(tokens, position);
    auto condition = cond_parser.parse_expression();
    position = cond_parser.get_position();

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after condition\n"; return nullptr;}

    std::unique_ptr<Statement> increment = nullptr;
    if(peek().type != TokenType::RIGHT_PAREN) {increment = parse_optional_assignment_or_expression();}

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after increment\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<ForStatement>(std::move(initializer), std::move(condition), std::move(increment), std::move(body), for_token.line, for_token.column);
}

std::unique_ptr<Statement> Parser::parse_foreach_statement()
{
    Token foreach_token = advance();

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "[ERROR] Expected '(' after 'foreach'\n"; return nullptr;}

    if(peek().type != TokenType::IDENTIFIER) {std::cerr << "[ERROR] Expected variable name in foreach\n"; return nullptr;}

    Token var_token = advance();
    std::string var_name = var_token.value;

    if(!match(TokenType::IN)) {std::cerr << "[ERROR] Expected 'in' after foreach variable\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto iterable = expr_parser.parse_expression();
    position = expr_parser.get_position();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "[ERROR] Expected ')' after foreach iterable\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<ForeachStatement>(std::move(var_name), std::move(iterable), std::move(body), foreach_token.line, foreach_token.column);
}

std::unique_ptr<Statement> Parser::parse_block()
{
    Token brace = advance();
    std::vector<std::unique_ptr<Statement>> statements;

    while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) {statements.push_back(std::move(stmt));}
    }

    if(!match(TokenType::RIGHT_BRACE)) {std::cerr << "Expected '}' at end of block\n"; return nullptr;}

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
    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!expr) {std::cerr << "[ERROR] Failed to parse expression in for-loop clause\n"; return nullptr;}

    position = expr_parser.get_position();
    return std::make_unique<ExpressionStatement>(std::move(expr), expr_start.line, expr_start.column);
}

std::unique_ptr<Statement> Parser::parse_function_statement()
{
    FunctionVisibility visibility = FunctionVisibility::PUBLIC;

    if(match(TokenType::PUBLIC)) {visibility = FunctionVisibility::PUBLIC;}
    else if(match(TokenType::PRIVATE)) {visibility = FunctionVisibility::PRIVATE;}

    Token func_token = advance();

    if(peek().type != TokenType::IDENTIFIER) {std::cerr << "Expected function name\n"; return nullptr;}

    Token name_token = advance();
    std::string name = name_token.value;

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '('\n"; return nullptr;}

    std::vector<std::string> params;
    if(peek().type != TokenType::RIGHT_PAREN)
    {
        do
        {
            if(peek().type != TokenType::IDENTIFIER) {std::cerr << "Expected parameter name\n"; return nullptr;}
            params.push_back(advance().value);
        } while(match(TokenType::COMMA));
    }

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')'\n"; return nullptr;}

    auto body = parse_block();
    return std::make_unique<FunctionStatement>(visibility, name, std::move(params), std::move(body), func_token.line, func_token.column);
}

std::unique_ptr<Statement> Parser::parse_return_statement()
{
    Token ret_token = advance();
    std::unique_ptr<Expression> val;

    if(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        ExpressionParser expr_parser(tokens, position);
        val = expr_parser.parse_expression();
    }

    if(!match(TokenType::SEMICOLON)) {std::cerr << "[ERROR] Missing ';' after return\n";}

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
        ExpressionParser ep(tokens, position);
        auto idx = ep.parse_expression();
        position = ep.get_position();

        if(!match(TokenType::RIGHT_BRACKET)) {std::cerr << "Expected ']' after index expression\n"; return nullptr;}

        target = std::make_unique<IndexExpr>(std::move(target), std::move(idx), lb.line, lb.column);
    } while(peek().type == TokenType::LEFT_BRACKET);

    if(!match(TokenType::EQUALS)) {std::cerr << "Expected '=' after indexed value\n"; return nullptr;}

    ExpressionParser epv(tokens, position);
    auto val = epv.parse_expression();
    position = epv.get_position();

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after assignment\n"; return nullptr;}

    if(dynamic_cast<IndexExpr*>(target.get()) == nullptr) {std::cerr << "[ERROR] Indexed assignment requires at least one []\n"; return nullptr;}

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
        ExpressionParser ep(tokens, position);
        auto idx = ep.parse_expression();
        position = ep.get_position();

        if(!match(TokenType::RIGHT_BRACKET)) {std::cerr << "Expected ']' after index expression\n"; return nullptr;}

        target = std::make_unique<IndexExpr>(std::move(target), std::move(idx), lb.line, lb.column);
    } while(peek().type == TokenType::LEFT_BRACKET);

    if(!match(TokenType::EQUALS)) {std::cerr << "Expected '=' after indexed value\n"; return nullptr;}

    ExpressionParser epv(tokens, position);
    auto val = epv.parse_expression();
    position = epv.get_position();

    if(dynamic_cast<IndexExpr*>(target.get()) == nullptr) {std::cerr << "[ERROR] Indexed assignment requires at least one []\n"; return nullptr;}

    return std::make_unique<IndexAssignment>(std::move(target), std::move(val), name_token.line, name_token.column);
}

std::unique_ptr<Statement> Parser::parse_enum_statement()
{
    Token enum_token = advance();

    if(peek().type != TokenType::IDENTIFIER) {std::cerr << "[ERROR] Expected enum name\n"; return nullptr;}

    Token enum_name_token = advance();
    std::string enum_name = enum_name_token.value;

    if(!match(TokenType::LEFT_BRACE)) {std::cerr << "[ERROR] Expected '{' after enum name\n"; return nullptr;}

    std::unordered_map<std::string, int> members;
    int auto_value = 0;

    while(peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        if(peek().type != TokenType::IDENTIFIER) {std::cerr << "[ERROR] Expected enum member name\n"; return nullptr;}

        Token member_token = advance();
        std::string member_name = member_token.value;
        int member_value = auto_value;

        if(match(TokenType::EQUALS))
        {
            if(peek().type != TokenType::NUMBER) {std::cerr << "[ERROR] Expected number after '=' in enum member\n"; return nullptr;}

            Token num_token = advance();
            const std::string& num = num_token.value;
            member_value = (num.find('.') != std::string::npos) ? static_cast<int>(std::stod(num)) : std::stoi(num);
        }

        members[member_name] = member_value;
        auto_value = member_value + 1;

        if(peek().type == TokenType::COMMA) {advance();}
    }

    if(!match(TokenType::RIGHT_BRACE)) {std::cerr << "[ERROR] Expected '}' after enum members\n"; return nullptr;}
    return std::make_unique<EnumStatement>(enum_name, std::move(members), enum_token.line, enum_token.column);
}
