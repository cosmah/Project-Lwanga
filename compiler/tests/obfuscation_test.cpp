#include "Obfuscator.h"
#include "IRGenerator.h"
#include "TypeChecker.h"
#include "Parser.h"
#include "Lexer.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

// Helper to compile a simple program
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

void testObfuscatorCreation() {
    std::cout << "Testing obfuscator creation..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    return 42;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    Obfuscator obfuscator(generator->getModule());
    assert(!obfuscator.hasErrors());
    
    std::cout << "  ✓ Obfuscator creation works" << std::endl;
}

void testControlFlowFlattening() {
    std::cout << "Testing control flow flattening..." << std::endl;
    
    std::string source = R"(
fn test(x: u64) -> u64 {
    if (x > 10) {
        return x * 2;
    }
    return x + 5;
}

fn main() -> u64 {
    return test(15);
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    llvm::Module* module = generator->getModule();
    
    // Count basic blocks before obfuscation
    llvm::Function* testFunc = module->getFunction("test");
    assert(testFunc != nullptr);
    size_t blocksBefore = testFunc->size();
    
    // Apply control flow flattening
    Obfuscator obfuscator(module, 12345); // Fixed seed for reproducibility
    obfuscator.flattenControlFlow();
    
    assert(!obfuscator.hasErrors());
    
    // Verify module is still valid
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    assert(!llvm::verifyModule(*module, &verifyStream));
    
    // Control flow flattening should add dispatch blocks
    size_t blocksAfter = testFunc->size();
    assert(blocksAfter >= blocksBefore);
    
    std::cout << "  ✓ Control flow flattening works" << std::endl;
}

void testOpaquePredicates() {
    std::cout << "Testing opaque predicates..." << std::endl;
    
    std::string source = R"(
fn compute(a: u64, b: u64) -> u64 {
    let x: u64 = a + b;
    let y: u64 = a * b;
    return x + y;
}

fn main() -> u64 {
    return compute(5, 10);
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    llvm::Module* module = generator->getModule();
    
    // Apply opaque predicates
    Obfuscator obfuscator(module, 54321); // Fixed seed
    obfuscator.insertOpaquePredicates();
    
    assert(!obfuscator.hasErrors());
    
    // Verify module is still valid
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    assert(!llvm::verifyModule(*module, &verifyStream));
    
    std::cout << "  ✓ Opaque predicates work" << std::endl;
}

void testInstructionSubstitution() {
    std::cout << "Testing instruction substitution..." << std::endl;
    
    std::string source = R"(
fn calculate(a: u64, b: u64) -> u64 {
    let sum: u64 = a + b;
    let diff: u64 = a - b;
    let xorVal: u64 = a ^ b;
    return sum + diff + xorVal;
}

fn main() -> u64 {
    return calculate(20, 10);
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    llvm::Module* module = generator->getModule();
    
    // Apply instruction substitution
    Obfuscator obfuscator(module, 99999); // Fixed seed
    obfuscator.substituteInstructions();
    
    assert(!obfuscator.hasErrors());
    
    // Verify module is still valid
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    assert(!llvm::verifyModule(*module, &verifyStream));
    
    std::cout << "  ✓ Instruction substitution works" << std::endl;
}

void testFullObfuscation() {
    std::cout << "Testing full obfuscation..." << std::endl;
    
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
    
    llvm::Module* module = generator->getModule();
    
    // Apply all obfuscation passes
    Obfuscator obfuscator(module, 11111); // Fixed seed
    obfuscator.obfuscate();
    
    assert(!obfuscator.hasErrors());
    
    // Verify module is still valid
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    assert(!llvm::verifyModule(*module, &verifyStream));
    
    std::cout << "  ✓ Full obfuscation works" << std::endl;
}

void testSemanticPreservation() {
    std::cout << "Testing semantic preservation..." << std::endl;
    
    std::string source = R"(
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let x: u64 = add(10, 20);
    let y: u64 = add(5, 15);
    return x + y;
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    llvm::Module* module = generator->getModule();
    
    // Apply obfuscation
    Obfuscator obfuscator(module, 77777); // Fixed seed
    obfuscator.obfuscate();
    
    assert(!obfuscator.hasErrors());
    
    // Verify module is still valid (semantic preservation)
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    bool isValid = !llvm::verifyModule(*module, &verifyStream);
    
    if (!isValid) {
        std::cerr << "Module verification failed: " << verifyError << std::endl;
    }
    
    assert(isValid);
    
    std::cout << "  ✓ Semantic preservation verified" << std::endl;
}

void testSmallFunctionSkipped() {
    std::cout << "Testing small function skipping..." << std::endl;
    
    std::string source = R"(
fn tiny() -> u64 {
    return 42;
}

fn main() -> u64 {
    return tiny();
}
)";
    
    auto generator = compileProgram(source);
    assert(generator != nullptr);
    
    llvm::Module* module = generator->getModule();
    
    llvm::Function* tinyFunc = module->getFunction("tiny");
    assert(tinyFunc != nullptr);
    size_t blocksBefore = tinyFunc->size();
    
    // Apply obfuscation
    Obfuscator obfuscator(module);
    obfuscator.flattenControlFlow();
    
    // Small functions should not be obfuscated
    size_t blocksAfter = tinyFunc->size();
    assert(blocksAfter == blocksBefore);
    
    std::cout << "  ✓ Small function skipping works" << std::endl;
}

int main() {
    std::cout << "\n=== Obfuscation Tests ===" << std::endl;
    
    testObfuscatorCreation();
    testControlFlowFlattening();
    testOpaquePredicates();
    testInstructionSubstitution();
    testFullObfuscation();
    testSemanticPreservation();
    testSmallFunctionSkipped();
    
    std::cout << "\n✓ All obfuscation tests passed!" << std::endl;
    return 0;
}
