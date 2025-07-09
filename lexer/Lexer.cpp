//
// Created by Denis on 08.07.2025.
//

#include "Lexer.h"
#include "Token.h"
#include <cctype>

Lexer::Lexer(const std::string& src) : source(src), position(0) {}

char Lexer::peek() const
{
    return position < source.length() ? source[position] : '\0';
}

char Lexer::advance()
{
    return position < source.length() ? source[position++] : '\0';
}

void Lexer::skip_whitespace()
{
    while(isspace(peek())) advance();
}

Token Lexer::next_token()
{
    skip_whitespace();
    char current = peek();
    int start_pos = position;

    if(isalpha(current) || current == '_')
    {
        std::string ident;
        ident += advance();

        while(isalnum(peek()) || peek() == '_') {ident += advance();}

        if(ident == "let")
        {
            return  Token(TokenType::LET, ident, start_pos);
        }

        return  Token(TokenType::IDENTIFIER, ident, start_pos);
    }

    if(isdigit(current))
    {
        std::string number;
        while(isdigit(peek())) {number += advance();}
        return Token(TokenType::NUMBER, number, start_pos);
    }

    switch (advance())
    {
        case '=': return Token(TokenType::EQUALS, "=", start_pos);
        case ';': return Token(TokenType::SEMICOLON, ";", start_pos);
        case '\0': return Token(TokenType::END_OF_FILE, "", start_pos);
        default: return Token(TokenType::UNKNOWN, std::string(1, current), start_pos);
    }
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    while(true)
    {
        Token token = next_token();
        tokens.push_back(token);
        if (token.type == TokenType::END_OF_FILE) {break;}
    }

    return tokens;
}