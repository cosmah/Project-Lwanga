#include "IRGenerator.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <cassert>
#include <iostream>

using namespace lwanga;

void testEncryptedString() {
    std::string source = R"(
        fn test_enc() -> ptr {
            let secret: ptr = enc { "Hello, World!" } as ptr;
            return secret;
        }
    )";
    
    std::cout << "Testing encrypted string literal...\n";
    
    // Parse
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
    std::cout << "  ✓ Parsing passed\n";
    
    // Type check
    TypeChecker checker;
    bool typeCheckResult = checker.check(program.get());
    
    if (!typeCheckResult) {
        std::cout << "Type checker errors:\n";
        for (const auto& error : checker.getErrors()) {
            std::cout << "  " << error.message << "\n";
        }
    }
    assert(typeCheckResult);
    std::cout << "  ✓ Type checking passed\n";
    
    // Generate IR
    IRGenerator generator("enc_test");
    bool success = generator.generate(program.get());
    
    if (!success) {
        std::cout << "IR generation errors:\n";
        for (const auto& error : generator.getErrors()) {
            std::cout << "  " << error << "\n";
        }
    }
    assert(success);
    std::cout << "  ✓ IR generation passed\n";
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    bool verifyResult = llvm::verifyModule(*generator.getModule(), &errorStream);
    if (verifyResult) {
        std::cout << "Module verification errors:\n" << errorStr << "\n";
    }
    assert(!verifyResult);
    std::cout << "  ✓ LLVM module verification passed\n";
    
    // Get the generated IR as string for validation
    std::string moduleStr;
    llvm::raw_string_ostream moduleStream(moduleStr);
    generator.getModule()->print(moduleStream, nullptr);
    moduleStream.flush();
    
    // Verify encrypted string features are present in IR
    // Should have encrypted data array
    assert(moduleStr.find(".enc_data") != std::string::npos);
    std::cout << "  ✓ Encrypted data array found in IR\n";
    
    // Should have XOR key array
    assert(moduleStr.find(".enc_key") != std::string::npos);
    std::cout << "  ✓ XOR key array found in IR\n";
    
    // Should have decryption loop
    assert(moduleStr.find("enc_loop") != std::string::npos);
    std::cout << "  ✓ Decryption loop found in IR\n";
    
    // Should have XOR operation
    assert(moduleStr.find("xor") != std::string::npos);
    std::cout << "  ✓ XOR operation found in IR\n";
    
    // Verify the plaintext "Hello, World!" is NOT in the IR
    // (it should be encrypted)
    assert(moduleStr.find("Hello, World!") == std::string::npos);
    std::cout << "  ✓ Plaintext NOT found in IR (properly encrypted)\n";
    
    std::cout << "\n✓ Encrypted string test passed!\n";
}

void testMultipleEncBlocks() {
    std::string source = R"(
        fn test_multiple() -> u64 {
            let secret1: ptr = enc { "password123" } as ptr;
            let secret2: ptr = enc { "api_key_xyz" } as ptr;
            return 0;
        }
    )";
    
    std::cout << "\nTesting multiple encrypted blocks...\n";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("multi_enc_test");
    assert(generator.generate(program.get()));
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "  ✓ Multiple encrypted blocks work correctly\n";
}

void testEncBlockInUnsafe() {
    std::string source = R"(
        fn test_unsafe_enc() -> ptr {
            unsafe {
                let secret: ptr = enc { "secret_data" } as ptr;
                return secret;
            }
        }
    )";
    
    std::cout << "\nTesting encrypted block in unsafe context...\n";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    assert(!parser.hasErrors());
    
    TypeChecker checker;
    assert(checker.check(program.get()));
    
    IRGenerator generator("unsafe_enc_test");
    assert(generator.generate(program.get()));
    
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    assert(!llvm::verifyModule(*generator.getModule(), &errorStream));
    
    std::cout << "  ✓ Encrypted block in unsafe context works\n";
}

int main() {
    std::cout << "Running Encrypted String Tests...\n\n";
    
    testEncryptedString();
    testMultipleEncBlocks();
    testEncBlockInUnsafe();
    
    std::cout << "\n✓ All encrypted string tests passed!\n";
    return 0;
}
