#include "Parser.h"
#include "Lexer.h"
#include <iostream>
#include <cassert>
#include <sstream>

using namespace lwanga;

void testFunctionParsing() {
    std::string source = R"(
        fn add(a: u64, b: u64) -> u64 {
            return a + b;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    assert(program->functions[0]->name == "add");
    assert(program->functions[0]->params.size() == 2);
    assert(program->functions[0]->params[0].name == "a");
    assert(program->functions[0]->params[1].name == "b");
    assert(program->functions[0]->body.size() == 1);
    
    std::cout << "✓ Function parsing test passed\n";
}

void testNakedFunction() {
    std::string source = R"(
        naked fn shellcode() -> u64 {
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    assert(program->functions[0]->isNaked == true);
    
    std::cout << "✓ Naked function parsing test passed\n";
}

void testVariableDeclaration() {
    std::string source = R"(
        fn test() -> u64 {
            let x: u64 = 42;
            let mut y: u32 = 10;
            return x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    assert(program->functions[0]->body.size() == 3);
    
    auto* varDecl1 = dynamic_cast<VarDeclStmt*>(program->functions[0]->body[0].get());
    assert(varDecl1 != nullptr);
    assert(varDecl1->name == "x");
    assert(varDecl1->isMutable == false);
    
    auto* varDecl2 = dynamic_cast<VarDeclStmt*>(program->functions[0]->body[1].get());
    assert(varDecl2 != nullptr);
    assert(varDecl2->name == "y");
    assert(varDecl2->isMutable == true);
    
    std::cout << "✓ Variable declaration parsing test passed\n";
}

void testIfStatement() {
    std::string source = R"(
        fn test(x: u64) -> u64 {
            if (x > 10) {
                return 1;
            } else {
                return 0;
            }
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* ifStmt = dynamic_cast<IfStmt*>(program->functions[0]->body[0].get());
    assert(ifStmt != nullptr);
    assert(ifStmt->thenBlock.size() == 1);
    assert(ifStmt->elseBlock.size() == 1);
    
    std::cout << "✓ If statement parsing test passed\n";
}

void testWhileLoop() {
    std::string source = R"(
        fn test() -> u64 {
            let mut i: u64 = 0;
            while (i < 10) {
                i = i + 1;
            }
            return i;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* whileStmt = dynamic_cast<WhileStmt*>(program->functions[0]->body[1].get());
    assert(whileStmt != nullptr);
    assert(whileStmt->body.size() == 1);
    
    std::cout << "✓ While loop parsing test passed\n";
}

void testBinaryExpressions() {
    std::string source = R"(
        fn test() -> u64 {
            let a: u64 = 1 + 2 * 3;
            let b: u64 = (1 + 2) * 3;
            let c: u64 = 10 << 2;
            let d: u64 = 5 & 3;
            return a;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    assert(program->functions[0]->body.size() == 5);
    
    std::cout << "✓ Binary expression parsing test passed\n";
}

void testUnaryExpressions() {
    std::string source = R"(
        fn test(x: u64) -> u64 {
            let a: u64 = -x;
            let b: ptr = &x;
            let c: u64 = *b;
            return a;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    std::cout << "✓ Unary expression parsing test passed\n";
}

void testFunctionCall() {
    std::string source = R"(
        fn test() -> u64 {
            let result: u64 = add(1, 2);
            return result;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->functions[0]->body[0].get());
    assert(varDecl != nullptr);
    
    auto* callExpr = dynamic_cast<CallExpr*>(varDecl->initializer.get());
    assert(callExpr != nullptr);
    assert(callExpr->args.size() == 2);
    
    std::cout << "✓ Function call parsing test passed\n";
}

void testArrayIndexing() {
    std::string source = R"(
        fn test() -> u64 {
            let arr: [u64; 10];
            let x: u64 = arr[5];
            return x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    std::cout << "✓ Array indexing parsing test passed\n";
}

void testSyscall() {
    std::string source = R"(
        fn test() -> u64 {
            let result: u64 = syscall(1, 1, 0, 0);
            return result;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->functions[0]->body[0].get());
    assert(varDecl != nullptr);
    
    auto* syscallExpr = dynamic_cast<SyscallExpr*>(varDecl->initializer.get());
    assert(syscallExpr != nullptr);
    assert(syscallExpr->syscallNumber == 1);
    assert(syscallExpr->args.size() == 3);
    
    std::cout << "✓ Syscall parsing test passed\n";
}

void testEncBlock() {
    std::string source = R"(
        fn test() -> ptr {
            let msg: ptr = enc { "secret" } as ptr;
            return msg;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    std::cout << "✓ Enc block parsing test passed\n";
}

void testAsmBlock() {
    std::string source = R"(
        fn test() -> u64 {
            asm {
                mov rax, 42
                ret
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* asmStmt = dynamic_cast<AsmStmt*>(program->functions[0]->body[0].get());
    assert(asmStmt != nullptr);
    
    std::cout << "✓ Asm block parsing test passed\n";
}

void testUnsafeBlock() {
    std::string source = R"(
        fn test() -> u64 {
            unsafe {
                let x: u64 = 42;
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(program->functions[0]->body[0].get());
    assert(unsafeBlock != nullptr);
    
    std::cout << "✓ Unsafe block parsing test passed\n";
}

void testStructDefinition() {
    std::string source = R"(
        struct Point {
            x: u64,
            y: u64
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->structs.size() == 1);
    assert(program->structs[0]->name == "Point");
    assert(program->structs[0]->fields.size() == 2);
    assert(program->structs[0]->fields[0].name == "x");
    assert(program->structs[0]->fields[1].name == "y");
    
    std::cout << "✓ Struct definition parsing test passed\n";
}

void testStructInitialization() {
    std::string source = R"(
        fn test() -> u64 {
            let p: Point = Point { x: 10, y: 20 };
            return p.x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    std::cout << "✓ Struct initialization parsing test passed\n";
}

void testFieldAccess() {
    std::string source = R"(
        fn test() -> u64 {
            let p: Point;
            let x: u64 = p.x;
            return x;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    auto* varDecl = dynamic_cast<VarDeclStmt*>(program->functions[0]->body[1].get());
    assert(varDecl != nullptr);
    
    auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(varDecl->initializer.get());
    assert(fieldAccess != nullptr);
    assert(fieldAccess->fieldName == "x");
    
    std::cout << "✓ Field access parsing test passed\n";
}

void testConstant() {
    std::string source = R"(
        const MAX_SIZE: u64 = 1024;
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->constants.size() == 1);
    assert(program->constants[0]->name == "MAX_SIZE");
    
    std::cout << "✓ Constant parsing test passed\n";
}

void testImport() {
    std::string source = R"(
        import "stdlib/io.lwanga";
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->imports.size() == 1);
    assert(program->imports[0]->path == "stdlib/io.lwanga");
    
    std::cout << "✓ Import parsing test passed\n";
}

void testTypeCast() {
    std::string source = R"(
        fn test() -> ptr {
            let x: u64 = 42;
            let p: ptr = x as ptr;
            return p;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->functions.size() == 1);
    
    std::cout << "✓ Type cast parsing test passed\n";
}

void testComplexProgram() {
    std::string source = R"(
        import "stdlib/io.lwanga";
        
        const BUFFER_SIZE: u64 = 1024;
        
        struct Message {
            data: ptr,
            len: u64
        }
        
        fn main() -> u64 {
            let msg: ptr = "Hello, Lwanga!\n" as ptr;
            let len: u64 = 15;
            
            syscall(1, 1, msg, len);
            
            return 0;
        }
        
        naked fn shellcode() -> u64 {
            asm {
                mov rax, 60
                xor rdi, rdi
                syscall
            }
            return 0;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(!parser.hasErrors());
    assert(program->imports.size() == 1);
    assert(program->constants.size() == 1);
    assert(program->structs.size() == 1);
    assert(program->functions.size() == 2);
    
    std::cout << "✓ Complex program parsing test passed\n";
}

void testErrorRecovery() {
    std::string source = R"(
        fn bad_function( {
            let x = ;
        }
        
        fn good_function() -> u64 {
            return 42;
        }
    )";
    
    Lexer lexer(source);
    Parser parser(lexer);
    auto program = parser.parse();
    
    assert(parser.hasErrors());
    // Should still parse the good function
    assert(program->functions.size() >= 1);
    
    std::cout << "✓ Error recovery test passed\n";
}

int main() {
    std::cout << "Running Parser Tests...\n\n";
    
    testFunctionParsing();
    testNakedFunction();
    testVariableDeclaration();
    testIfStatement();
    testWhileLoop();
    testBinaryExpressions();
    testUnaryExpressions();
    testFunctionCall();
    testArrayIndexing();
    testSyscall();
    testEncBlock();
    testAsmBlock();
    testUnsafeBlock();
    testStructDefinition();
    testStructInitialization();
    testFieldAccess();
    testConstant();
    testImport();
    testTypeCast();
    testComplexProgram();
    testErrorRecovery();
    
    std::cout << "\n✓ All parser tests passed!\n";
    return 0;
}
