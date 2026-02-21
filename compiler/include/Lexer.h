#ifndef LWANGA_LEXER_H
#define LWANGA_LEXER_H

#include "Token.h"
#include <string>
#include <unordered_map>
#include <cstdint>

namespace lwanga {

class Lexer {
public:
    explicit Lexer(const std::string& source);
    
    // Get the next token from the source
    Token nextToken();
    
    // Peek at the next token without consuming it
    Token peekToken();
    
    // Get current line number
    uint32_t getCurrentLine() const { return line; }
    
    // Get current column number
    uint32_t getCurrentColumn() const { return column; }

private:
    std::string source;
    size_t cursor;
    uint32_t line;
    uint32_t column;
    
    // Keyword lookup table
    std::unordered_map<std::string, TokenType> keywords;
    
    // Cached peek token
    Token cachedToken;
    bool hasCachedToken;
    
    // Helper methods
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    void skipLineComment();
    void skipBlockComment();
    
    Token makeToken(TokenType type, const std::string& lexeme);
    Token makeToken(TokenType type, const std::string& lexeme, uint32_t startLine, uint32_t startColumn);
    
    // Token parsing methods
    Token parseIdentifierOrKeyword();
    Token parseNumber();
    Token parseString();
    Token parseOperator();
    
    // Initialize keyword table
    void initKeywords();
};

} // namespace lwanga

#endif // LWANGA_LEXER_H
