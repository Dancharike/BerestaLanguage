#include <iostream>
#include "Lexer.h"

int main()
{
    std::string code = "let hp = 100;";
    Lexer lexer(code);
    auto tokens = lexer.tokenize();

    for(const auto& token : tokens)
    {
        std::cout << "Token(" << static_cast<int>(token.type) << ", '" << token.value << "')\n";
    }

    return 0;
}
