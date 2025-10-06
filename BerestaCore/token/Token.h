//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_TOKEN_H
#define BERESTALANGUAGE_TOKEN_H


#pragma once
#include <string>

enum class TokenType
{
    LET,
    IDENTIFIER,
    NUMBER,
    STRING,
    ENUM,
    DOT,
    TRUE,
    FALSE,
    EQUALS,
    SEMICOLON,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    COLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
    LEFT_BRACKET,
    RIGHT_BRACKET,
    EQUAL_EQUAL,
    BANG_EQUAL,
    BANG,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND,
    OR,
    COMMA,
    IF,
    ELSE,
    WHILE,
    REPEAT,
    FOR,
    FOREACH,
    IN,
    PUBLIC,
    PRIVATE,
    FUNCTION,
    RETURN,
    END_OF_FILE,
    UNKNOWN
};

struct Token
{
    TokenType type;
    std::string value;
    size_t position;
    int line;
    int column;

    Token(TokenType type, std::string value, size_t position, int line = -1, int column = -1) : type(type), value(std::move(value)), position(position), line(line), column(column) {}
};



#endif //BERESTALANGUAGE_TOKEN_H
