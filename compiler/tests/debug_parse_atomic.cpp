#include "Lexer.h"
#include "Parser.h"
#include <iostream>
#include <csignal>
#include <unistd.h>

using namespace lwanga;

void timeout_handler(int sig) {
    std::cout << "\n\nTIMEOUT! Parser is stuck in infinite loop!\n";
    exit(124);
}

int main() {
    // Set up alarm to detect infinite loops
    signal(SIGALRM, timeout_handler);
    alarm(3);  // 3 second timeout
    
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
    
    std::cout << "Calling parser.parse()...\n";
    auto program = parser.parse();
    
    alarm(0);  // Cancel alarm
    std::cout << "Parse complete!\n";
    
    if (parser.hasErrors()) {
        std::cout << "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    return 0;
}
