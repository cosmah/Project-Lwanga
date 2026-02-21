#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <cassert>
#include <iostream>

using namespace lwanga;

void testUnsafeBlockParsing() {
    std::string source = R"(
        fn test() -> u64 {
            unsafe {
                let x: u64 = 42;
                return x;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    if (parser.hasErrors()) {
        std::cout << "Parser errors:\n";
        for (const auto& error : parser.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Unsafe block parsing test passed\n";
}

void testPointerDerefRequiresUnsafe() {
    std::string source = R"(
        fn test(p: ptr) -> u64 {
            return *p;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    // Should fail - pointer dereference outside unsafe block
    assert(!typeCheckResult);
    assert(checker.getErrors().size() > 0);
    
    // Check error message
    bool foundError = false;
    for (const auto& error : checker.getErrors()) {
        if (error.message.find("Pointer operations must be in unsafe block") != std::string::npos) {
            foundError = true;
            break;
        }
    }
    assert(foundError);
    
    std::cout << "✓ Pointer dereference requires unsafe test passed\n";
}

void testPointerDerefInUnsafe() {
    std::string source = R"(
        fn test(p: ptr) -> u64 {
            unsafe {
                return *p;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Pointer dereference in unsafe test passed\n";
}

void testAddressOfRequiresUnsafe() {
    std::string source = R"(
        fn test() -> ptr {
            let x: u64 = 42;
            return &x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    // Should fail - address-of outside unsafe block
    assert(!typeCheckResult);
    assert(checker.getErrors().size() > 0);
    
    std::cout << "✓ Address-of requires unsafe test passed\n";
}

void testAddressOfInUnsafe() {
    std::string source = R"(
        fn test() -> ptr {
            let x: u64 = 42;
            unsafe {
                return &x;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Address-of in unsafe test passed\n";
}

void testInlineAsmRequiresUnsafe() {
    std::string source = R"(
        fn test() -> u64 {
            asm {
                nop
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    // Should fail - inline assembly outside unsafe block
    assert(!typeCheckResult);
    assert(checker.getErrors().size() > 0);
    
    bool foundError = false;
    for (const auto& error : checker.getErrors()) {
        if (error.message.find("Inline assembly must be in unsafe block") != std::string::npos) {
            foundError = true;
            break;
        }
    }
    assert(foundError);
    
    std::cout << "✓ Inline assembly requires unsafe test passed\n";
}

void testInlineAsmInUnsafe() {
    std::string source = R"(
        fn test() -> u64 {
            unsafe {
                asm {
                    nop
                }
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Inline assembly in unsafe test passed\n";
}

void testInlineAsmInNakedFunction() {
    std::string source = R"(
        naked fn shellcode() -> u64 {
            asm {
                mov rax, 60
                xor rdi, rdi
                syscall
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Inline assembly in naked function test passed\n";
}

void testSyscallRequiresUnsafe() {
    std::string source = R"(
        fn test() -> u64 {
            return syscall(60, 0);
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    // Should fail - syscall outside unsafe block
    assert(!typeCheckResult);
    assert(checker.getErrors().size() > 0);
    
    bool foundError = false;
    for (const auto& error : checker.getErrors()) {
        if (error.message.find("Syscall must be in unsafe block") != std::string::npos) {
            foundError = true;
            break;
        }
    }
    assert(foundError);
    
    std::cout << "✓ Syscall requires unsafe test passed\n";
}

void testSyscallInUnsafe() {
    std::string source = R"(
        fn test() -> u64 {
            unsafe {
                return syscall(60, 0);
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Syscall in unsafe test passed\n";
}

void testArrayIndexRequiresUnsafe() {
    std::string source = R"(
        fn test(arr: ptr, idx: u64) -> u64 {
            return arr[idx];
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    // Should fail - array indexing outside unsafe block
    assert(!typeCheckResult);
    assert(checker.getErrors().size() > 0);
    
    bool foundError = false;
    for (const auto& error : checker.getErrors()) {
        if (error.message.find("Array indexing must be in unsafe block") != std::string::npos) {
            foundError = true;
            break;
        }
    }
    assert(foundError);
    
    std::cout << "✓ Array indexing requires unsafe test passed\n";
}

void testArrayIndexInUnsafe() {
    std::string source = R"(
        fn test(arr: ptr, idx: u64) -> u64 {
            unsafe {
                return arr[idx];
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Array indexing in unsafe test passed\n";
}

void testNestedUnsafeBlocks() {
    std::string source = R"(
        fn test(p: ptr) -> u64 {
            unsafe {
                let x: u64 = *p;
                unsafe {
                    let y: u64 = *p;
                    return x + y;
                }
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Nested unsafe blocks test passed\n";
}

void testMixedSafeAndUnsafe() {
    std::string source = R"(
        fn test(p: ptr) -> u64 {
            let a: u64 = 10;
            let b: u64 = 20;
            unsafe {
                let c: u64 = *p;
                return a + b + c;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    
    std::cout << "✓ Mixed safe and unsafe test passed\n";
}

int main() {
    std::cout << "Running Unsafe Block tests...\n\n";
    
    testUnsafeBlockParsing();
    testPointerDerefRequiresUnsafe();
    testPointerDerefInUnsafe();
    testAddressOfRequiresUnsafe();
    testAddressOfInUnsafe();
    testInlineAsmRequiresUnsafe();
    testInlineAsmInUnsafe();
    testInlineAsmInNakedFunction();
    testSyscallRequiresUnsafe();
    testSyscallInUnsafe();
    testArrayIndexRequiresUnsafe();
    testArrayIndexInUnsafe();
    testNestedUnsafeBlocks();
    testMixedSafeAndUnsafe();
    
    std::cout << "\n✓ All Unsafe Block tests passed!\n";
    return 0;
}
