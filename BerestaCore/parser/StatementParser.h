//
// Created by Denis on 12.07.2025.
//

#ifndef BERESTALANGUAGE_STATEMENTPARSER_H
#define BERESTALANGUAGE_STATEMENTPARSER_H

#pragma once
#include <memory>
#include "Statement.h"
#include "token/Token.h"

class StatementParser
{
    public:
        StatementParser(const std::vector<Token>& tokens, size_t& position);
        std::unique_ptr<Statement> parse_block();

    private:
        const std::vector<Token>& tokens;
        size_t& position;

        [[nodiscard]] Token peek() const;
        Token advance();
        bool match(TokenType type);
        std::unique_ptr<Statement> parse_statement();
};


#endif //BERESTALANGUAGE_STATEMENTPARSER_H
