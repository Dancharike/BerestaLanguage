//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_LEXER_H
#define BERESTALANGUAGE_LEXER_H

#pragma once
#include <string>
#include <vector>
#include "../token/Token.h"

class Lexer
{
    public:
        explicit Lexer(std::string source);
        std::vector<Token> tokenize();

    private:
        std::string source;
        size_t position;

        char peek() const;
        char advance();
        void skip_whitespace();
        Token next_token();
        Token read_string(size_t start_pos);
};


#endif //BERESTALANGUAGE_LEXER_H
