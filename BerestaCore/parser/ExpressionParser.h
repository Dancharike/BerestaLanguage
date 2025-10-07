//
// Created by Denis on 09.07.2025.
//

#ifndef BERESTALANGUAGE_EXPRESSIONPARSER_H
#define BERESTALANGUAGE_EXPRESSIONPARSER_H

#pragma once
#include "../token/Token.h"
#include "../diagnostics/Diagnostics.h"
#include "../diagnostics/BaseContext.h"
#include "../environment/Environment.h"
#include "../interpreter/FunctionIndex.h"
#include "Expression.h"
#include <vector>
#include <memory>

class ExpressionParser : public BaseContext
{
    public:
        explicit ExpressionParser(const std::vector<Token>& tokens, size_t& position, Environment& env, FunctionIndex& index, std::string current_file, Diagnostics& diag);
        std::unique_ptr<Expression> parse_expression();
        size_t& get_position() {return position;}

    private:
        const std::vector<Token>& tokens;
        size_t& position;

        std::unique_ptr<Expression> parse_term();
        std::unique_ptr<Expression> parse_factor();
        std::unique_ptr<Expression> parse_primary();
        std::unique_ptr<Expression> parse_comparison();
        std::unique_ptr<Expression> parse_logic();
        std::unique_ptr<Expression> parse_postfix(std::unique_ptr<Expression> expr);
        Token advance();
        [[nodiscard]] Token peek() const;
        bool match(TokenType type);

        Environment& _env;
        FunctionIndex& _index;
        std::vector<std::string> _file_stack;
};


#endif //BERESTALANGUAGE_EXPRESSIONPARSER_H
