//
// Created by Denis on 08.07.2025.
//

#include "Parser.h"

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
    if(peek().type == TokenType::LET)
    {
        advance();
    }

    if(peek().type == TokenType::IDENTIFIER)
    {
        return parse_assignment();
    }

    return nullptr;
}

std::unique_ptr<Assignment> Parser::parse_assignment()
{
    Token name_token = advance();
    advance();
    Token value_token = advance();
    advance();

    Value val;

    if(value_token.type == TokenType::NUMBER)
    {
        val = Value(std::stoi(value_token.value));
    }

    return std::make_unique<Assignment>(name_token.value, val);
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