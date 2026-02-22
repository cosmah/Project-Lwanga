#include "Backend.h"
#include "IRGenerator.h"
#include "Parser.h"
#include "Lexer.h"
#include "TypeChecker.h"
#include <llvm/IR/Module.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

// Helper to compile a simple program
std::unique_ptr<IRGenerator> compileProgram(const std::string& source) {
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    assert(program != nullptr && "Parse failed");
    
    TypeChecker typeChecker;
    assert(typeChecker.check(program.get()) && "Type check failed");
    
    auto irGen = std::make_unique<IRGenerator>("test_module", "test.lwanga");
    assert(irGen->generate(program.get()) && "IR generation failed");
    
    return irGen;
}

void testTargetTripleValidation() {
    std::cout << "Testing target triple validation...\n";
    
    // Initialize LLVM targets
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmPrinters();
    llvm::InitializeAllAsmParsers();
    
    auto irGen = compileProgram("fn main() -> u64 { return 42; }");
    Backend backend(irGen->getModule(), 0);
    
    // Test valid x86_64 Linux target
    assert(backend.setTargetTriple("x86_64-unknown-linux-gnu") && "x86_64-linux should be supported");
    assert(!backend.hasErrors() && "x86_64-linux should not have errors");
    std::cout << "  ✓ x86_64-linux-gnu supported\n";
    
    // Test valid ARM64 Linux target
    assert(backend.setTargetTriple("aarch64-unknown-linux-gnu") && "aarch64-linux should be supported");
    assert(!backend.hasErrors() && "aarch64-linux should not have errors");
    std::cout << "  ✓ aarch64-linux-gnu supported\n";
    
    // Test valid x86_64 Windows target
    assert(backend.setTargetTriple("x86_64-pc-windows-msvc") && "x86_64-windows should be supported");
    assert(!backend.hasErrors() && "x86_64-windows should not have errors");
    std::cout << "  ✓ x86_64-windows-msvc supported\n";
    
    // Test valid ARM64 Windows target
    assert(backend.setTargetTriple("aarch64-pc-windows-msvc") && "aarch64-windows should be supported");
    assert(!backend.hasErrors() && "aarch64-windows should not have errors");
    std::cout << "  ✓ aarch64-windows-msvc supported\n";
    
    std::cout << "Target triple validation: PASSED\n\n";
}

void testUnsupportedTargets() {
    std::cout << "Testing unsupported target rejection...\n";
    
    auto irGen = compileProgram("fn main() -> u64 { return 42; }");
    
    // Test unsupported architecture
    Backend backend1(irGen->getModule(), 0);
    assert(!backend1.setTargetTriple("mips-unknown-linux-gnu") && "mips should be unsupported");
    assert(backend1.hasErrors() && "mips should have errors");
    assert(backend1.getError().find("Unsupported target triple") != std::string::npos);
    std::cout << "  ✓ mips-linux rejected\n";
    
    // Test unsupported platform (macOS)
    auto irGen2 = compileProgram("fn main() -> u64 { return 42; }");
    Backend backend2(irGen2->getModule(), 0);
    assert(!backend2.setTargetTriple("x86_64-apple-darwin") && "darwin should be unsupported");
    assert(backend2.hasErrors() && "darwin should have errors");
    std::cout << "  ✓ x86_64-darwin rejected\n";
    
    std::cout << "Unsupported target rejection: PASSED\n\n";
}

