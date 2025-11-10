//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_LEXER_H
#define BERESTALANGUAGE_LEXER_H

#pragma once
#include "frontend/token/Token.h"
#include "api/Export.h"
#include <string>
#include <vector>

class BERESTA_API Lexer
{
    public:
        explicit Lexer(std::string source);
        std::vector<Token> tokenize();

    private:
        std::string source;
        size_t position;
        int line;
        int column;

        char peek() const;
        char advance();
        void skip_whitespace();
        Token next_token();
        Token read_string(size_t start_pos, int start_line, int start_column);
};


#endif //BERESTALANGUAGE_LEXER_H
