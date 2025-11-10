//
// Created by Denis on 10.11.2025.
//

#include "doctest/doctest.h"
#include "frontend/lexer/Lexer.h"
#include "frontend/token/Token.h"
#include <algorithm>

TEST_CASE("Lexer correctly tokenizes a simple assignment")
{
    std::string src = "let x = 10 + 5;";
    Lexer lexer(src);

    std::vector<Token> tokens = lexer.tokenize();

    REQUIRE_EQ(tokens.size(), 8);

    CHECK_EQ(tokens[0].value, "let");
    CHECK_EQ(tokens[0].type,  TokenType::LET);

    CHECK_EQ(tokens[1].value, "x");
    CHECK_EQ(tokens[1].type,  TokenType::IDENTIFIER);

    CHECK_EQ(tokens[2].value, "=");
    CHECK_EQ(tokens[2].type,  TokenType::EQUALS);

    CHECK_EQ(tokens[3].value, "10");
    CHECK_EQ(tokens[3].type,  TokenType::NUMBER);

    CHECK_EQ(tokens[4].value, "+");
    CHECK_EQ(tokens[4].type,  TokenType::PLUS);

    CHECK_EQ(tokens[5].value, "5");
    CHECK_EQ(tokens[5].type,  TokenType::NUMBER);

    CHECK_EQ(tokens[6].value, ";");
    CHECK_EQ(tokens[6].type,  TokenType::SEMICOLON);

    CHECK_EQ(tokens[7].type,  TokenType::END_OF_FILE);
}

TEST_CASE("Lexer handles strings and comments correctly")
{
    std::string src = R"(
        // This is a line comment
        let message = "Hello, world!";
        /* This is
           a block
           comment */
        let number = 42;
    )";

    Lexer lexer(src);
    std::vector<Token> tokens = lexer.tokenize();

    REQUIRE(tokens.size() > 0);

    CHECK_EQ(tokens[0].value, "let");
    CHECK_EQ(tokens[0].type, TokenType::LET);

    CHECK_EQ(tokens[1].value, "message");
    CHECK_EQ(tokens[1].type, TokenType::IDENTIFIER);

    CHECK_EQ(tokens[2].value, "=");
    CHECK_EQ(tokens[2].type, TokenType::EQUALS);

    CHECK_EQ(tokens[3].value, "Hello, world!");
    CHECK_EQ(tokens[3].type, TokenType::STRING);

    CHECK_EQ(tokens[4].value, ";");
    CHECK_EQ(tokens[4].type, TokenType::SEMICOLON);

    bool found_second_let = false;
    for(auto &t : tokens)
    {
        if(t.type == TokenType::LET && &t != &tokens[0])
        {
            found_second_let = true;
            break;
        }
    }
    CHECK(found_second_let);

    CHECK_EQ(tokens.back().type, TokenType::END_OF_FILE);
}

TEST_CASE("Lexer tokenizes numeric literals correctly")
{
    std::string src = R"(
        let a = 10;
        let b = 3.14;
        let c = .5;
        let d = 5.;
        let e = 0.001;
    )";

    Lexer lexer(src);
    std::vector<Token> tokens = lexer.tokenize();

    bool found_int = false;
    bool found_float1 = false;
    bool found_float2 = false;
    bool found_float3 = false;

    for(auto &t : tokens)
    {
        if(t.type == TokenType::NUMBER)
        {
            if(t.value == "10") {found_int = true;}
            if(t.value == "3.14") {found_float1 = true;}
            if(t.value == ".5") {found_float2 = true;}
            if(t.value == "5.") {found_float3 = true;}
        }
    }

    CHECK(found_int);
    CHECK(found_float1);
    CHECK(found_float2);
    CHECK(found_float3);

    auto it = std::find_if(tokens.begin(), tokens.end(),[](const Token &tok){return tok.value == "0.001";});
    CHECK(it != tokens.end());
    CHECK_EQ(it->type, TokenType::NUMBER);

    CHECK_EQ(tokens.back().type, TokenType::END_OF_FILE);
}
