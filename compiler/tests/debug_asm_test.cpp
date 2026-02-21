#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include <iostream>

using namespace lwanga;

int main() {
    std::string source = R"(
        fn get_timestamp() -> u64 {
            let mut result: u64 = 0;
            unsafe {
                asm {
                    rdtsc
                    shl rdx, 32
                    or rax, rdx
                }
            }
            return result;
        }
    )";
    
    std::cout << "Starting parse...\n";
    Lexer lexer(source);
    Parser parser(lexer);
    
    std::cout << "Calling parse()...\n";
    auto program = parser.parse();
    
    std::cout << "Parse complete!\n";
    
    if (parser.hasErrors()) {
        std::cout << "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
        return 1;
    }
    
    std::cout << "Success!\n";
    return 0;
}
