#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <cassert>
#include <iostream>
#include <sstream>

using namespace lwanga;

void testSimpleFunction() {
    std::string source = R"(
        fn add(x: u32, y: u32) -> u32 {
            return x + y;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    assert(program->functions[0]->name == "add");
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    if (!success) {
        std::cout << "Type errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    assert(success);
    std::cout << "✓ Simple function integration test passed\n";
}

void testVariableDeclarations() {
    std::string source = R"(
        fn test() -> u64 {
            let x: u64 = 42;
            let mut y: u64 = 100;
            y = y + 1;
            return y;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    if (!success) {
        std::cout << "Type errors in testVariableDeclarations:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    assert(success);
    std::cout << "✓ Variable declarations integration test passed\n";
}

void testControlFlow() {
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
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Control flow integration test passed\n";
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
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ While loop integration test passed\n";
}

void testStructDefinition() {
    std::string source = R"(
        struct Point {
            x: u32,
            y: u32
        }
        
        fn create_point() -> Point {
            return Point { x: 10, y: 20 };
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->structs.size() == 1);
    assert(program->structs[0]->name == "Point");
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Struct definition integration test passed\n";
}

void testFieldAccess() {
    std::string source = R"(
        struct Vec2 {
            x: u64,
            y: u64
        }
        
        fn get_x(v: Vec2) -> u64 {
            return v.x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Field access integration test passed\n";
}

void testUnsafeBlock() {
    std::string source = R"(
        fn deref_ptr(p: ptr) -> u64 {
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
    bool success = checker.check(program.get());
    
    if (!success) {
        std::cout << "Type errors in testUnsafeBlock:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    assert(success);
    std::cout << "✓ Unsafe block integration test passed\n";
}

void testSyscall() {
    std::string source = R"(
        fn write_stdout(msg: ptr, len: u64) -> u64 {
            unsafe {
                return syscall(1, 1, msg, len);
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Syscall integration test passed\n";
}

void testTypeCast() {
    std::string source = R"(
        fn ptr_to_int(p: ptr) -> u64 {
            return p as u64;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Type cast integration test passed\n";
}

void testArrayIndexing() {
    std::string source = R"(
        fn get_element(arr: ptr, idx: u64) -> u64 {
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
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Array indexing integration test passed\n";
}

void testNakedFunction() {
    std::string source = R"(
        naked fn entry() -> u64 {
            asm {
                "mov rax, 60"
                "xor rdi, rdi"
                "syscall"
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions[0]->isNaked);
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    if (!success) {
        std::cout << "Type errors in testNakedFunction:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    assert(success);
    std::cout << "✓ Naked function integration test passed\n";
}

void testConstantDeclaration() {
    std::string source = R"(
        const MAX_SIZE: u64 = 1024;
        
        fn get_max() -> u64 {
            return MAX_SIZE;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->constants.size() == 1);
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Constant declaration integration test passed\n";
}

void testMultipleFunctions() {
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
    assert(program->functions.size() == 2);
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    if (!success) {
        std::cout << "Type errors in testMultipleFunctions:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    
    assert(success);
    std::cout << "✓ Multiple functions integration test passed\n";
}

void testTypeError_ImmutableAssignment() {
    std::string source = R"(
        fn test() -> u64 {
            let x: u64 = 10;
            x = 20;
            return x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    // Should fail - cannot assign to immutable variable
    assert(!success);
    assert(checker.hasErrors());
    std::cout << "✓ Immutable assignment error detection passed\n";
}

void testTypeError_TypeMismatch() {
    std::string source = R"(
        fn test() -> u32 {
            let x: u32 = 10;
            let y: u64 = 20;
            return y;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    // Should fail - return type mismatch (u64 vs u32)
    assert(!success);
    assert(checker.hasErrors());
    std::cout << "✓ Type mismatch error detection passed\n";
}

void testTypeError_UnsafeOutsideBlock() {
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
    bool success = checker.check(program.get());
    
    // Should fail - pointer dereference outside unsafe block
    assert(!success);
    assert(checker.hasErrors());
    std::cout << "✓ Unsafe operation error detection passed\n";
}

void testTypeError_UndefinedVariable() {
    std::string source = R"(
        fn test() -> u64 {
            return undefined_var;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    // Should fail - undefined variable
    assert(!success);
    assert(checker.hasErrors());
    std::cout << "✓ Undefined variable error detection passed\n";
}

void testComplexExpression() {
    std::string source = R"(
        fn compute(a: u64, b: u64, c: u64) -> u64 {
            return (a + b) * c - (a & b) | (c ^ 0xFF);
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Complex expression integration test passed\n";
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
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Nested control flow integration test passed\n";
}

void testPointerArithmetic() {
    std::string source = R"(
        fn advance_ptr(p: ptr, offset: u64) -> ptr {
            return p + offset;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Pointer arithmetic integration test passed\n";
}

void testEncryptedString() {
    std::string source = R"(
        fn get_secret() -> ptr {
            return enc { "secret_password" };
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    bool success = checker.check(program.get());
    
    assert(success);
    std::cout << "✓ Encrypted string integration test passed\n";
}

int main() {
    std::cout << "Running Integration Tests (Lexer + Parser + TypeChecker)...\n\n";
    
    // Positive tests
    testSimpleFunction();
    testVariableDeclarations();
    testControlFlow();
    testWhileLoop();
    testStructDefinition();
    testFieldAccess();
    testUnsafeBlock();
    testSyscall();
    testTypeCast();
    testArrayIndexing();
    testNakedFunction();
    testConstantDeclaration();
    testMultipleFunctions();
    testComplexExpression();
    testNestedControlFlow();
    testPointerArithmetic();
    testEncryptedString();
    
    std::cout << "\n";
    
    // Negative tests (error detection)
    testTypeError_ImmutableAssignment();
    testTypeError_TypeMismatch();
    testTypeError_UnsafeOutsideBlock();
    testTypeError_UndefinedVariable();
    
    std::cout << "\n✓ All integration tests passed!\n";
    std::cout << "✓ Lexer, Parser, and TypeChecker work together correctly\n";
    
    return 0;
}
