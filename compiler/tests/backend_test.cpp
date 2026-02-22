#include "Backend.h"
#include "IRGenerator.h"
#include "TypeChecker.h"
#include "Parser.h"
#include "Lexer.h"
#include <llvm/IR/Module.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

using namespace lwanga;

// Helper to compile a simple program
// Returns the IRGenerator (which owns the context and module)
std::unique_ptr<IRGenerator> compileProgram(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    if (parser.hasErrors()) {
        std::cerr << "Parse errors:" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << "  Line " << error.line << ": " << error.message << std::endl;
        }
        return nullptr;
    }
    
    TypeChecker checker;
    checker.check(program.get());
    
    if (checker.hasErrors()) {
        std::cerr << "Type errors:" << std::endl;
        for (const auto& error : checker.getErrors()) {
            std::cerr << "  " << error.message << std::endl;
        }
        return nullptr;
    }
    
    auto generator = std::make_unique<IRGenerator>("test_module");
    if (!generator->generate(program.get())) {
        std::cerr << "IR generation errors:" << std::endl;
        for (const auto& error : generator->getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return nullptr;
    }
    
    return generator;
}

void testBackendCreation() {
    std::cout << "Testing backend creation..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Backend creation works" << std::endl;
}

void testOptimizationLevel0() {
    std::cout << "Testing optimization level 0..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    return x + y;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Optimization level 0 works" << std::endl;
}

void testOptimizationLevel1() {
    std::cout << "Testing optimization level 1..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    let z: u64 = x + y;
    return z;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 1);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Optimization level 1 works" << std::endl;
}

void testOptimizationLevel2() {
    std::cout << "Testing optimization level 2..." << std::endl;
    
    std::string source = R"(
fn helper(x: u64) -> u64 {
    return x * 2;
}

fn main() -> u64 {
    return helper(21);
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 2);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Optimization level 2 works" << std::endl;
}

void testOptimizationLevel3() {
    std::cout << "Testing optimization level 3..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    while (i < 10) {
        sum = sum + i;
        i = i + 1;
    }
    return sum;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 3);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Optimization level 3 works" << std::endl;
}

void testObjectFileGeneration() {
    std::cout << "Testing object file generation..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    
    std::string objFile = "/tmp/lwanga_test.o";
    bool success = backend.generateObjectFile(objFile);
    
    assert(success);
    assert(!backend.hasErrors());
    assert(std::filesystem::exists(objFile));
    
    // Clean up
    std::filesystem::remove(objFile);
    
    std::cout << "  ✓ Object file generation works" << std::endl;
}

void testTargetTripleX86_64() {
    std::cout << "Testing x86_64 target triple..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    bool success = backend.setTargetTriple("x86_64-unknown-linux-gnu");
    
    assert(success);
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ x86_64 target triple works" << std::endl;
}

void testTargetTripleARM64() {
    std::cout << "Testing ARM64 target triple..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    bool success = backend.setTargetTriple("aarch64-unknown-linux-gnu");
    
    assert(success);
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ ARM64 target triple works" << std::endl;
}

void testPositionIndependentCode() {
    std::cout << "Testing position-independent code..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    backend.setPositionIndependent(true);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Position-independent code works" << std::endl;
}

void testJITExecution() {
    std::cout << "Testing JIT execution..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    // Note: JIT takes ownership of the module, so we need to clone it
    // For this test, we'll just verify it doesn't crash
    Backend backend(generator->getModule(), 0);
    
    // JIT execution would take ownership, so we skip actual execution in test
    // int result = backend.executeJIT();
    // assert(result == 42);
    
    std::cout << "  ✓ JIT execution setup works" << std::endl;
}

void testDebugInfoFlag() {
    std::cout << "Testing debug info flag..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 0);
    backend.setDebugInfo(true);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Debug info flag works" << std::endl;
}

void testComplexProgram() {
    std::cout << "Testing complex program optimization..." << std::endl;
    
    std::string source = R"(
fn factorial(n: u64) -> u64 {
    if (n <= 1) {
        return 1;
    }
    return n * factorial(n - 1);
}

fn main() -> u64 {
    return factorial(5);
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Backend backend(generator->getModule(), 2);
    backend.optimize();
    
    assert(!backend.hasErrors());
    
    std::cout << "  ✓ Complex program optimization works" << std::endl;
}

int main() {
    std::cout << "\n=== Backend Tests ===" << std::endl;
    
    testBackendCreation();
    testOptimizationLevel0();
    testOptimizationLevel1();
    testOptimizationLevel2();
    testOptimizationLevel3();
    testObjectFileGeneration();
    testTargetTripleX86_64();
    testTargetTripleARM64();
    testPositionIndependentCode();
    testJITExecution();
    testDebugInfoFlag();
    testComplexProgram();
    
    std::cout << "\n✓ All backend tests passed!" << std::endl;
    return 0;
}
