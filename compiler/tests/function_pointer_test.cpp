#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

void testFunctionPointerType() {
    std::string source = R"(
        fn add(x: u64, y: u64) -> u64 {
            return x + y;
        }
        
        fn test() -> u64 {
            let fp: fn(u64, u64) -> u64 = add;
            return 0;
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
    
    std::cout << "✓ Function pointer type test passed\n";
}

void testIndirectCall() {
    std::string source = R"(
        fn add(x: u64, y: u64) -> u64 {
            return x + y;
        }
        
        fn test() -> u64 {
            let fp: fn(u64, u64) -> u64 = add;
            let result: u64 = fp(10, 20);
            return result;
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
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Indirect call test passed\n";
}

void testFunctionPointerCast() {
    std::string source = R"(
        fn callback() -> u64 {
            return 42;
        }
        
        fn test() -> ptr {
            let fp: fn() -> u64 = callback;
            let p: ptr = fp as ptr;
            return p;
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
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Function pointer cast test passed\n";
}

void testFunctionPointerFromPtr() {
    std::string source = R"(
        fn test(addr: ptr) -> u64 {
            let fp: fn() -> u64 = addr as fn() -> u64;
            let result: u64 = fp();
            return result;
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
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Function pointer from ptr test passed\n";
}

void testMultipleFunctionPointers() {
    std::string source = R"(
        fn add(x: u64, y: u64) -> u64 {
            return x + y;
        }
        
        fn sub(x: u64, y: u64) -> u64 {
            return x - y;
        }
        
        fn compute(a: u64, b: u64, use_add: u64) -> u64 {
            let mut fp: fn(u64, u64) -> u64 = add;
            if (use_add == 0) {
                fp = sub;
            }
            return fp(a, b);
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
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Multiple function pointers test passed\n";
}

int main() {
    std::cout << "Running Function Pointer tests...\n\n";
    
    testFunctionPointerType();
    testIndirectCall();
    testFunctionPointerCast();
    testFunctionPointerFromPtr();
    testMultipleFunctionPointers();
    
    std::cout << "\n✓ All Function Pointer tests passed!\n";
    return 0;
}
