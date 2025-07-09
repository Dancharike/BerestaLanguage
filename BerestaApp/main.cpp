#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include "../BerestaCore/interpreter/Interpreter.h"
#include "../BerestaCore/lexer/Lexer.h"
#include "../BerestaCore/parser/Parser.h"

std::string read_file(const std::string& filepath)
{
    std::ifstream file(filepath);
    if(!file.is_open())
    {
        std::cerr << "Error : cannot open the file: " << filepath << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Using : BerestaApp <file_path.beresta>" << std::endl;
        return 1;
    }

    std::string code = read_file(argv[1]);

    Interpreter interpreter;
    interpreter.run(code);
}

/*
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
*/