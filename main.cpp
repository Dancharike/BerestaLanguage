#include <iostream>
#include "Lexer.h"
#include "value/Value.h"

int main()
{
    Value v1(42);
    Value v2(true);
    Value v3(std::string("hello"));

    std::cout << "v1 = " << v1.to_string() << std::endl;
    std::cout << "v2 = " << v2.to_string() << std::endl;
    std::cout << "v3 = " << v3.to_string() << std::endl;

    return 0;
}
