//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSIONPARSER_H
#define BERESTALANGUAGE_EXPRESSIONPARSER_H

#pragma once
#include "../token/Token.h"
#include "Expression.h"
#include <vector>
#include <memory>

class ExpressionParser
{
    public:
        explicit ExpressionParser(const std::vector<Token>& tokens, size_t& position);
        std::unique_ptr<Expression> parse_expression();

    private:
        const std::vector<Token>& tokens;
        size_t& position;

        std::unique_ptr<Expression> parse_term();
        std::unique_ptr<Expression> parse_factor();
        std::unique_ptr<Expression> parse_primary();
        std::unique_ptr<Expression> parse_comparison();
        std::unique_ptr<Expression> parse_logic();
        std::unique_ptr<Expression> parse_if_expression();
        Token advance();
        [[nodiscard]] Token peek() const;
        bool match(TokenType type);
};


#endif //BERESTALANGUAGE_EXPRESSIONPARSER_H
