#include "Lexer.h"
#include <iostream>
#include <cassert>
#include <vector>

using namespace lwanga;

void testKeywords() {
    std::string source = "u8 u16 u32 u64 ptr fn return if else while asm syscall enc naked let mut struct const import unsafe as";
    Lexer lexer(source);
    
    std::vector<TokenType> expected = {
        TokenType::TOK_U8, TokenType::TOK_U16, TokenType::TOK_U32, TokenType::TOK_U64,
        TokenType::TOK_PTR, TokenType::TOK_FN, TokenType::TOK_RETURN, TokenType::TOK_IF,
        TokenType::TOK_ELSE, TokenType::TOK_WHILE, TokenType::TOK_ASM, TokenType::TOK_SYSCALL,
        TokenType::TOK_ENC, TokenType::TOK_NAKED, TokenType::TOK_LET, TokenType::TOK_MUT,
        TokenType::TOK_STRUCT, TokenType::TOK_CONST, TokenType::TOK_IMPORT, TokenType::TOK_UNSAFE,
        TokenType::TOK_AS, TokenType::TOK_EOF
    };
    
    for (auto expectedType : expected) {
        Token tok = lexer.nextToken();
        assert(tok.type == expectedType);
    }
    
    std::cout << "✓ Keyword recognition test passed\n";
}

void testNumbers() {
    std::string source = "42 0x1A2B 0xDEADBEEF 123456";
    Lexer lexer(source);
    
    Token tok1 = lexer.nextToken();
    assert(tok1.type == TokenType::TOK_NUMBER);
    assert(tok1.lexeme == "42");
    
    Token tok2 = lexer.nextToken();
    assert(tok2.type == TokenType::TOK_NUMBER);
    assert(tok2.lexeme == "0x1A2B");
    
    Token tok3 = lexer.nextToken();
    assert(tok3.type == TokenType::TOK_NUMBER);
    assert(tok3.lexeme == "0xDEADBEEF");
    
    Token tok4 = lexer.nextToken();
    assert(tok4.type == TokenType::TOK_NUMBER);
    assert(tok4.lexeme == "123456");
    
    std::cout << "✓ Number parsing test passed\n";
}

void testOperators() {
    std::string source = "+ - * / % & | ^ ~ << >> == != < > <= >= = ! ->";
    Lexer lexer(source);
    
    std::vector<TokenType> expected = {
        TokenType::TOK_PLUS, TokenType::TOK_MINUS, TokenType::TOK_STAR, TokenType::TOK_SLASH,
        TokenType::TOK_PERCENT, TokenType::TOK_AMPERSAND, TokenType::TOK_PIPE, TokenType::TOK_CARET,
        TokenType::TOK_TILDE, TokenType::TOK_LEFT_SHIFT, TokenType::TOK_RIGHT_SHIFT,
        TokenType::TOK_EQUAL, TokenType::TOK_NOT_EQUAL, TokenType::TOK_LESS, TokenType::TOK_GREATER,
        TokenType::TOK_LESS_EQUAL, TokenType::TOK_GREATER_EQUAL, TokenType::TOK_ASSIGN,
        TokenType::TOK_NOT, TokenType::TOK_ARROW, TokenType::TOK_EOF
    };
    
    for (auto expectedType : expected) {
        Token tok = lexer.nextToken();
        assert(tok.type == expectedType);
    }
    
    std::cout << "✓ Operator parsing test passed\n";
}

void testDelimiters() {
    std::string source = "( ) { } [ ] ; , : .";
    Lexer lexer(source);
    
    std::vector<TokenType> expected = {
        TokenType::TOK_LEFT_PAREN, TokenType::TOK_RIGHT_PAREN,
        TokenType::TOK_LEFT_BRACE, TokenType::TOK_RIGHT_BRACE,
        TokenType::TOK_LEFT_BRACKET, TokenType::TOK_RIGHT_BRACKET,
        TokenType::TOK_SEMICOLON, TokenType::TOK_COMMA, TokenType::TOK_COLON, TokenType::TOK_DOT,
        TokenType::TOK_EOF
    };
    
    for (auto expectedType : expected) {
        Token tok = lexer.nextToken();
        assert(tok.type == expectedType);
    }
    
    std::cout << "✓ Delimiter parsing test passed\n";
}

