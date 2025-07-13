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
    match(TokenType::LET);

    if(peek().type == TokenType::IF) {return parse_if_statement();}
    if(peek().type == TokenType::LEFT_BRACE) {return parse_block();}
    if(peek().type == TokenType::IDENTIFIER && tokens[position + 1].type == TokenType::EQUALS) {return parse_assignment();}

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!expr) {std::cerr << "[ERROR] Failed to parse expression\n"; return nullptr;}

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after expression\n"; return nullptr;}

    return std::make_unique<ExpressionStatement>(std::move(expr));
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    Token name_token = advance();
    if(!match(TokenType::EQUALS)) {std::cerr << "Expected '=' after variable name\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    if(!match(TokenType::SEMICOLON)) {std::cerr << "Expected ';' after expression\n"; return nullptr;}

    return std::make_unique<Assignment>(name_token.value, std::move(expr));
}

std::unique_ptr<Statement> Parser::parse_if_statement()
{
    match(TokenType::IF);

    if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'if'\n"; return nullptr;}

    ExpressionParser expr_parser(tokens, position);
    auto condition = expr_parser.parse_expression();

    if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after condition\n"; return nullptr;}

    auto then_branch = parse_statement();
    std::unique_ptr<Statement> else_branch = nullptr;

    if(match(TokenType::ELSE)) {else_branch = parse_statement();}

    return std::make_unique<IfStatement>(std::move(condition), std::move(then_branch), std::move(else_branch));
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
