//
// Created by Denis on 09.07.2025.
//

#include "ExpressionParser.h"

ExpressionParser::ExpressionParser(const std::vector<Token> &tokens, size_t &position) : tokens(tokens), position(position) {}

Token ExpressionParser::peek() const
{
    return position < tokens.size() ? tokens[position] : Token(TokenType::END_OF_FILE, "", position);
}

Token ExpressionParser::advance()
{
    return tokens[position++];
}

std::unique_ptr<Expression> ExpressionParser::parse_expression()
{
    auto left = parse_term();

    while(peek().value == "+" || peek().value == "-")
    {
        char op = advance().value[0];
        auto right = parse_term();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> ExpressionParser::parse_term()
{
    auto left = parse_factor();

    while(peek().value == "*" || peek().value == "/")
    {
        char op = advance().value[0];
        auto right = parse_factor();
        left = std::make_unique<BinaryExpr>(op, std::move(left), std::move(right));
    }

    return left;
}

std::unique_ptr<Expression> ExpressionParser::parse_factor()
{
    Token tok = advance();

    if(tok.type == TokenType::NUMBER)
    {
        return std::make_unique<NumberExpr>(std::stoi(tok.value));
    }

    if(tok.type == TokenType::IDENTIFIER)
    {
        return std::make_unique<VariableExpr>(tok.value);
    }

    std::cerr << "[ERROR] parse_factor(): unexpected token: " << tok.value << "\n";
    return nullptr;
}