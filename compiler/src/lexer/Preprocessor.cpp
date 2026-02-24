#include "Preprocessor.h"
#include <cctype>
#include <stdexcept>

namespace lwanga {

Preprocessor::Preprocessor(const std::string& src, const std::string& target)
    : source(src), targetTriple(target), cursor(0), line(1) {
    initPlatformSymbols();
}

void Preprocessor::initPlatformSymbols() {
    // Parse target triple to set platform and architecture symbols
    // Format: <arch>-<vendor>-<os>-<env>
    // Examples: x86_64-unknown-linux-gnu, aarch64-linux-android
    
    if (targetTriple.empty() || targetTriple.find("linux") != std::string::npos) {
        define("PLATFORM_LINUX");
    }
    if (targetTriple.find("windows") != std::string::npos) {
        define("PLATFORM_WINDOWS");
    }
    if (targetTriple.find("darwin") != std::string::npos || targetTriple.find("macos") != std::string::npos) {
        define("PLATFORM_MACOS");
    }
    
    if (targetTriple.empty() || targetTriple.find("x86_64") != std::string::npos || targetTriple.find("amd64") != std::string::npos) {
        define("ARCH_X86_64");
    }
    if (targetTriple.find("aarch64") != std::string::npos || targetTriple.find("arm64") != std::string::npos) {
        define("ARCH_ARM64");
    }
    if (targetTriple.find("arm") != std::string::npos && targetTriple.find("aarch64") == std::string::npos) {
        define("ARCH_ARM");
    }
}

void Preprocessor::define(const std::string& symbol, const std::string& value) {
    symbols[symbol] = value;
}

bool Preprocessor::isDefined(const std::string& symbol) const {
    return symbols.find(symbol) != symbols.end();
}

char Preprocessor::currentChar() const {
    if (cursor >= source.length()) {
        return '\0';
    }
    return source[cursor];
}

char Preprocessor::peekChar(size_t offset) const {
    size_t pos = cursor + offset;
    if (pos >= source.length()) {
        return '\0';
    }
    return source[pos];
}

void Preprocessor::advance() {
    if (cursor < source.length()) {
        if (source[cursor] == '\n') {
            line++;
        }
        cursor++;
    }
}

void Preprocessor::skipWhitespace() {
    while (currentChar() == ' ' || currentChar() == '\t') {
        advance();
    }
}

std::string Preprocessor::readIdentifier() {
    std::string ident;
    while (std::isalnum(currentChar()) || currentChar() == '_') {
        ident += currentChar();
        advance();
    }
    return ident;
}

bool Preprocessor::evaluateCondition(const std::string& condition) {
    // Simple condition evaluation: just check if symbol is defined
    // Trim whitespace
    size_t start = condition.find_first_not_of(" \t");
    size_t end = condition.find_last_not_of(" \t");
    
    if (start == std::string::npos) {
        return false;
    }
    
    std::string symbol = condition.substr(start, end - start + 1);
    return isDefined(symbol);
}

void Preprocessor::skipUntilEndif(int depth) {
    while (currentChar() != '\0') {
        if (currentChar() == '#') {
            advance();
            skipWhitespace();
            std::string directive = readIdentifier();
            
            if (directive == "if" || directive == "ifdef" || directive == "ifndef") {
                depth++;
            } else if (directive == "endif") {
                depth--;
                if (depth == 0) {
                    // Skip rest of line
                    while (currentChar() != '\n' && currentChar() != '\0') advance();
                    if (currentChar() == '\n') advance();
                    return;
                }
            }
        }
        advance();
    }
}

void Preprocessor::skipUntilElseOrEndif(int depth) {
    while (currentChar() != '\0') {
        if (currentChar() == '#') {
            advance();
            skipWhitespace();
            std::string directive = readIdentifier();
            
            if (directive == "if" || directive == "ifdef" || directive == "ifndef") {
                depth++;
            } else if (directive == "else" && depth == 1) {
                // Skip to end of line
                while (currentChar() != '\n' && currentChar() != '\0') advance();
                if (currentChar() == '\n') advance();
                return;
            } else if (directive == "endif") {
                depth--;
                if (depth == 0) {
                    // Skip to end of line
                    while (currentChar() != '\n' && currentChar() != '\0') advance();
                    if (currentChar() == '\n') advance();
                    return;
                }
            }
        }
        advance();
    }
}

bool Preprocessor::processDirective(std::string& output) {
    // We're at '#'
    advance();
    skipWhitespace();
    
    std::string directive = readIdentifier();
    
    if (directive == "if") {
        directiveStack.push({directive, line});
        skipWhitespace();
        std::string condition;
        while (currentChar() != '\n' && currentChar() != '\0') {
            condition += currentChar();
            advance();
        }
        if (currentChar() == '\n') advance();
        
        if (!evaluateCondition(condition)) {
            skipUntilElseOrEndif(1);
        }
        return true;
    } else if (directive == "ifdef") {
        directiveStack.push({directive, line});
        skipWhitespace();
        std::string symbol = readIdentifier();
        while (currentChar() != '\n' && currentChar() != '\0') advance();
        if (currentChar() == '\n') advance();
        
        if (!isDefined(symbol)) {
            skipUntilElseOrEndif(1);
        }
        return true;
    } else if (directive == "ifndef") {
        directiveStack.push({directive, line});
        skipWhitespace();
        std::string symbol = readIdentifier();
        while (currentChar() != '\n' && currentChar() != '\0') advance();
        if (currentChar() == '\n') advance();
        
        if (isDefined(symbol)) {
            skipUntilElseOrEndif(1);
        }
        return true;
    } else if (directive == "else") {
        if (directiveStack.empty() || (directiveStack.top().type != "if" && 
            directiveStack.top().type != "ifdef" && directiveStack.top().type != "ifndef")) {
            throw std::runtime_error("Line " + std::to_string(line) + ": #else without #if");
        }
        // If we reached here, the #if block was active, so we must skip the #else block
        directiveStack.pop(); // Pop the matching #if entry
        skipUntilEndif(1);
        return true;
    } else if (directive == "endif") {
        if (directiveStack.empty()) {
            throw std::runtime_error("Line " + std::to_string(line) + ": #endif without #if");
        }
        directiveStack.pop();
        while (currentChar() != '\n' && currentChar() != '\0') advance();
        if (currentChar() == '\n') advance();
        return true;
    } else if (directive == "define") {
        skipWhitespace();
        std::string symbol = readIdentifier();
        std::string value;
        if (!symbol.empty()) {
            skipWhitespace();
            while (currentChar() != '\n' && currentChar() != '\0') {
                value += currentChar();
                advance();
            }
            // Trim trailing whitespace from value
            size_t last = value.find_last_not_of(" \t\r");
            if (last != std::string::npos) {
                value = value.substr(0, last + 1);
            }
            define(symbol, value);
        }
        while (currentChar() != '\n' && currentChar() != '\0') advance();
        if (currentChar() == '\n') advance();
        return true;
    } else if (directive == "undef") {
        skipWhitespace();
        std::string symbol = readIdentifier();
        if (!symbol.empty()) symbols.erase(symbol);
        while (currentChar() != '\n' && currentChar() != '\0') advance();
        if (currentChar() == '\n') advance();
        return true;
    }
    
    return false;
}

std::string Preprocessor::process() {
    std::string output;
    
    while (currentChar() != '\0') {
        // Check for preprocessor directive at start of line
        if (currentChar() == '#') {
            // Check if it's at the beginning of a line (after whitespace)
            bool atLineStart = true;
            if (cursor > 0) {
                // Look back to see if there's only whitespace before this
                bool onlyWs = true;
                for (int i = (int)cursor - 1; i >= 0; i--) {
                    if (source[i] == '\n') break;
                    if (source[i] != ' ' && source[i] != '\t') {
                        onlyWs = false;
                        break;
                    }
                }
                atLineStart = onlyWs;
            }
            
            if (atLineStart) {
                processDirective(output);
                continue;
            }
        }
        
        // Check for identifier which could be a macro
        if (std::isalpha(currentChar()) || currentChar() == '_') {
            std::string ident = readIdentifier();
            auto it = symbols.find(ident);
            if (it != symbols.end() && !it->second.empty()) {
                // Perform substitution
                output += it->second;
            } else {
                output += ident;
            }
            continue;
        }
        
        // Regular character, copy to output
        output += currentChar();
        advance();
    }
    
    return output;
}

} // namespace lwanga
