#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

void testComprehensiveProgram() {
    // This test covers all features from Tasks 1-11:
    // - Lexer: keywords, operators, literals, comments
    // - Parser: functions, structs, constants, control flow, expressions
    // - Type system: primitives, pointers, structs, type checking
    // - IR generation: functions, expressions, control flow, pointers, structs
    // - Syscalls: platform-specific syscall generation
    // - Inline assembly: basic and extended asm with constraints
    // - Naked functions: functions without prologue/epilogue
    
    std::string source = R"(
        // Task 2: Lexer - comments, keywords, literals, operators
        // Task 14: Constants with compile-time evaluation
        const BUFFER_SIZE: u64 = 1024;
        const DOUBLED: u64 = BUFFER_SIZE * 2;
        const SYSCALL_WRITE: u64 = 1;
        const SYSCALL_EXIT: u64 = 60;
        const STDOUT: u64 = 1;
        
        // Task 8: Structs - definition, initialization, field access
        struct Message {
            data: ptr,
            len: u64,
            flags: u32
        }
        
        packed struct Header {
            magic: u8,
            version: u8,
            size: u16
        }
        
        // Task 3: Parser - function definitions with parameters
        // Task 4: Type system - type checking
        // Task 6: IR generation - basic function generation
        fn create_message(text: ptr, length: u64) -> Message {
            let msg: Message = Message {
                data: text,
                len: length,
                flags: 0
            };
            return msg;
        }
        
        // Task 7: Pointer operations - address-of, dereference, arithmetic
        fn pointer_operations(base: ptr, offset: u64) -> u64 {
            let p1: ptr = base + offset;
            let p2: ptr = p1 - 5;
            
            unsafe {
                let addr: u64 = p2 as u64;
                let back: ptr = addr as ptr;
                return back as u64;
            }
        }
        
        // Task 6: Control flow - if/else, while loops
        fn calculate_sum(n: u64) -> u64 {
            let mut sum: u64 = 0;
            let mut i: u64 = 0;
            
            while (i < n) {
                if (i % 2 == 0) {
                    sum = sum + i;
                } else {
                    sum = sum + (i * 2);
                }
                i = i + 1;
            }
            
            return sum;
        }
        
        // Task 6: Complex expressions with operator precedence
        fn complex_math(a: u64, b: u64, c: u64) -> u64 {
            let result: u64 = (a + b) * c - (a & b) | (c ^ 255);
            let shifted: u64 = result << 2;
            let masked: u64 = shifted & 0xFFFF;
            return masked >> 1;
        }
        
        // Task 9: Syscalls - system call interface
        fn write_message(msg: Message) -> u64 {
            let mut result: u64 = 0;
            unsafe {
                result = syscall(SYSCALL_WRITE, STDOUT, msg.data, msg.len);
            }
            return result;
        }
        
        // Task 10: Inline assembly - basic asm blocks
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
        
        // Task 10: Extended inline assembly - with inputs, outputs, clobbers
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
        
        // Task 11: Naked functions - no prologue/epilogue
        naked fn shellcode_exit() -> u64 {
            unsafe {
                asm {
                    mov rax, 60
                    xor rdi, rdi
                    syscall
                }
            }
            return 0;
        }
        
        // Main function exercising all features
        fn main() -> u64 {
            // Test constants
            let buffer_size: u64 = BUFFER_SIZE;
            let doubled: u64 = DOUBLED;
            
            // Test struct operations
            let text: ptr = 0x1000 as ptr;
            let msg: Message = create_message(text, 13);
            let data_ptr: ptr = msg.data;
            let length: u64 = msg.len;
            
            // Test pointer operations
            let ptr_result: u64 = pointer_operations(text, 100);
            
            // Test control flow and expressions
            let sum: u64 = calculate_sum(10);
            let math_result: u64 = complex_math(5, 10, 3);
            
            // Test type casts
            let addr: u64 = text as u64;
            let back: ptr = addr as ptr;
            
            // Test multiple types
            let a: u8 = 1 as u8;
            let b: u16 = 2 as u16;
            let c: u32 = 3 as u32;
            let d: u64 = (a as u64) + (b as u64) + (c as u64);
            
            // Test nested control flow
            let mut result: u64 = 0;
            if (sum > 50) {
                let mut i: u64 = 0;
                while (i < 5) {
                    if (i == 3) {
                        result = i;
                    }
                    i = i + 1;
                }
            } else {
                result = 0;
            }
            
            // Test syscall
            let syscall_result: u64 = write_message(msg);
            
            // Test inline assembly
            let timestamp: u64 = get_timestamp();
            
            // Return combined result
            return sum + math_result + result + d;
        }
    )";
    
    std::cout << "Testing comprehensive program covering Tasks 1-11...\n";
    
    // Task 2: Lexer
    std::cout << "  Creating lexer and parser...\n";
    Lexer lexer(source);
    Parser parser(lexer);
    std::cout << "  Parsing source code...\n";
    auto program = parser.parse();
    std::cout << "  Parsing complete!\n";
    
    if (parser.hasErrors()) {
        std::cout << "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(!parser.hasErrors());
    
    // Verify parsed structure
    std::cout << "  Parsed: " << program->constants.size() << " constants, " 
              << program->structs.size() << " structs, " 
              << program->functions.size() << " functions\n";
    
    std::cout << "  Functions:\n";
    for (const auto& func : program->functions) {
        std::cout << "    - " << func->name << "\n";
    }
    
    assert(program->constants.size() == 5);  // 5 constants
    assert(program->structs.size() == 2);     // 2 structs
    assert(program->functions.size() == 9);  // 9 functions
    
    // Verify struct definitions
    assert(program->structs[0]->name == "Message");
    assert(program->structs[0]->fields.size() == 3);
    assert(program->structs[0]->isPacked == false);
    
    assert(program->structs[1]->name == "Header");
    assert(program->structs[1]->fields.size() == 3);  // 3 fields: magic, version, size
    assert(program->structs[1]->isPacked == true);
    
    // Verify naked function
    bool foundNakedFunction = false;
    for (const auto& func : program->functions) {
        if (func->name == "shellcode_exit" && func->isNaked) {
            foundNakedFunction = true;
            break;
        }
    }
    assert(foundNakedFunction);
    
    std::cout << "  ✓ Lexer and Parser validation passed\n";
    
    // Task 4: Type checker
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "  ✓ Type checker validation passed\n";
    
    // Task 6-11: IR generation
    IRGenerator generator("comprehensive_test");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    assert(success);
    
    std::cout << "  ✓ IR generation passed\n";
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    bool verifyResult = llvm::verifyModule(*generator.getModule(), &errorStream);
    if (verifyResult) {
        std::cout << "Module verification errors:\n" << errorStr << "\n";
    }
    assert(!verifyResult);
    
    std::cout << "  ✓ LLVM module verification passed\n";
    
    // Get the generated IR as string for validation
    std::string moduleStr;
    llvm::raw_string_ostream moduleStream(moduleStr);
    generator.getModule()->print(moduleStream, nullptr);
    moduleStream.flush();
    
    // Verify key features are present in IR
    
    // Task 14: Constants should be present
    assert(moduleStr.find("BUFFER_SIZE") != std::string::npos || 
           moduleStr.find("1024") != std::string::npos);
    
    // Task 8: Struct types should be defined
    assert(moduleStr.find("Message") != std::string::npos);
    // Note: Header struct is defined but not used, so it may not appear in IR
    
    // Task 9: Syscall inline assembly should be present
    assert(moduleStr.find("syscall") != std::string::npos || 
           moduleStr.find("svc") != std::string::npos);
    
    // Task 10: Inline assembly should be present
    assert(moduleStr.find("asm") != std::string::npos);
    
    // Task 11: Naked function should have naked attribute
    llvm::Function* nakedFunc = generator.getModule()->getFunction("shellcode_exit");
    assert(nakedFunc != nullptr);
    assert(nakedFunc->hasFnAttribute(llvm::Attribute::Naked));
    
    std::cout << "  ✓ Feature validation in IR passed\n";
    
    // Verify all functions were generated
    assert(generator.getModule()->getFunction("create_message") != nullptr);
    assert(generator.getModule()->getFunction("pointer_operations") != nullptr);
    assert(generator.getModule()->getFunction("calculate_sum") != nullptr);
    assert(generator.getModule()->getFunction("complex_math") != nullptr);
    assert(generator.getModule()->getFunction("write_message") != nullptr);
    assert(generator.getModule()->getFunction("get_timestamp") != nullptr);
    assert(generator.getModule()->getFunction("atomic_increment") != nullptr);
    assert(generator.getModule()->getFunction("shellcode_exit") != nullptr);
    assert(generator.getModule()->getFunction("main") != nullptr);
    
    std::cout << "  ✓ All functions generated\n";
    
    std::cout << "\n✓ Comprehensive test passed - All features from Tasks 1-11 working!\n";
}

int main() {
    std::cout << "Running Comprehensive Integration Test...\n\n";
    
    testComprehensiveProgram();
    
    std::cout << "\n✓ Comprehensive test suite completed successfully!\n";
    return 0;
}
