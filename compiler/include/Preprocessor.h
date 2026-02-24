#ifndef LWANGA_PREPROCESSOR_H
#define LWANGA_PREPROCESSOR_H

#include <string>
#include <unordered_map>
#include <unordered_set>
#include <stack>
#include <cstdint>

namespace lwanga {

struct DirectiveInfo {
    std::string type;
    uint32_t line;
};

class Preprocessor {
public:
    explicit Preprocessor(const std::string& source, const std::string& targetTriple = "");
    
    // Process the source code and return preprocessed output
    std::string process();
    
    // Define a preprocessor symbol with an optional value
    void define(const std::string& symbol, const std::string& value = "");
    
    // Check if a symbol is defined
    bool isDefined(const std::string& symbol) const;

private:
    std::string source;
    std::string targetTriple;
    std::unordered_map<std::string, std::string> symbols;
    std::stack<DirectiveInfo> directiveStack;
    size_t cursor;
    uint32_t line;
    
    // Helper methods
    char currentChar() const;
    char peekChar(size_t offset = 1) const;
    void advance();
    void skipWhitespace();
    std::string readIdentifier();
    
    // Process directives
    bool processDirective(std::string& output);
    bool evaluateCondition(const std::string& condition);
    void skipUntilEndif(int depth = 1);
    void skipUntilElseOrEndif(int depth = 1);
    
    // Initialize platform-specific symbols
    void initPlatformSymbols();
};

} // namespace lwanga

#endif // LWANGA_PREPROCESSOR_H
