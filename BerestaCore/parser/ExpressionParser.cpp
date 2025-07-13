//
// Created by Denis on 09.07.2025.
//

#include "ExpressionParser.h"
#include "Statement.h"
#include "Parser.h"

ExpressionParser::ExpressionParser(const std::vector<Token> &tokens, size_t &position) : tokens(tokens), position(position) {}

Token ExpressionParser::peek() const
{
    return position < tokens.size() ? tokens[position] : Token(TokenType::END_OF_FILE, "", position);
}

Token ExpressionParser::advance() {return tokens[position++];}

bool ExpressionParser::match(TokenType type)
{
    if(peek().type == type) {advance(); return true;}
    return false;
}

std::unique_ptr<Expression> ExpressionParser::parse_expression()
{
    if(auto function_expr = parse_functions()) {return function_expr;}
    return parse_logic();
}

std::unique_ptr<Expression> ExpressionParser::parse_functions()
{
    if(match(TokenType::CONSOLE_PRINT))
    {
        if(!match(TokenType::LEFT_PAREN)) {std::cerr << "Expected '(' after 'console_print'.\n"; return nullptr;}

        auto expr = parse_expression();

        if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after 'console_print'.\n"; return nullptr;}

        return std::make_unique<ConsolePrintExpr>(std::move(expr));
    }

    return nullptr;
}

std::unique_ptr<Expression> ExpressionParser::parse_logic()
{
    auto expr = parse_comparison();

    while(match(TokenType::AND) || match(TokenType::OR))
    {
        Token op = tokens[position - 1];
        auto right = parse_comparison();
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_comparison()
{
    auto expr = parse_term();

    while(match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL) || match(TokenType::LESS) ||
        match(TokenType::LESS_EQUAL) || match(TokenType::GREATER) || match((TokenType::GREATER_EQUAL)))
    {
        Token op = tokens[position - 1];
        auto right = parse_term();
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_term()
{
    auto expr = parse_factor();

    while(peek().value == "+" || peek().value == "-")
    {
        std::string op = advance().value;
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
        std::string op = advance().value;
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
        const std::string& val = tokens[position - 1].value;
        if(val.find('.') != std::string::npos) {return std::make_unique<NumberExpr>(std::stod(val));}
        else {return std::make_unique<NumberExpr>(std::stoi(val));}
    }

    if(match(TokenType::IDENTIFIER))
    {
        return std::make_unique<VariableExpr>(tokens[position - 1].value);
    }

    if(match(TokenType::LEFT_PAREN))
    {
        auto expr = parse_expression();
        if (!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after expression.\n"; return nullptr;}
        return expr;
    }

    std::cerr << "Unexpected token: " << peek().value << "\n";
    return nullptr;
}
