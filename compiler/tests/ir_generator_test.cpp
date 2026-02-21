#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

void testSimpleFunctionGeneration() {
    std::string source = R"(
        fn add(x: u64, y: u64) -> u64 {
            return x + y;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    assert(generator.getModule() != nullptr);
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Simple function generation test passed\n";
}

void testVariableDeclaration() {
    std::string source = R"(
        fn test() -> u64 {
            let x: u64 = 42;
            return x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Variable declaration test passed\n";
}

void testIfStatement() {
    std::string source = R"(
        fn max(a: u64, b: u64) -> u64 {
            if (a > b) {
                return a;
            } else {
                return b;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ If statement test passed\n";
}

void testWhileLoop() {
    std::string source = R"(
        fn countdown(n: u64) -> u64 {
            let mut i: u64 = n;
            while (i > 0) {
                i = i - 1;
            }
            return i;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ While loop test passed\n";
}

void testFunctionCall() {
    std::string source = R"(
        fn helper(x: u64) -> u64 {
            return x * 2;
        }
        
        fn main() -> u64 {
            let result: u64 = helper(21);
            return result;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Function call test passed\n";
}

void testComplexExpression() {
    std::string source = R"(
        fn compute(a: u64, b: u64, c: u64) -> u64 {
            return (a + b) * c - (a & b) | (c ^ 255);
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Complex expression test passed\n";
}

void testTypeCast() {
    std::string source = R"(
        fn cast_test(x: u32) -> u64 {
            return x as u64;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Type cast test passed\n";
}

void testNestedControlFlow() {
    std::string source = R"(
        fn nested(x: u64) -> u64 {
            if (x > 10) {
                let mut i: u64 = 0;
                while (i < x) {
                    if (i == 5) {
                        return i;
                    }
                    i = i + 1;
                }
                return i;
            } else {
                return 0;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Nested control flow test passed\n";
}

void testMultipleTypes() {
    std::string source = R"(
        fn test_types(a: u8, b: u16, c: u32, d: u64) -> u64 {
            return (a as u64) + (b as u64) + (c as u64) + d;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("test_module");
    bool success = generator.generate(program.get());
    
    assert(success);
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "✓ Multiple types test passed\n";
}

void testConstants() {
    std::string source = R"(
        const MAX_SIZE: u64 = 1024;
        const DOUBLED: u64 = MAX_SIZE + MAX_SIZE;
        
        fn get_max() -> u64 {
            return MAX_SIZE;
        }
        
        fn get_doubled() -> u64 {
            return DOUBLED;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
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
    
    std::cout << "✓ Constants test passed\n";
}

int main() {
    std::cout << "Running IR Generator tests...\n\n";
    
    testSimpleFunctionGeneration();
    testVariableDeclaration();
    testIfStatement();
    testWhileLoop();
    testFunctionCall();
    testComplexExpression();
    testTypeCast();
    testNestedControlFlow();
    testMultipleTypes();
    testConstants();
    
    std::cout << "\n✓ All IR Generator tests passed!\n";
    return 0;
}
