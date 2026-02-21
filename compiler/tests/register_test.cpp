#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

void testRegisterDeclaration() {
    std::string source = R"(
        fn test() -> u64 {
            let rax: register;
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
    
    std::cout << "✓ Register declaration test passed\n";
}

void testRegisterRead() {
    std::string source = R"(
        fn get_rax() -> u64 {
            let rax: register;
            return rax;
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
    
    // Verify inline assembly is present
    std::string moduleStr;
    llvm::raw_string_ostream moduleStream(moduleStr);
    generator.getModule()->print(moduleStream, nullptr);
    moduleStream.flush();
    
    assert(moduleStr.find("call") != std::string::npos);
    assert(moduleStr.find("asm") != std::string::npos);
    
    std::cout << "✓ Register read test passed\n";
}

void testRegisterWrite() {
    std::string source = R"(
        fn set_rax(value: u64) -> u64 {
            let mut rax: register;
            rax = value;
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
    
    std::cout << "✓ Register write test passed\n";
}

void testRegisterInitialization() {
    std::string source = R"(
        fn init_rax() -> u64 {
            let rax: register = 42;
            return rax;
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
    
    std::cout << "✓ Register initialization test passed\n";
}

void testMultipleRegisters() {
    std::string source = R"(
        fn swap_registers() -> u64 {
            let mut rax: register = 10;
            let mut rbx: register = 20;
            let temp: u64 = rax;
            rax = rbx;
            rbx = temp;
            return rax;
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
    
    std::cout << "✓ Multiple registers test passed\n";
}

int main() {
    std::cout << "Running Register Access tests...\n\n";
    
    testRegisterDeclaration();
    testRegisterRead();
    testRegisterWrite();
    testRegisterInitialization();
    testMultipleRegisters();
    
    std::cout << "\n✓ All Register Access tests passed!\n";
    return 0;
}
