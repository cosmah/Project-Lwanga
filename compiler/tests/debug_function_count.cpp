#include "Lexer.h"
#include "Parser.h"
#include <iostream>

using namespace lwanga;

int main() {
    std::string source = R"(
        const BUFFER_SIZE: u64 = 1024;
        
        fn test1() -> u64 { return 0; }
        fn test2() -> u64 { return 0; }
        fn test3() -> u64 { return 0; }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    std::cout << "Constants: " << program->constants.size() << "\n";
    std::cout << "Functions: " << program->functions.size() << "\n";
    
    for (const auto& func : program->functions) {
        std::cout << "  - " << func->name << "\n";
    }
    
    if (parser.hasErrors()) {
        std::cout << "\nParser errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    return 0;
}
