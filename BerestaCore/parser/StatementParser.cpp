//
// Created by Denis on 12.07.2025.
//

#include "StatementParser.h"
#include "Statement.h"
#include "token/Token.h"
#include "Parser.h"
#include <memory>

std::unique_ptr<Statement> StatementParser::parse_block()
{
    if (!match(TokenType::LEFT_BRACE)) {std::cerr << "Expected '{' at beginning of block\n"; return nullptr;}

    std::vector<std::unique_ptr<Statement>> statements;

    while(!match(TokenType::RIGHT_BRACE) && peek().type != TokenType::END_OF_FILE)
    {
        auto stmt = parse_statement();
        if(stmt) {statements.push_back(std::move(stmt));}
    }

    return std::make_unique<BlockStatement>(std::move(statements));
}

StatementParser::StatementParser(const std::vector<Token> &tokens, size_t &position) : tokens(tokens), position(position) {}

Token StatementParser::peek() const
{
    return position < tokens.size() ? tokens[position] : Token(TokenType::END_OF_FILE, "", position);
}

Token StatementParser::advance()
{
    return tokens[position++];
}

bool StatementParser::match(TokenType type)
{
    if(peek().type == type)
    {
        advance();
        return true;
    }

    return false;
}

std::unique_ptr<Statement> StatementParser::parse_statement()
{
    Parser parser(tokens);
    parser.position = position;

    auto stmt = parser.parse_statement();
    position = parser.position;

    return stmt;
}