#include "Preprocessor.h"
#include <cassert>
#include <iostream>
#include <string>

using namespace lwanga;

void testBasicIfDef() {
    std::cout << "Testing basic #if with defined symbol..." << std::endl;
    
    std::string source = R"(
#define TEST_SYMBOL
#if TEST_SYMBOL
let x: u32 = 42;
#endif
)";
    
    Preprocessor pp(source);
    std::string result = pp.process();
    
    // Should include the let statement
    assert(result.find("let x: u32 = 42;") != std::string::npos);
    // Should not include the directives
    assert(result.find("#define") == std::string::npos);
    assert(result.find("#if") == std::string::npos);
    assert(result.find("#endif") == std::string::npos);
    
    std::cout << "  ✓ Basic #if with defined symbol works" << std::endl;
}

void testIfDefUndefined() {
    std::cout << "Testing #if with undefined symbol..." << std::endl;
    
    std::string source = R"(
#if UNDEFINED_SYMBOL
let x: u32 = 42;
#endif
let y: u32 = 100;
)";
    
    Preprocessor pp(source);
    std::string result = pp.process();
    
    // Should not include the let x statement
    assert(result.find("let x: u32 = 42;") == std::string::npos);
    // Should include the let y statement
    assert(result.find("let y: u32 = 100;") != std::string::npos);
    
    std::cout << "  ✓ #if with undefined symbol excludes code" << std::endl;
}

void testIfElse() {
    std::cout << "Testing #if #else..." << std::endl;
    
    std::string source1 = R"(
#define FEATURE_ENABLED
#if FEATURE_ENABLED
let x: u32 = 1;
#else
let x: u32 = 2;
#endif
)";
    
    Preprocessor pp1(source1);
    std::string result1 = pp1.process();
    
    // Should include the first branch
    assert(result1.find("let x: u32 = 1;") != std::string::npos);
    assert(result1.find("let x: u32 = 2;") == std::string::npos);
    
    std::string source2 = R"(
#if FEATURE_DISABLED
let x: u32 = 1;
#else
let x: u32 = 2;
#endif
)";
    
    Preprocessor pp2(source2);
    std::string result2 = pp2.process();
    
    // Should include the else branch
    assert(result2.find("let x: u32 = 1;") == std::string::npos);
    assert(result2.find("let x: u32 = 2;") != std::string::npos);
    
    std::cout << "  ✓ #if #else works correctly" << std::endl;
}

void testNestedIf() {
    std::cout << "Testing nested #if..." << std::endl;
    
    std::string source = R"(
#define OUTER
#define INNER
#if OUTER
let a: u32 = 1;
#if INNER
let b: u32 = 2;
#endif
let c: u32 = 3;
#endif
)";
    
    Preprocessor pp(source);
    std::string result = pp.process();
    
    // Should include all statements
    assert(result.find("let a: u32 = 1;") != std::string::npos);
    assert(result.find("let b: u32 = 2;") != std::string::npos);
    assert(result.find("let c: u32 = 3;") != std::string::npos);
    
    std::cout << "  ✓ Nested #if works correctly" << std::endl;
}

void testPlatformLinux() {
    std::cout << "Testing PLATFORM_LINUX..." << std::endl;
    
    std::string source = R"(
#if PLATFORM_LINUX
let syscall_num: u64 = 1;
#endif
)";
    
    // Default target should be Linux
    Preprocessor pp1(source, "");
    std::string result1 = pp1.process();
    assert(result1.find("let syscall_num: u64 = 1;") != std::string::npos);
    
    // Explicit Linux target
    Preprocessor pp2(source, "x86_64-unknown-linux-gnu");
    std::string result2 = pp2.process();
    assert(result2.find("let syscall_num: u64 = 1;") != std::string::npos);
    
    std::cout << "  ✓ PLATFORM_LINUX works correctly" << std::endl;
}

void testPlatformWindows() {
    std::cout << "Testing PLATFORM_WINDOWS..." << std::endl;
    
    std::string source = R"(
#if PLATFORM_WINDOWS
let api_call: u64 = 1;
#endif
#if PLATFORM_LINUX
let syscall_num: u64 = 2;
#endif
)";
    
    Preprocessor pp(source, "x86_64-pc-windows-msvc");
    std::string result = pp.process();
    
    // Should include Windows code
    assert(result.find("let api_call: u64 = 1;") != std::string::npos);
    // Should not include Linux code
    assert(result.find("let syscall_num: u64 = 2;") == std::string::npos);
    
    std::cout << "  ✓ PLATFORM_WINDOWS works correctly" << std::endl;
}

