//
// Created by Denis on 08.07.2025.
//

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
    if(peek().type == TokenType::LEFT_BRACE) {return parse_block();}
    if(peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::EQUALS) {return parse_assignment();}
    if(peek().type == TokenType::PUBLIC || peek().type == TokenType::PRIVATE) {return parse_function_statement();}
    if(peek().type == TokenType::FUNCTION) return {parse_function_statement()};
    if(peek().type == TokenType::RETURN) return {parse_return_statement()};

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!expr) {std::cerr << "[ERROR] Failed to parse expression\n"; return nullptr;}

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after expression\n"; return nullptr;}

    return std::make_unique<ExpressionStatement>(std::move(expr));
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

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr));
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

    return std::make_unique<Assignment>(is_let, name_token.value, std::move(expr));
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    match(TokenType::IF);

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'if'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto condition = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'if' condition\n"; return nullptr;}

    auto then_branch = parse_statement();
    std::unique_ptr<Statement> else_branch = nullptr;

    if(match(TokenType::ELSE)) {else_branch = parse_statement();}

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch));
}

std::unique_ptr<Statement> Parser::parse_while_statement()
{
    match(TokenType::WHILE);

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'while'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto condition = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'while' condition\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<WhileStatement>(std::move(condition), std::move(body));
}

std::unique_ptr<Statement> Parser::parse_repeat_statement()
{
    match(TokenType::REPEAT);

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'repeat'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto count_expr = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'repeat' condition\n"; return nullptr;}

    auto body = parse_statement();
    return std::make_unique<RepeatStatement>(std::move(count_expr), std::move(body));
}

std::unique_ptr<Statement> Parser::parse_for_statement()
{
    match(TokenType::FOR);

    if (!match(TokenType::LEFT_PAREN)) {
        std::cerr << "Expected '(' after 'for'\n";
        return nullptr;
    }

    std::unique_ptr<Statement> initializer = nullptr;
    if (peek().type != TokenType::SEMICOLON) {
        initializer = parse_optional_assignment_or_expression();
    }
    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after initializer\n";
        return nullptr;
    }

    ExpressionParser cond_parser(tokens, position);
    auto condition = cond_parser.parse_expression();
    position = cond_parser.get_position();

    if (!match(TokenType::SEMICOLON)) {
        std::cerr << "Expected ';' after condition\n";
        return nullptr;
    }

    std::unique_ptr<Statement> increment = nullptr;
    if (peek().type != TokenType::RIGHT_PAREN) {
        increment = parse_optional_assignment_or_expression();
    }

    if (!match(TokenType::RIGHT_PAREN)) {
        std::cerr << "Expected ')' after increment\n";
        return nullptr;
    }

    auto body = parse_statement();
    return std::make_unique<ForStatement>(
            std::move(initializer),
            std::move(condition),
            std::move(increment),
            std::move(body)
    );
}

std::unique_ptr<Statement> Parser::parse_block()
{
    if(!match(TokenType::LEFT_BRACE)) {std::cerr << "Expected '{' at start of block\n"; return nullptr;}

    std::vector<std::unique_ptr<Statement>> statements;

    while (peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) {statements.push_back(std::move(stmt));}
    }

    if(!match(TokenType::RIGHT_BRACE)) {std::cerr << "Expected '}' at end of block\n"; return nullptr;}

    return std::make_unique<BlockStatement>(std::move(statements));
}

std::unique_ptr<Statement> Parser::parse_optional_assignment_or_expression()
{
    if(peek().type == TokenType::LET || peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::EQUALS)
    {
        auto assignment = parse_assignment_expression();
        return std::make_unique<AssignmentStatement>(std::move(assignment));
    }

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!expr) {std::cerr << "[ERROR] Failed to parse expression in for-loop clause\n"; return nullptr;}

    position = expr_parser.get_position();
    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Statement> Parser::parse_function_statement()
{
    FunctionVisibility visibility = FunctionVisibility::PUBLIC;

    if(match(TokenType::PUBLIC)) {visibility = FunctionVisibility::PUBLIC;}
    else if(match(TokenType::PRIVATE)) {visibility = FunctionVisibility::PRIVATE;}

    if(!match(TokenType::FUNCTION)) {std::cerr << "Expected 'function' after visibility modifier\n"; return nullptr;}

    if(peek().type != TokenType::IDENTIFIER) {std::cerr << "Expected function name\n"; return nullptr;}

    std::string name = advance().value;

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
    return std::make_unique<FunctionStatement>(visibility, name, std::move(params), std::move(body));
}

std::unique_ptr<Statement> Parser::parse_return_statement()
{
    if(match(TokenType::RETURN))
    {
        std::unique_ptr<Expression> val;

        if(peek().type != TokenType::SEMICOLON && peek().type != TokenType::RIGHT_BRACE && peek().type != TokenType::END_OF_FILE)
        {
            ExpressionParser expr_parser(tokens, position);
            val = expr_parser.parse_expression();
        }

        if(!match(TokenType::SEMICOLON))
        {
            std::cerr << "[ERROR] Missing ';' after return\n";
        }

        return std::make_unique<ReturnStatement>(std::move(val));
    }

}