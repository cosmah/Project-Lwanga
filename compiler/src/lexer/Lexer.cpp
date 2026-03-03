#include "Lexer.h"
#include <cctype>
#include <stdexcept>

namespace lwanga {

Lexer::Lexer(const std::string& src)
    : source(src), cursor(0), line(1), column(1), hasCachedToken(false) {
    initKeywords();
}

void Lexer::initKeywords() {
    keywords = {
        {"u8", TokenType::TOK_U8},
        {"u16", TokenType::TOK_U16},
        {"u32", TokenType::TOK_U32},
        {"u64", TokenType::TOK_U64},
        {"ptr", TokenType::TOK_PTR},
        {"fn", TokenType::TOK_FN},
        {"return", TokenType::TOK_RETURN},
        {"if", TokenType::TOK_IF},
        {"else", TokenType::TOK_ELSE},
        {"while", TokenType::TOK_WHILE},
        {"asm", TokenType::TOK_ASM},
        {"syscall", TokenType::TOK_SYSCALL},
        {"enc", TokenType::TOK_ENC},
        {"naked", TokenType::TOK_NAKED},
        {"let", TokenType::TOK_LET},
        {"mut", TokenType::TOK_MUT},
        {"struct", TokenType::TOK_STRUCT},
        {"const", TokenType::TOK_CONST},
        {"import", TokenType::TOK_IMPORT},
        {"unsafe", TokenType::TOK_UNSAFE},
        {"as", TokenType::TOK_AS},
        {"packed", TokenType::TOK_PACKED},
        {"register", TokenType::TOK_REGISTER}
    };
}

char Lexer::currentChar() const {
    if (cursor >= source.length()) {
        return '\0';
    }
    return source[cursor];
}

char Lexer::peekChar(size_t offset) const {
    size_t pos = cursor + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

void Lexer::advance() {
    if (cursor < source.length()) {
        if (source[cursor] == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        cursor++;
    }
}

void Lexer::skipWhitespace() {
    while (std::isspace(currentChar())) {
        advance();
    }
}

void Lexer::skipLineComment() {
    // Skip //
    advance();
    advance();
    
    // Skip until end of line
    while (currentChar() != '\n' && currentChar() != '\0') {
        advance();
    }
}

void Lexer::skipBlockComment() {
    // Skip /*
    advance();
    advance();
    
    // Skip until */
    while (currentChar() != '\0') {
        if (currentChar() == '*' && peekChar() == '/') {
            advance(); // *
            advance(); // /
            break;
        }
        advance();
    }
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme) {
    return Token(type, lexeme, line, column - lexeme.length());
}

Token Lexer::makeToken(TokenType type, const std::string& lexeme, uint32_t startLine, uint32_t startColumn) {
    return Token(type, lexeme, startLine, startColumn);
}

Token Lexer::parseIdentifierOrKeyword() {
    uint32_t startLine = line;
    uint32_t startColumn = column;
    std::string buffer;
    
    // Parse [a-zA-Z_][a-zA-Z0-9_]*
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        buffer += currentChar();
        advance();
    }
    
    // Check if it's a keyword
    auto it = keywords.find(buffer);
    if (it != keywords.end()) {
        return makeToken(it->second, buffer, startLine, startColumn);
    }
    
    // It's an identifier
    return makeToken(TokenType::TOK_IDENTIFIER, buffer, startLine, startColumn);
}

Token Lexer::parseNumber() {
    uint32_t startLine = line;
    uint32_t startColumn = column;
    std::string buffer;
    
    // Check for hexadecimal (0x prefix)
    if (currentChar() == '0' && (peekChar() == 'x' || peekChar() == 'X')) {
        buffer += currentChar();
        advance();
        buffer += currentChar();
        advance();
        
        // Parse hex digits
        while (std::isxdigit(currentChar())) {
            buffer += currentChar();
            advance();
        }
    } else {
        // Parse decimal digits
        while (std::isdigit(currentChar())) {
            buffer += currentChar();
            advance();
        }
    }
    
    return makeToken(TokenType::TOK_NUMBER, buffer, startLine, startColumn);
}

Token Lexer::parseString() {
    uint32_t startLine = line;
    uint32_t startColumn = column;
    std::string buffer;
    
    // Skip opening quote
    advance();
    
    while (currentChar() != '"' && currentChar() != '\0') {
        if (currentChar() == '\\') {
            // Handle escape sequences
            advance();
            switch (currentChar()) {
                case 'n': buffer += '\n'; break;
                case 'r': buffer += '\r'; break;
                case 't': buffer += '\t'; break;
                case '\\': buffer += '\\'; break;
                case '"': buffer += '"'; break;
                case 'x': {
                    // Hex escape \xHH
                    advance();
                    std::string hex;
                    if (std::isxdigit(currentChar())) {
                        hex += currentChar();
                        advance();
                    }
                    if (std::isxdigit(currentChar())) {
                        hex += currentChar();
                        advance();
                    }
                    if (!hex.empty()) {
                        buffer += static_cast<char>(std::stoi(hex, nullptr, 16));
                    }
                    continue;
                }
                default:
                    buffer += currentChar();
                    break;
            }
            advance();
        } else {
            buffer += currentChar();
            advance();
        }
    }
    
    if (currentChar() == '"') {
        advance(); // Skip closing quote
    }
    
    return makeToken(TokenType::TOK_STRING, buffer, startLine, startColumn);
}

Token Lexer::parseOperator() {
    uint32_t startLine = line;
    uint32_t startColumn = column;
    char current = currentChar();
    char next = peekChar();
    
    // Two-character operators
    if (current == '<' && next == '<') {
        advance(); advance();
        return makeToken(TokenType::TOK_LEFT_SHIFT, "<<", startLine, startColumn);
    }
    if (current == '>' && next == '>') {
        advance(); advance();
        return makeToken(TokenType::TOK_RIGHT_SHIFT, ">>", startLine, startColumn);
    }
    if (current == '=' && next == '=') {
        advance(); advance();
        return makeToken(TokenType::TOK_EQUAL, "==", startLine, startColumn);
    }
    if (current == '!' && next == '=') {
        advance(); advance();
        return makeToken(TokenType::TOK_NOT_EQUAL, "!=", startLine, startColumn);
    }
    if (current == '<' && next == '=') {
        advance(); advance();
        return makeToken(TokenType::TOK_LESS_EQUAL, "<=", startLine, startColumn);
    }
    if (current == '>' && next == '=') {
        advance(); advance();
        return makeToken(TokenType::TOK_GREATER_EQUAL, ">=", startLine, startColumn);
    }
    if (current == '&' && next == '&') {
        advance(); advance();
        return makeToken(TokenType::TOK_LOGICAL_AND, "&&", startLine, startColumn);
    }
    if (current == '|' && next == '|') {
        advance(); advance();
        return makeToken(TokenType::TOK_LOGICAL_OR, "||", startLine, startColumn);
    }
    if (current == '-' && next == '>') {
        advance(); advance();
        return makeToken(TokenType::TOK_ARROW, "->", startLine, startColumn);
    }
    
    // Single-character operators
    advance();
    switch (current) {
        case '+': return makeToken(TokenType::TOK_PLUS, "+", startLine, startColumn);
        case '-': return makeToken(TokenType::TOK_MINUS, "-", startLine, startColumn);
        case '*': return makeToken(TokenType::TOK_STAR, "*", startLine, startColumn);
        case '/': return makeToken(TokenType::TOK_SLASH, "/", startLine, startColumn);
        case '%': return makeToken(TokenType::TOK_PERCENT, "%", startLine, startColumn);
        case '&': return makeToken(TokenType::TOK_AMPERSAND, "&", startLine, startColumn);
        case '|': return makeToken(TokenType::TOK_PIPE, "|", startLine, startColumn);
        case '^': return makeToken(TokenType::TOK_CARET, "^", startLine, startColumn);
        case '~': return makeToken(TokenType::TOK_TILDE, "~", startLine, startColumn);
        case '=': return makeToken(TokenType::TOK_ASSIGN, "=", startLine, startColumn);
        case '!': return makeToken(TokenType::TOK_NOT, "!", startLine, startColumn);
        case '<': return makeToken(TokenType::TOK_LESS, "<", startLine, startColumn);
        case '>': return makeToken(TokenType::TOK_GREATER, ">", startLine, startColumn);
        default:
            return makeToken(TokenType::TOK_UNKNOWN, std::string(1, current), startLine, startColumn);
    }
}

Token Lexer::nextToken() {
    // Return cached token if available
    if (hasCachedToken) {
        hasCachedToken = false;
        return cachedToken;
    }
    
    // Skip whitespace and comments
    while (true) {
        skipWhitespace();
        
        if (currentChar() == '/' && peekChar() == '/') {
            skipLineComment();
        } else if (currentChar() == '/' && peekChar() == '*') {
            skipBlockComment();
        } else {
            break;
        }
    }
    
    // Check for EOF
    if (currentChar() == '\0') {
        return makeToken(TokenType::TOK_EOF, "");
    }
    
    uint32_t startLine = line;
    uint32_t startColumn = column;
    char current = currentChar();
    
    // Identifiers and keywords
    if (std::isalpha(current) || current == '_') {
        return parseIdentifierOrKeyword();
    }
    
    // Numbers
    if (std::isdigit(current)) {
        return parseNumber();
    }
    
    // String literals
    if (current == '"') {
        return parseString();
    }
    
    // Delimiters
    switch (current) {
        case '(':
            advance();
            return makeToken(TokenType::TOK_LEFT_PAREN, "(", startLine, startColumn);
        case ')':
            advance();
            return makeToken(TokenType::TOK_RIGHT_PAREN, ")", startLine, startColumn);
        case '{':
            advance();
            return makeToken(TokenType::TOK_LEFT_BRACE, "{", startLine, startColumn);
        case '}':
            advance();
            return makeToken(TokenType::TOK_RIGHT_BRACE, "}", startLine, startColumn);
        case '[':
            advance();
            return makeToken(TokenType::TOK_LEFT_BRACKET, "[", startLine, startColumn);
        case ']':
            advance();
            return makeToken(TokenType::TOK_RIGHT_BRACKET, "]", startLine, startColumn);
        case ';':
            advance();
            return makeToken(TokenType::TOK_SEMICOLON, ";", startLine, startColumn);
        case ',':
            advance();
            return makeToken(TokenType::TOK_COMMA, ",", startLine, startColumn);
        case ':':
            advance();
            return makeToken(TokenType::TOK_COLON, ":", startLine, startColumn);
        case '.':
            advance();
            return makeToken(TokenType::TOK_DOT, ".", startLine, startColumn);
    }
    
    // Operators
    if (current == '+' || current == '-' || current == '*' || current == '/' ||
        current == '%' || current == '&' || current == '|' || current == '^' ||
        current == '~' || current == '=' || current == '!' || current == '<' ||
        current == '>') {
        return parseOperator();
    }
    
    // Unknown character
    advance();
    return makeToken(TokenType::TOK_UNKNOWN, std::string(1, current), startLine, startColumn);
}

Token Lexer::peekToken() {
    if (!hasCachedToken) {
        cachedToken = nextToken();
        hasCachedToken = true;
    }
    return cachedToken;
}

} // namespace lwanga
