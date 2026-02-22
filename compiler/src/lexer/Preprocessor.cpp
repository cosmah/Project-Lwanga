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

void Preprocessor::define(const std::string& symbol) {
    symbols[symbol] = true;
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
            
            if (directive == "if") {
                depth++;
            } else if (directive == "endif") {
                depth--;
                if (depth == 0) {
                    // Skip to end of line
                    while (currentChar() != '\n' && currentChar() != '\0') {
                        advance();
                    }
                    if (currentChar() == '\n') {
                        advance();
                    }
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
            
            if (directive == "if") {
                depth++;
                // Skip rest of line
                while (currentChar() != '\n' && currentChar() != '\0') {
                    advance();
                }
                if (currentChar() == '\n') {
                    advance();
                }
            } else if (directive == "else" && depth == 1) {
                // Skip to end of line
                while (currentChar() != '\n' && currentChar() != '\0') {
                    advance();
                }
                if (currentChar() == '\n') {
                    advance();
                }
                return;
            } else if (directive == "endif") {
                depth--;
                if (depth == 0) {
                    // Skip to end of line
                    while (currentChar() != '\n' && currentChar() != '\0') {
                        advance();
                    }
                    if (currentChar() == '\n') {
                        advance();
                    }
                    return;
                }
                // Skip rest of line
                while (currentChar() != '\n' && currentChar() != '\0') {
                    advance();
                }
                if (currentChar() == '\n') {
                    advance();
                }
            } else {
                // Unknown directive, skip rest of line
                while (currentChar() != '\n' && currentChar() != '\0') {
                    advance();
                }
                if (currentChar() == '\n') {
                    advance();
                }
            }
        } else {
            advance();
        }
    }
}

bool Preprocessor::processDirective(std::string& output) {
    // We're at '#'
    advance();
    skipWhitespace();
    
    std::string directive = readIdentifier();
    
    if (directive == "if") {
        skipWhitespace();
        // Read condition until end of line
        std::string condition;
        while (currentChar() != '\n' && currentChar() != '\0') {
            condition += currentChar();
            advance();
        }
        if (currentChar() == '\n') {
            advance();
        }
        
        bool conditionResult = evaluateCondition(condition);
        
        if (conditionResult) {
            // Include this block, continue processing
            return true;
        } else {
            // Skip until #else or #endif
            skipUntilElseOrEndif(1);
            return true;
        }
    } else if (directive == "else") {
        // We're in an #else block, which means the #if was true
        // Skip until #endif
        skipUntilEndif(1);
        return true;
    } else if (directive == "endif") {
        // Just skip to end of line
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
        if (currentChar() == '\n') {
            advance();
        }
        return true;
    } else if (directive == "define") {
        skipWhitespace();
        std::string symbol = readIdentifier();
        if (!symbol.empty()) {
            define(symbol);
        }
        // Skip to end of line
        while (currentChar() != '\n' && currentChar() != '\0') {
            advance();
        }
        if (currentChar() == '\n') {
            advance();
        }
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
                for (size_t i = cursor - 1; i > 0; i--) {
                    if (source[i] == '\n') {
                        break;
                    }
                    if (source[i] != ' ' && source[i] != '\t') {
                        atLineStart = false;
                        break;
                    }
                }
            }
            
            if (atLineStart) {
                processDirective(output);
                continue;
            }
        }
        
        // Regular character, copy to output
        output += currentChar();
        advance();
    }
    
    return output;
}

} // namespace lwanga
