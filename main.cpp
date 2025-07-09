#include <iostream>
#include "Lexer.h"
#include "interpreter/Interpreter.h"
#include "parser/Parser.h"

int main()
{
    std::string code = "let hp = 100;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto statements = parser.parse();

    Interpreter interpreter;
    interpreter.interpret(statements);
    interpreter.print_variables();

    return 0;
}
