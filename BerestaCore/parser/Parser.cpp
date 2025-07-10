//
// Created by Denis on 08.07.2025.
//

#include "Parser.h"
#include "Expression.h"
#include "ExpressionParser.h"

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

Token Parser::peek() const
{
    return position < tokens.size() ? tokens[position] : Token(TokenType::END_OF_FILE, "", position);
}

Token Parser::advance()
{
    return tokens[position++];
}

std::unique_ptr<Statement> Parser::parse_statement()
{
    match(TokenType::LET);
    /*
    if(peek().type == TokenType::LET)
    {
        advance();
    }
    */

    if(peek().type == TokenType::IDENTIFIER)
    {
        return parse_assignment();
    }

    return nullptr;
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    Token name_token = advance();
    //advance();

    if(!match(TokenType::EQUALS))
    {
        std::cerr << "Expected '=' after variable name\n";
        return nullptr;
    }

    ExpressionParser expr_parser(tokens, position);
    auto expr = expr_parser.parse_expression();

    //advance();

    if(!match(TokenType::SEMICOLON))
    {
        std::cerr << "Expected ';' after expression\n";
        return nullptr;
    }

    return std::make_unique<Assignment>(name_token.value, std::move(expr));
}

std::vector<std::unique_ptr<Statement>> Parser::parse()
{
    std::vector<std::unique_ptr<Statement>> statements;

    while(peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) statements.push_back(std::move(stmt));
    }

    return statements;
}

bool Parser::match(TokenType type)
{
    if(peek().type == type)
    {
        advance();
        return true;
    }

    return false;
}