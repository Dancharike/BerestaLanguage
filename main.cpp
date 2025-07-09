#include <iostream>
#include <string>
#include "Lexer.h"
#include "parser/Parser.h"
#include "interpreter/Interpreter.h"

int main()
{
    Interpreter interpreter;

    std::cout << "BerestaLang REPL. Type 'exit' to quit.\n";
    while(true)
    {
        std::cout << ">>> ";
        std::string line;
        std::getline(std::cin, line);

        if (line == "exit") {break;}
        if (line.empty()) {continue;}

        Lexer lexer(line);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        auto statements = parser.parse();

        auto [name, value] = interpreter.interpret(statements);
        if (!name.empty())
        {
            std::cout << " " << name << " = " << value.to_string() << std::endl;
        }
    }

    return 0;
}
