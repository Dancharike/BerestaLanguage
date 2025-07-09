//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_PARSER_H
#define BERESTALANGUAGE_PARSER_H

#pragma once
#include "../token/Token.h"
#include "Statement.h"
#include <vector>
#include <memory>

class Parser
{
    public:
        explicit Parser(const std::vector<Token>& tokens);
        std::vector<std::unique_ptr<Statement>> parse();

    private:
        const std::vector<Token>& tokens;
        size_t position = 0;

        Token peek() const;
        Token advance();
        std::unique_ptr<Statement> parse_statement();
        std::unique_ptr<Assignment> parse_assignment();
};


#endif //BERESTALANGUAGE_PARSER_H
