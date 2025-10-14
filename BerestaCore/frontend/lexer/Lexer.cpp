//
// Created by Denis on 08.07.2025.
//

#include "Lexer.h"
#include <cctype>
#include <iostream>

Lexer::Lexer(std::string src) : source(std::move(src)), position(0), line(1), column(1) {}

char Lexer::peek() const
{
    return position < source.length() ? source[position] : '\0';
}

char Lexer::advance()
{
    if(position >= source.length()) {return '\0';}

    char c = source[position++];

    if(c == '\n')
    {
        line++;
        column = 1;
    }
    else
    {
        column++;
    }

    return c;
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
    int start_line = line;
    int start_column = column;

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

        auto make = [&](TokenType t) {return Token(t, ident, start_pos, start_line, start_column);};

        if(ident == "let")      {return make(TokenType::LET);}
        if(ident == "and")      {return make(TokenType::AND);}
        if(ident == "or")       {return make(TokenType::OR);}
        if(ident == "if")       {return make(TokenType::IF);}
        if(ident == "else")     {return make(TokenType::ELSE);}
        if(ident == "true")     {return make(TokenType::TRUE);}
        if(ident == "false")    {return make(TokenType::FALSE);}
        if(ident == "while")    {return make(TokenType::WHILE);}
        if(ident == "repeat")   {return make(TokenType::REPEAT);}
        if(ident == "for")      {return make(TokenType::FOR);}
        if(ident == "foreach")  {return make(TokenType::FOREACH);}
        if(ident == "in")       {return make(TokenType::IN);}
        if(ident == "public")   {return make(TokenType::PUBLIC);}
        if(ident == "private")  {return make(TokenType::PRIVATE);}
        if(ident == "function") {return make(TokenType::FUNCTION);}
        if(ident == "enum")     {return make(TokenType::ENUM);}
        if(ident == "return")   {return make(TokenType::RETURN);}

        return make(TokenType::IDENTIFIER);
    }

    if(isdigit(current) || (current == '.' && isdigit(source[position + 1])))
    {
        std::string number;
        bool has_dot = false;

        while(isdigit(peek()) || (peek() == '.' && !has_dot))
        {
            if(peek() == '.') has_dot = true;
            number += advance();
        }

        return {TokenType::NUMBER, number, start_pos, start_line, start_column};
    }

    if(current == '"') {return read_string(start_pos, start_line, start_column);}
    if(current == '=' && source[position + 1] == '=') {advance(); advance(); return {TokenType::EQUAL_EQUAL, "==", start_pos, start_line, start_column};}
    if(current == '!' && source[position + 1] == '=') {advance(); advance(); return {TokenType::BANG_EQUAL, "!=", start_pos, start_line, start_column};}
    if(current == '<' && source[position + 1] == '=') {advance(); advance(); return {TokenType::LESS_EQUAL, "<=", start_pos, start_line, start_column};}
    if(current == '>' && source[position + 1] == '=') {advance(); advance(); return {TokenType::GREATER_EQUAL, ">=", start_pos, start_line, start_column};}
    if(current == '&' && source[position + 1] == '&') {advance(); advance(); return {TokenType::AND, "&&", start_pos, start_line, start_column};}
    if(current == '|' && source[position + 1] == '|') {advance(); advance(); return {TokenType::OR, "||", start_pos, start_line, start_column};}

    switch(advance())
    {
        case '+': return {TokenType::PLUS, "+", start_pos, start_line, start_column};
        case '-': return {TokenType::MINUS, "-", start_pos, start_line, start_column};
        case '*': return {TokenType::STAR, "*", start_pos, start_line, start_column};
        case '/': return {TokenType::SLASH, "/", start_pos, start_line, start_column};
        case '=': return {TokenType::EQUALS, "=", start_pos, start_line, start_column};
        case '!': return {TokenType::BANG, "!", start_pos, start_line, start_column};
        case '<': return {TokenType::LESS, "<", start_pos, start_line, start_column};
        case '>': return {TokenType::GREATER, ">", start_pos, start_line, start_column};
        case '.': return {TokenType::DOT, ".", start_pos, start_line, start_column};
        case ',': return {TokenType::COMMA, ",", start_pos, start_line, start_column};
        case ':': return {TokenType::COLON, ":", start_pos, start_line, start_column};
        case ';': return {TokenType::SEMICOLON, ";", start_pos, start_line, start_column};
        case '(': return {TokenType::LEFT_PAREN, "(", start_pos, start_line, start_column};
        case ')': return {TokenType::RIGHT_PAREN, ")", start_pos, start_line, start_column};
        case '{': return {TokenType::LEFT_BRACE, "{", start_pos, start_line, start_column};
        case '}': return {TokenType::RIGHT_BRACE, "}", start_pos, start_line, start_column};
        case '[': return {TokenType::LEFT_BRACKET, "[", start_pos, start_line, start_column};
        case ']': return {TokenType::RIGHT_BRACKET, "]", start_pos, start_line, start_column};
        case '\0': return {TokenType::END_OF_FILE, "", start_pos, start_line, start_column};
        default:  return {TokenType::UNKNOWN, std::string(1, current), start_pos, start_line, start_column};
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

Token Lexer::read_string(size_t start_pos, int start_line, int start_column)
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

    return {TokenType::STRING, str, start_pos, start_line, start_column};
}