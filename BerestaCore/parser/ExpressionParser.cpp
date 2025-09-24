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
    return parse_logic();
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

    while(peek().type == TokenType::PLUS || peek().type == TokenType::MINUS)
    {
        Token op = advance();
        auto right = parse_factor();
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_factor()
{
    auto expr = parse_primary();

    while(match(TokenType::LEFT_BRACKET))
    {
        auto index = parse_expression();
        if(!match(TokenType::RIGHT_BRACKET)) {std::cerr << "Expected ']' after index expression\n"; return nullptr;}
        expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index));
    }

    while(peek().type == TokenType::STAR || peek().type == TokenType::SLASH)
    {
        Token op = advance();
        auto right = parse_primary();

        while(match(TokenType::LEFT_BRACKET))
        {
            auto index = parse_expression();
            if(!match(TokenType::RIGHT_BRACKET)) {std::cerr << "Expected ']' after index expression\n"; return nullptr;}
            right = std::make_unique<IndexExpr>(std::move(right), std::move(index));
        }

        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right));
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_primary()
{
    if(peek().type == TokenType::BANG || peek().type == TokenType::MINUS || peek().type == TokenType::PLUS)
    {
        Token op = advance();

        auto right = parse_primary();
        char op_char;

        if(op.type == TokenType::BANG) {op_char = '!';}
        else if(op.type == TokenType::MINUS) {op_char = '-';}
        else {op_char = '+';}

        return std::make_unique<UnaryExpr>(op_char, std::move(right));
    }

    if(match(TokenType::TRUE)) {return std::make_unique<BoolExpr>(true);}

    if(match(TokenType::FALSE)) {return std::make_unique<BoolExpr>(false);}

    if(match(TokenType::NUMBER))
    {
        const std::string& val = tokens[position - 1].value;
        if(val.find('.') != std::string::npos) {return std::make_unique<NumberExpr>(std::stod(val));}
        else {return std::make_unique<NumberExpr>(std::stoi(val));}
    }

    if(match(TokenType::STRING)) {return std::make_unique<StringExpr>(tokens[position - 1].value);}

    if(match(TokenType::IDENTIFIER))
    {
        std::string name = tokens[position - 1].value;

        if(peek().type == TokenType::LEFT_PAREN)
        {
            advance();
            std::vector<std::unique_ptr<Expression>> args;

            if(peek().type != TokenType::RIGHT_PAREN)
            {
                do
                {
                    args.push_back(parse_expression());
                }
                while(match(TokenType::COMMA));
            }

            if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Unexpected token: " << peek().value << "\n"; return nullptr;}

            return std::make_unique<FunctionCallExpr>(std::make_unique<VariableExpr>(name), std::move(args));
        }

        return std::make_unique<VariableExpr>(name);
    }

    if(match(TokenType::LEFT_PAREN))
    {
        auto expr = parse_expression();
        if(!match(TokenType::RIGHT_PAREN)) {std::cerr << "Expected ')' after expression.\n"; return nullptr;}
        return expr;
    }

    if(match(TokenType::LEFT_BRACKET))
    {
        std::vector<std::unique_ptr<Expression>> elems;
        if(peek().type != TokenType::RIGHT_BRACKET)
        {
            do
            {
                elems.push_back(parse_expression());
            } while(match(TokenType::COMMA));
        }

        if(!match(TokenType::RIGHT_BRACKET)) {std::cerr << "Expected ']' after array literal\n"; return nullptr;}

        return std::make_unique<ArrayLiteralExpr>(std::move(elems));
    }

    std::cerr << "Unexpected token: " << peek().value << "\n";
    return nullptr;
}
