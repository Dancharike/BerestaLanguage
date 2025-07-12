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
    EQUALS,
    SEMICOLON,
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
    IF,
    ELSE,
    END_OF_FILE,
    CONSOLE_PRINT,
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
