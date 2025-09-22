#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include "../BerestaCore/interpreter/Interpreter.h"

std::string read_file(const std::string& filepath)
{
    std::ifstream file(filepath);
    if(!file.is_open()) {std::cerr << "Error : cannot open the file: " << filepath << std::endl; exit(1);}

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[])
{
    if(argc < 2) {std::cerr << "Using : BerestaApp <entry_file.beresta>" << std::endl; return 1;}

    std::filesystem::path entry_path = argv[1];
    if(!std::filesystem::exists(entry_path)) {std::cerr << "Entry file not found: " << entry_path.string() << std::endl; return 1;}

    std::filesystem::path root_dir = entry_path.parent_path();

    Interpreter interpreter;

    for(const auto& p : std::filesystem::directory_iterator(root_dir))
    {
        if(!p.is_regular_file()) {continue;}
        if(p.path().extension() == ".beresta")
        {
            std::string code = read_file(p.path().string());
            interpreter.register_file(p.path().filename().string(), code);
        }
    }

    interpreter.run_project(entry_path.filename().string());
    return 0;
}
