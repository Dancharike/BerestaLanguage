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

bool ExpressionParser::match(TokenType type)
{
    if(peek().type == type)
    {
        advance();
        return true;
    }
    return false;
}

std::unique_ptr<Expression> ExpressionParser::parse_expression()
{
    return parse_term();
}

std::unique_ptr<Expression> ExpressionParser::parse_term()
{
    auto expr = parse_factor();

    while(peek().value == "+" || peek().value == "-")
    {
        char op = advance().value[0];
        auto right = parse_factor();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_factor()
{
    auto expr = parse_primary();

    while(peek().value == "*" || peek().value == "/")
    {
        char op = advance().value[0];
        auto right = parse_primary();
        expr = std::make_unique<BinaryExpr>(op, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_primary()
{
    if(peek().value == "-" || peek().value == "+")
    {
        char op = advance().value[0];
        auto right = parse_primary();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }

    if(match(TokenType::NUMBER))
    {
        //return std::make_unique<NumberExpr>(std::stoi(tokens[position - 1].value));

        const std::string& val = tokens[position - 1].value;
        if(val.find('.') != std::string::npos)
        {
            return std::make_unique<NumberExpr>(std::stod(val));
        }
        else
        {
            return std::make_unique<NumberExpr>(std::stoi(val));
        }
    }

    if(match(TokenType::IDENTIFIER))
    {
        return std::make_unique<VariableExpr>(tokens[position - 1].value);
    }

    if(match(TokenType::LEFT_PAREN))
    {
        auto expr = parse_expression();
        if (!match(TokenType::RIGHT_PAREN))
        {
            std::cerr << "Expected ')' after expression.\n";
            return nullptr;
        }
        return expr;
    }

    std::cerr << "Unexpected token: " << peek().value << "\n";
    return nullptr;
}
