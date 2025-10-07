//
// Created by Denis on 09.07.2025.
//

#include "ExpressionParser.h"
#include "Statement.h"

ExpressionParser::ExpressionParser(const std::vector<Token> &tokens, size_t &position, Environment& env, FunctionIndex& index, std::string current_file, Diagnostics& diag)
    : BaseContext(diag, std::move(current_file)), _env(env), _index(index), tokens(tokens), position(position)
    {
        _file_stack.push_back(_current_file);
    }

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
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right), op.line, op.column);
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_comparison()
{
    auto expr = parse_term();

    while(match(TokenType::EQUAL_EQUAL) || match(TokenType::BANG_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL) || match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL))
    {
        Token op = tokens[position - 1];
        auto right = parse_term();
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right), op.line, op.column);
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
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right), op.line, op.column);
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_factor()
{
    auto expr = parse_postfix(parse_primary());

    while(peek().type == TokenType::STAR || peek().type == TokenType::SLASH)
    {
        Token op = advance();
        auto right = parse_postfix(parse_primary());
        expr = std::make_unique<BinaryExpr>(op.value, std::move(expr), std::move(right), op.line, op.column);
    }

    return expr;
}

std::unique_ptr<Expression> ExpressionParser::parse_primary()
{
    if(peek().type == TokenType::BANG || peek().type == TokenType::MINUS || peek().type == TokenType::PLUS)
    {
        Token op = advance();
        auto right = parse_primary();
        char op_char = (op.type == TokenType::BANG) ? '!' : (op.type == TokenType::MINUS ? '-' : '+');
        return std::make_unique<UnaryExpr>(op_char, std::move(right), op.line, op.column);
    }

    if(match(TokenType::TRUE))  {Token t = tokens[position - 1]; return std::make_unique<BoolExpr>(true,  t.line, t.column);}
    if(match(TokenType::FALSE)) {Token t = tokens[position - 1]; return std::make_unique<BoolExpr>(false, t.line, t.column);}

    if(match(TokenType::NUMBER))
    {
        Token t = tokens[position - 1];
        const std::string& val = t.value;
        if(val.find('.') != std::string::npos) {return std::make_unique<NumberExpr>(std::stod(val), t.line, t.column);}
        else                                      {return std::make_unique<NumberExpr>(std::stoi(val), t.line, t.column);}
    }

    if(match(TokenType::STRING))
    {
        Token t = tokens[position - 1];
        return std::make_unique<StringExpr>(t.value, t.line, t.column);
    }

    if(match(TokenType::IDENTIFIER))
    {
        Token t = tokens[position - 1];
        return std::make_unique<VariableExpr>(t.value, t.line, t.column);
    }

    if(match(TokenType::LEFT_PAREN))
    {
        Token left_paren = tokens[position - 1];
        auto expr = parse_expression();
        if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after expression", current_file(), left_paren.line); return nullptr;}
        return expr;
    }

    if(match(TokenType::LEFT_BRACKET))
    {
        Token lb = tokens[position - 1];
        std::vector<std::unique_ptr<Expression>> elems;
        if(peek().type != TokenType::RIGHT_BRACKET)
        {
            do
            {
                elems.push_back(parse_expression());
            } while(match(TokenType::COMMA));
        }

        if(!match(TokenType::RIGHT_BRACKET)) {_diag.error("Expected ']' after array literal", current_file(), lb.line); return nullptr;}
        return std::make_unique<ArrayLiteralExpr>(std::move(elems), lb.line, lb.column);
    }

    if(match(TokenType::LEFT_BRACE))
    {
        Token lb = tokens[position - 1];
        std::vector<std::pair<std::unique_ptr<Expression>, std::unique_ptr<Expression>>> entries;

        if(peek().type != TokenType::RIGHT_BRACE)
        {
            do
            {
                if(peek().type != TokenType::STRING) {_diag.error("Dictionary keys must be string literals", current_file(), lb.line); return nullptr;}
                auto key = parse_expression();

                if(!match(TokenType::COLON)) {_diag.error("Expected ':' in dictionary literal", current_file(), lb.line); return nullptr;}

                auto value = parse_expression();
                entries.emplace_back(std::move(key), std::move(value));
            } while(match(TokenType::COMMA));
        }

        if(!match(TokenType::RIGHT_BRACE)) {_diag.error("Expected '}' after dictionary literal", current_file(), lb.line); return nullptr;}
        return std::make_unique<DictionaryLiteralExpr>(std::move(entries), lb.line, lb.column);
    }

    _diag.error("Unexpected token: " + peek().value, current_file(), peek().line);
    return nullptr;
}

std::unique_ptr<Expression> ExpressionParser::parse_postfix(std::unique_ptr<Expression> expr)
{
    while(true)
    {
        if(match(TokenType::LEFT_BRACKET))
        {
            Token lb = tokens[position - 1];
            auto index = parse_expression();
            if(!match(TokenType::RIGHT_BRACKET)) {_diag.error("Expected ']' after index expression", current_file(), lb.line); return nullptr;}
            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index), lb.line, lb.column);
            continue;
        }

        if(match(TokenType::LEFT_PAREN))
        {
            Token lp = tokens[position - 1];
            std::vector<std::unique_ptr<Expression>> args;
            if(peek().type != TokenType::RIGHT_PAREN)
            {
                do {args.push_back(parse_expression());} while(match(TokenType::COMMA));
            }

            if(!match(TokenType::RIGHT_PAREN)) {_diag.error("Expected ')' after function arguments", current_file(), lp.line); return nullptr;}
            expr = std::make_unique<FunctionCallExpr>(std::move(expr), std::move(args), lp.line, lp.column);
            continue;
        }

        if(match(TokenType::DOT))
        {
            Token dot = tokens[position - 1];
            if(peek().type != TokenType::IDENTIFIER) {_diag.error("Expected identifier after '.'", current_file(), dot.line); return nullptr;}
            std::string member = advance().value;
            expr = std::make_unique<MemberAccessExpr>(std::move(expr), std::move(member), dot.line, dot.column);
            continue;
        }

        break;
    }

    return expr;
}
