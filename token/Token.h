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
    END_OF_FILE,
    UNKNOWN
};

struct Token
{
    TokenType type;
    std::string value;
    int position;

    Token(TokenType type, const std::string& value, int position)
            : type(type), value(value), position(position) {}
};



#endif //BERESTALANGUAGE_TOKEN_H
