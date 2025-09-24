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
        std::unique_ptr<Assignment> parse_assignment_expression();

    private:
        std::unique_ptr<Statement> parse_if_statement();
        std::unique_ptr<Statement> parse_while_statement();
        std::unique_ptr<Statement> parse_repeat_statement();
        std::unique_ptr<Statement> parse_for_statement();
        std::unique_ptr<Statement> parse_optional_assignment_or_expression();
        std::unique_ptr<Statement> parse_block();
        std::unique_ptr<Statement> parse_function_statement();
        std::unique_ptr<Statement> parse_return_statement();
        std::unique_ptr<Statement> parse_index_assignment();
        std::unique_ptr<Statement> parse_index_assignment_expression();

        [[nodiscard]] Token peek() const;
        Token advance();
        bool match(TokenType type);

        const std::vector<Token>& tokens;
        size_t position = 0;
};


#endif //BERESTALANGUAGE_PARSER_H
