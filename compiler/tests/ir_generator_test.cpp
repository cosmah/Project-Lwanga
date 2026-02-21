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
        const HALVED: u64 = MAX_SIZE / 2;
        const REMAINDER: u64 = MAX_SIZE % 100;
        const SHIFTED_LEFT: u64 = MAX_SIZE << 2;
        const SHIFTED_RIGHT: u64 = MAX_SIZE >> 1;
        const BITWISE_AND: u64 = MAX_SIZE & 255;
        const BITWISE_OR: u64 = MAX_SIZE | 15;
        const BITWISE_XOR: u64 = MAX_SIZE ^ 511;
        const MULTIPLIED: u64 = MAX_SIZE * 3;
        const SUBTRACTED: u64 = MAX_SIZE - 24;
        
        fn get_max() -> u64 {
            return MAX_SIZE;
        }
        
        fn get_doubled() -> u64 {
            return DOUBLED;
        }
        
        fn get_all_ops() -> u64 {
            return HALVED + REMAINDER + SHIFTED_LEFT + SHIFTED_RIGHT + 
                   BITWISE_AND + BITWISE_OR + BITWISE_XOR + MULTIPLIED + SUBTRACTED;
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
    
    std::cout << "✓ Constants test passed (all arithmetic and bitwise operations)\n";
}

void testPointerOperations() {
    std::string source = R"(
        fn pointer_test() -> u64 {
            let x: u64 = 42;
            unsafe {
                let p: ptr = &x;
                let y: u64 = *p;
                return y;
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
    
    std::cout << "✓ Pointer operations test passed\n";
}

void testPointerArithmetic() {
    std::string source = R"(
        fn pointer_arithmetic(base: ptr, offset: u64) -> ptr {
            let p1: ptr = base + offset;
            let p2: ptr = p1 - 10;
            return p2;
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
    
    std::cout << "✓ Pointer arithmetic test passed\n";
}

void testPointerDifference() {
    std::string source = R"(
        fn pointer_diff(p1: ptr, p2: ptr) -> u64 {
            return p1 - p2;
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
    
    std::cout << "✓ Pointer difference test passed\n";
}

void testPointerCasts() {
    std::string source = R"(
        fn pointer_casts(addr: u64) -> ptr {
            let p: ptr = addr as ptr;
            let n: u64 = p as u64;
            let p2: ptr = n as ptr;
            return p2;
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
    
    std::cout << "✓ Pointer casts test passed\n";
}

void testAddressOfDereference() {
    std::string source = R"(
        fn address_of_deref(p: ptr) -> ptr {
            unsafe {
                let x: u64 = *p;
                let addr: ptr = &x;
                return addr;
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
    
    std::cout << "✓ Address-of dereference test passed\n";
}

void testComplexLValueAssignment() {
    std::string source = R"(
        fn complex_assignment(p: ptr) -> u64 {
            unsafe {
                *p = 42;
                return *p;
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
    
    std::cout << "✓ Complex lvalue assignment test passed\n";
}

void testStructOperations() {
    std::string source = R"(
        struct Point {
            x: u64,
            y: u64
        }
        
        fn create_point() -> Point {
            return Point { x: 10, y: 20 };
        }
        
        fn get_x(p: Point) -> u64 {
            return p.x;
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
    
    std::cout << "✓ Struct operations test passed\n";
}

void testPackedStruct() {
    std::string source = R"(
        packed struct Data {
            flag: u8,
            value: u32
        }
        
        fn create_data() -> Data {
            return Data { flag: 1, value: 42 };
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
    
    std::cout << "✓ Packed struct test passed\n";
}

void testNestedStructs() {
    std::string source = R"(
        struct Inner {
            value: u64
        }
        
        struct Outer {
            inner: Inner,
            extra: u64
        }
        
        fn create_nested() -> Outer {
            let i: Inner = Inner { value: 42 };
            return Outer { inner: i, extra: 100 };
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
    
    std::cout << "✓ Nested structs test passed\n";
}

void testSyscallBasic() {
    std::string source = R"(
        fn test_write() -> u64 {
            let msg: ptr = 0x1000 as ptr;
            let mut result: u64 = 0;
            unsafe {
                result = syscall(1, 1, msg, 13);
            }
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
    
    std::cout << "✓ Basic syscall test passed\n";
}

void testSyscallMultipleArgs() {
    std::string source = R"(
        fn test_syscall_6args() -> u64 {
            let mut result: u64 = 0;
            unsafe {
                result = syscall(9, 0x1000, 4096, 3, 34, 0, 0);
            }
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
    
    std::cout << "✓ Syscall with multiple arguments test passed\n";
}

void testSyscallWithVariables() {
    std::string source = R"(
        fn test_syscall_vars() -> u64 {
            let fd: u64 = 1;
            let buf: ptr = 0x2000 as ptr;
            let count: u64 = 100;
            let mut result: u64 = 0;
            unsafe {
                result = syscall(1, fd, buf, count);
            }
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
    
    std::cout << "✓ Syscall with variables test passed\n";
}

void testSyscallInUnsafeBlock() {
    std::string source = R"(
        fn test_unsafe_syscall() -> u64 {
            let mut result: u64 = 0;
            unsafe {
                result = syscall(60, 0);
            }
            return result;
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
    
    std::cout << "✓ Syscall in unsafe block test passed\n";
}

void testSyscallX86_64() {
    std::string source = R"(
        fn test_x86_syscall() -> u64 {
            let mut result: u64 = 0;
            unsafe {
                result = syscall(1, 1, 0x1000, 10);
            }
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
    
    // Set x86_64 target triple
    generator.getModule()->setTargetTriple("x86_64-unknown-linux-gnu");
    
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    // Verify that the IR contains x86_64 syscall instruction
    std::string moduleStr;
    llvm::raw_string_ostream moduleStream(moduleStr);
    generator.getModule()->print(moduleStream, nullptr);
    moduleStream.flush();
    
    assert(moduleStr.find("syscall") != std::string::npos);
    assert(moduleStr.find("rax") != std::string::npos);
    
    std::cout << "✓ x86_64 syscall generation test passed\n";
}

void testSyscallARM64() {
    std::string source = R"(
        fn test_arm_syscall() -> u64 {
            let mut result: u64 = 0;
            unsafe {
                result = syscall(64, 1, 0x2000, 20);
            }
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
    
    // Set ARM64 target triple
    generator.getModule()->setTargetTriple("aarch64-unknown-linux-gnu");
    
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    
    assert(success);
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    // Verify that the IR contains ARM64 svc instruction
    std::string moduleStr;
    llvm::raw_string_ostream moduleStream(moduleStr);
    generator.getModule()->print(moduleStream, nullptr);
    moduleStream.flush();
    
    assert(moduleStr.find("svc") != std::string::npos);
    assert(moduleStr.find("x0") != std::string::npos || moduleStr.find("x8") != std::string::npos);
    
    std::cout << "✓ ARM64 syscall generation test passed\n";
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
    testPointerOperations();
    testPointerArithmetic();
    testPointerDifference();
    testPointerCasts();
    testAddressOfDereference();
    testComplexLValueAssignment();
    testStructOperations();
    testPackedStruct();
    testNestedStructs();
    testSyscallBasic();
    testSyscallMultipleArgs();
    testSyscallWithVariables();
    testSyscallInUnsafeBlock();
    testSyscallX86_64();
    testSyscallARM64();
    
    std::cout << "\n✓ All IR Generator tests passed!\n";
    return 0;
}