void testArchX86_64() {
    std::cout << "Testing ARCH_X86_64..." << std::endl;
    
    std::string source = R"(
#if ARCH_X86_64
let reg: register = rax;
#endif
)";
    
    // Default target should be x86_64
    Preprocessor pp1(source, "");
    std::string result1 = pp1.process();
    assert(result1.find("let reg: register = rax;") != std::string::npos);
    
    // Explicit x86_64 target
    Preprocessor pp2(source, "x86_64-unknown-linux-gnu");
    std::string result2 = pp2.process();
    assert(result2.find("let reg: register = rax;") != std::string::npos);
    
    std::cout << "  ✓ ARCH_X86_64 works correctly" << std::endl;
}

void testArchARM64() {
    std::cout << "Testing ARCH_ARM64..." << std::endl;
    
    std::string source = R"(
#if ARCH_ARM64
let reg: register = x0;
#endif
#if ARCH_X86_64
let reg: register = rax;
#endif
)";
    
    Preprocessor pp(source, "aarch64-unknown-linux-gnu");
    std::string result = pp.process();
    
    // Should include ARM64 code
    assert(result.find("let reg: register = x0;") != std::string::npos);
    // Should not include x86_64 code
    assert(result.find("let reg: register = rax;") == std::string::npos);
    
    std::cout << "  ✓ ARCH_ARM64 works correctly" << std::endl;
}

void testMultiplePlatformConditions() {
    std::cout << "Testing multiple platform conditions..." << std::endl;
    
    std::string source = R"(
#if PLATFORM_LINUX
#if ARCH_X86_64
let syscall_instruction: ptr = 0x050f;
#endif
#if ARCH_ARM64
let syscall_instruction: ptr = 0xd4000001;
#endif
#endif
)";
    
    Preprocessor pp1(source, "x86_64-unknown-linux-gnu");
    std::string result1 = pp1.process();
    assert(result1.find("let syscall_instruction: ptr = 0x050f;") != std::string::npos);
    assert(result1.find("let syscall_instruction: ptr = 0xd4000001;") == std::string::npos);
    
    Preprocessor pp2(source, "aarch64-unknown-linux-gnu");
    std::string result2 = pp2.process();
    assert(result2.find("let syscall_instruction: ptr = 0x050f;") == std::string::npos);
    assert(result2.find("let syscall_instruction: ptr = 0xd4000001;") != std::string::npos);
    
    std::cout << "  ✓ Multiple platform conditions work correctly" << std::endl;
}

void testDefineDirective() {
    std::cout << "Testing #define directive..." << std::endl;
    
    std::string source = R"(
#define MY_FEATURE
#if MY_FEATURE
let enabled: u32 = 1;
#endif
)";
    
    Preprocessor pp(source);
    std::string result = pp.process();
    
    assert(result.find("let enabled: u32 = 1;") != std::string::npos);
    
    std::cout << "  ✓ #define directive works correctly" << std::endl;
}

void testComplexConditionalCompilation() {
    std::cout << "Testing complex conditional compilation..." << std::endl;
    
    std::string source = R"(
fn main() -> u64 {
    #if PLATFORM_LINUX
    #if ARCH_X86_64
    let write_syscall: u64 = 1;
    #else
    let write_syscall: u64 = 64;
    #endif
    #else
    let write_syscall: u64 = 0;
    #endif
    
    return write_syscall;
}
)";
    
    Preprocessor pp(source, "x86_64-unknown-linux-gnu");
    std::string result = pp.process();
    
    // Should include the function and the x86_64 Linux syscall number
    assert(result.find("fn main() -> u64 {") != std::string::npos);
    assert(result.find("let write_syscall: u64 = 1;") != std::string::npos);
    assert(result.find("let write_syscall: u64 = 64;") == std::string::npos);
    assert(result.find("let write_syscall: u64 = 0;") == std::string::npos);
    assert(result.find("return write_syscall;") != std::string::npos);
    
    std::cout << "  ✓ Complex conditional compilation works correctly" << std::endl;
}

void testPreserveNonDirectiveCode() {
    std::cout << "Testing preservation of non-directive code..." << std::endl;
    
    std::string source = R"(
fn test() -> u32 {
    let x: u32 = 42;
    let y: u32 = x + 10;
    return y;
}
)";
    
    Preprocessor pp(source);
    std::string result = pp.process();
    
    // All code should be preserved
    assert(result.find("fn test() -> u32 {") != std::string::npos);
    assert(result.find("let x: u32 = 42;") != std::string::npos);
    assert(result.find("let y: u32 = x + 10;") != std::string::npos);
    assert(result.find("return y;") != std::string::npos);
    
    std::cout << "  ✓ Non-directive code is preserved" << std::endl;
}

int main() {
    std::cout << "\n=== Preprocessor Tests ===" << std::endl;
    
    testBasicIfDef();
    testIfDefUndefined();
    testIfElse();
    testNestedIf();
    testPlatformLinux();
    testPlatformWindows();
    testArchX86_64();
    testArchARM64();
    testMultiplePlatformConditions();
    testDefineDirective();
    testComplexConditionalCompilation();
    testPreserveNonDirectiveCode();
    
    std::cout << "\n✓ All preprocessor tests passed!" << std::endl;
    return 0;
}
