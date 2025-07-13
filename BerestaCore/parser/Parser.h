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
        std::unique_ptr<Statement> parse_statement();
        std::unique_ptr<Assignment> parse_assignment();

    private:
        std::unique_ptr<Statement> parse_if_statement();
        std::unique_ptr<Statement> parse_block();

        [[nodiscard]] Token peek() const;
        Token advance();
        bool match(TokenType type);

        const std::vector<Token>& tokens;
        size_t position = 0;
};


#endif //BERESTALANGUAGE_PARSER_H
