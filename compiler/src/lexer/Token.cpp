#include "Token.h"

namespace lwanga {

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case TokenType::TOK_EOF: return "EOF";
        case TokenType::TOK_IDENTIFIER: return "IDENTIFIER";
        case TokenType::TOK_NUMBER: return "NUMBER";
        case TokenType::TOK_STRING: return "STRING";
        
        // Keywords
        case TokenType::TOK_U8: return "u8";
        case TokenType::TOK_U16: return "u16";
        case TokenType::TOK_U32: return "u32";
        case TokenType::TOK_U64: return "u64";
        case TokenType::TOK_PTR: return "ptr";
        case TokenType::TOK_FN: return "fn";
        case TokenType::TOK_RETURN: return "return";
        case TokenType::TOK_IF: return "if";
        case TokenType::TOK_ELSE: return "else";
        case TokenType::TOK_WHILE: return "while";
        case TokenType::TOK_ASM: return "asm";
        case TokenType::TOK_SYSCALL: return "syscall";
        case TokenType::TOK_ENC: return "enc";
        case TokenType::TOK_NAKED: return "naked";
        case TokenType::TOK_LET: return "let";
        case TokenType::TOK_MUT: return "mut";
        case TokenType::TOK_STRUCT: return "struct";
        case TokenType::TOK_CONST: return "const";
        case TokenType::TOK_IMPORT: return "import";
        case TokenType::TOK_UNSAFE: return "unsafe";
        case TokenType::TOK_AS: return "as";
        case TokenType::TOK_PACKED: return "packed";
        case TokenType::TOK_REGISTER: return "register";
        
        // Operators
        case TokenType::TOK_PLUS: return "+";
        case TokenType::TOK_MINUS: return "-";
        case TokenType::TOK_STAR: return "*";
        case TokenType::TOK_SLASH: return "/";
        case TokenType::TOK_PERCENT: return "%";
        case TokenType::TOK_AMPERSAND: return "&";
        case TokenType::TOK_PIPE: return "|";
        case TokenType::TOK_CARET: return "^";
        case TokenType::TOK_TILDE: return "~";
        case TokenType::TOK_LEFT_SHIFT: return "<<";
        case TokenType::TOK_RIGHT_SHIFT: return ">>";
        case TokenType::TOK_EQUAL: return "==";
        case TokenType::TOK_NOT_EQUAL: return "!=";
        case TokenType::TOK_LESS: return "<";
        case TokenType::TOK_GREATER: return ">";
        case TokenType::TOK_LESS_EQUAL: return "<=";
        case TokenType::TOK_GREATER_EQUAL: return ">=";
        case TokenType::TOK_ASSIGN: return "=";
        case TokenType::TOK_NOT: return "!";
        case TokenType::TOK_LOGICAL_AND: return "&&";
        case TokenType::TOK_LOGICAL_OR: return "||";
        case TokenType::TOK_ARROW: return "->";
        
        // Delimiters
        case TokenType::TOK_LEFT_PAREN: return "(";
        case TokenType::TOK_RIGHT_PAREN: return ")";
        case TokenType::TOK_LEFT_BRACE: return "{";
        case TokenType::TOK_RIGHT_BRACE: return "}";
        case TokenType::TOK_LEFT_BRACKET: return "[";
        case TokenType::TOK_RIGHT_BRACKET: return "]";
        case TokenType::TOK_SEMICOLON: return ";";
        case TokenType::TOK_COMMA: return ",";
        case TokenType::TOK_COLON: return ":";
        case TokenType::TOK_DOT: return ".";
        
        case TokenType::TOK_UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

std::ostream& operator<<(std::ostream& os, const Token& token) {
    os << "Token(" << tokenTypeToString(token.type) 
       << ", \"" << token.lexeme << "\""
       << ", " << token.line << ":" << token.column << ")";
    return os;
}

} // namespace lwanga