void testPlatformSpecificSyscalls() {
    std::cout << "Testing platform-specific syscall generation...\n";
    
    std::string source = R"(
        fn main() -> u64 {
            unsafe {
                let result: u64 = syscall(1, 1, 0, 0);
                return result;
            }
        }
    )";
    
    // Test x86_64 syscall generation
    {
        Lexer lexer(source);
        Parser parser(lexer);
        auto program = parser.parse();
        assert(program != nullptr && "Parse failed");
        
        TypeChecker typeChecker;
        assert(typeChecker.check(program.get()) && "Type check failed");
        
        // Create IRGenerator and set target BEFORE generating IR
        auto irGen = std::make_unique<IRGenerator>("test_x86_64", "test.lwanga");
        irGen->getModule()->setTargetTriple("x86_64-unknown-linux-gnu");
        assert(irGen->generate(program.get()) && "IR generation failed");
        
        // Verify module contains syscall instruction (x86_64 specific)
        std::string moduleStr;
        llvm::raw_string_ostream os(moduleStr);
        irGen->getModule()->print(os, nullptr);
        os.flush();
        
        assert(moduleStr.find("syscall") != std::string::npos && "x86_64 should use syscall instruction");
        std::cout << "  ✓ x86_64 uses 'syscall' instruction\n";
    }
    
    // Test ARM64 syscall generation
    {
        Lexer lexer(source);
        Parser parser(lexer);
        auto program = parser.parse();
        assert(program != nullptr && "Parse failed");
        
        TypeChecker typeChecker;
        assert(typeChecker.check(program.get()) && "Type check failed");
        
        // Create IRGenerator and set target BEFORE generating IR
        auto irGen = std::make_unique<IRGenerator>("test_arm64", "test.lwanga");
        irGen->getModule()->setTargetTriple("aarch64-unknown-linux-gnu");
        assert(irGen->generate(program.get()) && "IR generation failed");
        
        // Verify module contains svc instruction (ARM64 specific)
        std::string moduleStr;
        llvm::raw_string_ostream os(moduleStr);
        irGen->getModule()->print(os, nullptr);
        os.flush();
        
        assert(moduleStr.find("svc") != std::string::npos && "ARM64 should use svc instruction");
        std::cout << "  ✓ ARM64 uses 'svc' instruction\n";
    }
    
    std::cout << "Platform-specific syscalls: PASSED\n\n";
}

void testCallingConventions() {
    std::cout << "Testing platform-specific calling conventions...\n";
    
    std::string source = R"(
        fn add(a: u64, b: u64) -> u64 {
            return a + b;
        }
        
        fn main() -> u64 {
            return add(10, 20);
        }
    )";
    
    // Test x86_64 calling convention
    {
        auto irGen = compileProgram(source);
        irGen->getModule()->setTargetTriple("x86_64-unknown-linux-gnu");
        
        // Verify IR is valid
        std::string errorStr;
        llvm::raw_string_ostream errorStream(errorStr);
        assert(!llvm::verifyModule(*irGen->getModule(), &errorStream) && "x86_64 module should be valid");
        std::cout << "  ✓ x86_64 calling convention valid\n";
    }
    
    // Test ARM64 calling convention
    {
        auto irGen = compileProgram(source);
        irGen->getModule()->setTargetTriple("aarch64-unknown-linux-gnu");
        
        // Verify IR is valid
        std::string errorStr;
        llvm::raw_string_ostream errorStream(errorStr);
        assert(!llvm::verifyModule(*irGen->getModule(), &errorStream) && "ARM64 module should be valid");
        std::cout << "  ✓ ARM64 calling convention valid\n";
    }
    
    std::cout << "Calling conventions: PASSED\n\n";
}

void testInlineAssemblyPlatformSpecific() {
    std::cout << "Testing platform-specific inline assembly...\n";
    
    // Test x86_64 inline assembly
    {
        std::string source = R"(
            fn main() -> u64 {
                unsafe {
                    asm {
                        movq $42, %rax
                    }
                }
                return 0;
            }
        )";
        
        auto irGen = compileProgram(source);
        irGen->getModule()->setTargetTriple("x86_64-unknown-linux-gnu");
        
        // Verify module contains inline assembly
        std::string moduleStr;
        llvm::raw_string_ostream os(moduleStr);
        irGen->getModule()->print(os, nullptr);
        os.flush();
        
        assert(moduleStr.find("movq") != std::string::npos && "x86_64 asm should contain movq");
        std::cout << "  ✓ x86_64 inline assembly preserved\n";
    }
    
    // Test ARM64 inline assembly
    {
        std::string source = R"(
            fn main() -> u64 {
                unsafe {
                    asm {
                        mov x0, #42
                    }
                }
                return 0;
            }
        )";
        
        auto irGen = compileProgram(source);
        irGen->getModule()->setTargetTriple("aarch64-unknown-linux-gnu");
        
        // Verify module contains inline assembly
        std::string moduleStr;
        llvm::raw_string_ostream os(moduleStr);
        irGen->getModule()->print(os, nullptr);
        os.flush();
        
        assert(moduleStr.find("mov") != std::string::npos && "ARM64 asm should contain mov");
        std::cout << "  ✓ ARM64 inline assembly preserved\n";
    }
    
    std::cout << "Inline assembly: PASSED\n\n";
}

int main() {
    std::cout << "=== Cross-Compilation Test Suite ===\n\n";
    
    try {
        testTargetTripleValidation();
        testUnsupportedTargets();
        testPlatformSpecificSyscalls();
        testCallingConventions();
        testInlineAssemblyPlatformSpecific();
        
        std::cout << "=== All Cross-Compilation Tests PASSED ===\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << "\n";
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception\n";
        return 1;
    }
}
