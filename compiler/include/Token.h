#ifndef LWANGA_TOKEN_H
#define LWANGA_TOKEN_H

#include <string>
#include <ostream>
#include <cstdint>

namespace lwanga {

// Token types covering all keywords, operators, and delimiters
enum class TokenType {
    // End of file
    TOK_EOF = 0,
    
    // Identifiers and literals
    TOK_IDENTIFIER,
    TOK_NUMBER,
    TOK_STRING,
    
    // Keywords
    TOK_U8,
    TOK_U16,
    TOK_U32,
    TOK_U64,
    TOK_PTR,
    TOK_FN,
    TOK_RETURN,
    TOK_IF,
    TOK_ELSE,
    TOK_WHILE,
    TOK_ASM,
    TOK_SYSCALL,
    TOK_ENC,
    TOK_NAKED,
    TOK_LET,
    TOK_MUT,
    TOK_STRUCT,
    TOK_CONST,
    TOK_IMPORT,
    TOK_UNSAFE,
    TOK_AS,
    
    // Operators
    TOK_PLUS,           // +
    TOK_MINUS,          // -
    TOK_STAR,           // *
    TOK_SLASH,          // /
    TOK_PERCENT,        // %
    TOK_AMPERSAND,      // &
    TOK_PIPE,           // |
    TOK_CARET,          // ^
    TOK_TILDE,          // ~
    TOK_LEFT_SHIFT,     // <<
    TOK_RIGHT_SHIFT,    // >>
    TOK_EQUAL,          // ==
    TOK_NOT_EQUAL,      // !=
    TOK_LESS,           // <
    TOK_GREATER,        // >
    TOK_LESS_EQUAL,     // <=
    TOK_GREATER_EQUAL,  // >=
    TOK_ASSIGN,         // =
    TOK_NOT,            // !
    TOK_ARROW,          // ->
    
    // Delimiters
    TOK_LEFT_PAREN,     // (
    TOK_RIGHT_PAREN,    // )
    TOK_LEFT_BRACE,     // {
    TOK_RIGHT_BRACE,    // }
    TOK_LEFT_BRACKET,   // [
    TOK_RIGHT_BRACKET,  // ]
    TOK_SEMICOLON,      // ;
    TOK_COMMA,          // ,
    TOK_COLON,          // :
    TOK_DOT,            // .
    
    // Unknown/Error
    TOK_UNKNOWN
};

// Token structure with type, lexeme, and position information
struct Token {
    TokenType type;
    std::string lexeme;
    uint32_t line;
    uint32_t column;
    
    Token() : type(TokenType::TOK_EOF), lexeme(""), line(0), column(0) {}
    
    Token(TokenType t, const std::string& lex, uint32_t ln, uint32_t col)
        : type(t), lexeme(lex), line(ln), column(col) {}
    
    // Check if token is a specific type
    bool is(TokenType t) const { return type == t; }
    
    // Check if token is not a specific type
    bool isNot(TokenType t) const { return type != t; }
    
    // Check if token is EOF
    bool isEOF() const { return type == TokenType::TOK_EOF; }
};

// Convert TokenType to string for debugging
const char* tokenTypeToString(TokenType type);

// Output operator for Token
std::ostream& operator<<(std::ostream& os, const Token& token);

} // namespace lwanga

#endif // LWANGA_TOKEN_H
