//
// Created by Denis on 08.07.2025.
//

#ifndef BERESTALANGUAGE_STATEMENTPARSER_H
#define BERESTALANGUAGE_STATEMENTPARSER_H

#pragma once
#include "frontend/diagnostics/Diagnostics.h"
#include "frontend/diagnostics/BaseContext.h"
#include "frontend/token/Token.h"
#include "Statement.h"
#include <vector>
#include <memory>

class StatementParser : public BaseContext
{
    public:
        explicit StatementParser(const std::vector<Token>& tokens, std::string current_file, Diagnostics& diag);
        std::vector<std::unique_ptr<Statement>> parse();
        std::unique_ptr<Statement> parse_statement();
        std::unique_ptr<Assignment> parse_assignment();
        std::unique_ptr<Assignment> parse_assignment_expression();

    private:
        std::unique_ptr<Statement> parse_if_statement();
        std::unique_ptr<Statement> parse_while_statement();
        std::unique_ptr<Statement> parse_repeat_statement();
        std::unique_ptr<Statement> parse_for_statement();
        std::unique_ptr<Statement> parse_foreach_statement();
        std::unique_ptr<Statement> parse_optional_assignment_or_expression();
        std::unique_ptr<Statement> parse_block();
        std::unique_ptr<Statement> parse_function_statement();
        std::unique_ptr<Statement> parse_return_statement();
        std::unique_ptr<Statement> parse_index_assignment();
        std::unique_ptr<Statement> parse_index_assignment_expression();
        std::unique_ptr<Statement> parse_enum_statement();
        std::unique_ptr<Statement> parse_macros_statement();

        [[nodiscard]] Token peek() const;
        Token advance();
        bool match(TokenType type);

        const std::vector<Token>& tokens;
        size_t position = 0;

        std::vector<std::string> _file_stack;
};


#endif //BERESTALANGUAGE_STATEMENTPARSER_H
