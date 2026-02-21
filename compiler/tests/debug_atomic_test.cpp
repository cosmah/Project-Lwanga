#include "Lexer.h"
#include "Parser.h"
#include <iostream>

using namespace lwanga;

int main() {
    // Test just the atomic_increment function
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
    
    std::cout << "Parsing atomic_increment...\n";
    Lexer lexer(source);
    
    // Debug: print first 50 tokens
    std::cout << "First 50 tokens:\n";
    for (int i = 0; i < 50; i++) {
        Token tok = lexer.nextToken();
        std::cout << i << ": " << tok.lexeme << " (type=" << static_cast<int>(tok.type) << ")\n";
        if (tok.type == TokenType::TOK_EOF) break;
    }
    
    return 0;
}
