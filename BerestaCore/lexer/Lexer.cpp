//
// Created by Denis on 08.07.2025.
//

#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(std::string src) : source(std::move(src)), position(0) {}

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
    size_t start_pos = position;

    if(current == '/' && source[position + 1] == '/')
    {
        while(peek() != '\n' && peek() != '\0') {advance();}
        return next_token();
    }

    if(current == '/' && source[position + 1] == '*')
    {
        advance(); advance();

        while(peek() != '\0')
        {
            if(peek() == '*' && source[position + 1] == '/')
            {
                advance(); advance(); break;
            }

            advance();
        }

        return next_token();
    }

    if(isalpha(current) || current == '_')
    {
        std::string ident;
        ident += advance();

        while(isalnum(peek()) || peek() == '_') {ident += advance();}

        if(ident == "let") return {TokenType::LET, ident, start_pos};
        if(ident == "and") return {TokenType::AND, ident, start_pos};
        if(ident == "or") return {TokenType::OR, ident, start_pos};
        if(ident == "if") return {TokenType::IF, ident, start_pos};
        if(ident == "else") return {TokenType::ELSE, ident, start_pos};
        if(ident == "true") return {TokenType::TRUE, ident, start_pos};
        if(ident == "false") return {TokenType::FALSE, ident, start_pos};
        if(ident == "while") return {TokenType::WHILE, ident, start_pos};
        if(ident == "repeat") return {TokenType::REPEAT, ident, start_pos};
        if(ident == "for") return {TokenType::FOR, ident, start_pos};
        if(ident == "foreach") return {TokenType::FOREACH, ident, start_pos};
        if(ident == "in") return {TokenType::IN, ident, start_pos};
        if(ident == "public") return {TokenType::PUBLIC, ident, start_pos};
        if(ident == "private") return {TokenType::PRIVATE, ident, start_pos};
        if(ident == "function") return {TokenType::FUNCTION, ident, start_pos};
        if(ident == "enum") return {TokenType::ENUM, ident, start_pos};
        if(ident == "return") return {TokenType::RETURN, ident, start_pos};

        return  {TokenType::IDENTIFIER, ident, start_pos};
    }

    if(isdigit(current) || (current == '.' && isdigit(source[position + 1])))
    {
        std::string number;
        bool has_dot = false;

        while(isdigit(peek()) || (peek() == '.' && !has_dot))
        {
            if(peek() == '.')
            {
                has_dot = true;
            }

            number += advance();
        }

        return {TokenType::NUMBER, number, start_pos};
    }

    if(current == '"') {return read_string(start_pos);}
    if(current == '=' && source[position + 1] == '=') {advance(); advance(); return {TokenType::EQUAL_EQUAL, "==", start_pos};}
    if(current == '!' && source[position + 1] == '=') {advance(); advance(); return {TokenType::BANG_EQUAL, "!=", start_pos};}
    if(current == '<' && source[position + 1] == '=') {advance(); advance(); return {TokenType::LESS_EQUAL, "<=", start_pos};}
    if(current == '>' && source[position + 1] == '=') {advance(); advance(); return {TokenType::GREATER_EQUAL, ">=", start_pos};}
    if(current == '&' && source[position + 1] == '&') {advance(); advance(); return {TokenType::AND, "&&", start_pos};}
    if(current == '|' && source[position + 1] == '|') {advance(); advance(); return {TokenType::OR, "||", start_pos};}

    switch(advance())
    {
        case '+': return {TokenType::PLUS, "+", start_pos};
        case '-': return {TokenType::MINUS, "-", start_pos};
        case '*': return {TokenType::STAR, "*", start_pos};
        case '/': return {TokenType::SLASH, "/", start_pos};
        case '=': return {TokenType::EQUALS, "=", start_pos};
        case '!': return {TokenType::BANG, "!", start_pos};
        case '<': return {TokenType::LESS, "<", start_pos};
        case '>': return {TokenType::GREATER, ">", start_pos};
        case '.': return {TokenType::DOT, ".", start_pos};
        case ',': return {TokenType::COMMA, ",", start_pos};
        case ';': return {TokenType::SEMICOLON, ";", start_pos};
        case '(': return {TokenType::LEFT_PAREN, "(", start_pos};
        case ')': return {TokenType::RIGHT_PAREN, ")", start_pos};
        case '{': return {TokenType::LEFT_BRACE, "{", start_pos};
        case '}': return {TokenType::RIGHT_BRACE, "}", start_pos};
        case '[': return {TokenType::LEFT_BRACKET, "[", start_pos};
        case ']': return {TokenType::RIGHT_BRACKET, "]", start_pos};
        case '\0': return {TokenType::END_OF_FILE, "", start_pos};
        default: return {TokenType::UNKNOWN, std::string(1, current), start_pos};
    }
}

std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    while(true)
    {
        Token token = next_token();
        tokens.push_back(token);
        if(token.type == TokenType::END_OF_FILE) {break;}
    }

    return tokens;
}

Token Lexer::read_string(size_t start_pos)
{
    advance();
    std::string str;

    while(true)
    {
        char c = peek();
        if(c == '\0') {std::cerr << "[ERROR] Unterminated string literal\n"; break;}
        if(c == '"') {advance(); break;}
        else {str += advance();}
    }

    return {TokenType::STRING, str, start_pos};
}