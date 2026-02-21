#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include <iostream>

using namespace lwanga;

int main() {
    std::string source = R"(
        fn atomic_increment(counter: ptr) -> u64 {
            let mut old_value: u64 = 0;
            unsafe {
                asm {
                    mov $1, %rax
                    lock xadd %rax, (%1)
                    mov %rax, %0
                    : "=r"(old_value)
                    : "r"(counter)
                    : "rax", "memory"
                }
            }
            return old_value;
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