void testStrings() {
    std::string source = R"("hello" "world\n" "tab\there" "quote\"test" "hex\x41\x42")";
    Lexer lexer(source);
    
    Token tok1 = lexer.nextToken();
    assert(tok1.type == TokenType::TOK_STRING);
    assert(tok1.lexeme == "hello");
    
    Token tok2 = lexer.nextToken();
    assert(tok2.type == TokenType::TOK_STRING);
    assert(tok2.lexeme == "world\n");
    
    Token tok3 = lexer.nextToken();
    assert(tok3.type == TokenType::TOK_STRING);
    assert(tok3.lexeme == "tab\there");
    
    Token tok4 = lexer.nextToken();
    assert(tok4.type == TokenType::TOK_STRING);
    assert(tok4.lexeme == "quote\"test");
    
    Token tok5 = lexer.nextToken();
    assert(tok5.type == TokenType::TOK_STRING);
    assert(tok5.lexeme == "hexAB");
    
    std::cout << "✓ String literal test passed\n";
}

void testComments() {
    std::string source = R"(
        // This is a line comment
        let x = 42; // inline comment
        /* This is a
           block comment */
        let y = 10;
    )";
    Lexer lexer(source);
    
    Token tok1 = lexer.nextToken();
    assert(tok1.type == TokenType::TOK_LET);
    
    Token tok2 = lexer.nextToken();
    assert(tok2.type == TokenType::TOK_IDENTIFIER);
    assert(tok2.lexeme == "x");
    
    Token tok3 = lexer.nextToken();
    assert(tok3.type == TokenType::TOK_ASSIGN);
    
    Token tok4 = lexer.nextToken();
    assert(tok4.type == TokenType::TOK_NUMBER);
    assert(tok4.lexeme == "42");
    
    Token tok5 = lexer.nextToken();
    assert(tok5.type == TokenType::TOK_SEMICOLON);
    
    Token tok6 = lexer.nextToken();
    assert(tok6.type == TokenType::TOK_LET);
    
    Token tok7 = lexer.nextToken();
    assert(tok7.type == TokenType::TOK_IDENTIFIER);
    assert(tok7.lexeme == "y");
    
    std::cout << "✓ Comment skipping test passed\n";
}

void testComplexProgram() {
    std::string source = R"(
        fn main() -> u64 {
            let msg: ptr = "Hello, Lwanga!\n" as ptr;
            let len: u64 = 15;
            syscall(1, 1, msg, len);
            return 0;
        }
    )";
    Lexer lexer(source);
    
    // Just verify it tokenizes without errors
    Token tok;
    int tokenCount = 0;
    do {
        tok = lexer.nextToken();
        tokenCount++;
    } while (tok.type != TokenType::TOK_EOF);
    
    assert(tokenCount > 20); // Should have many tokens
    
    std::cout << "✓ Complex program tokenization test passed\n";
}

void testLineTracking() {
    std::string source = "let\nx\n=\n42;";
    Lexer lexer(source);
    
    Token tok1 = lexer.nextToken();
    assert(tok1.line == 1);
    
    Token tok2 = lexer.nextToken();
    assert(tok2.line == 2);
    
    Token tok3 = lexer.nextToken();
    assert(tok3.line == 3);
    
    Token tok4 = lexer.nextToken();
    assert(tok4.line == 4);
    
    std::cout << "✓ Line tracking test passed\n";
}

int main() {
    std::cout << "Running Lexer Tests...\n\n";
    
    testKeywords();
    testNumbers();
    testOperators();
    testDelimiters();
    testStrings();
    testComments();
    testComplexProgram();
    testLineTracking();
    
    std::cout << "\n✓ All lexer tests passed!\n";
    return 0;
}
