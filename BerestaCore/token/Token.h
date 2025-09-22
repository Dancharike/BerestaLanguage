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
    EQUALS,
    SEMICOLON,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_BRACE,
    RIGHT_BRACE,
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

    Token(TokenType type, std::string value, size_t position) : type(type), value(std::move(value)), position(position) {}
};



#endif //BERESTALANGUAGE_TOKEN_H
