#include "SyntaxHighlighter.h"
#include "Lexer.h"
#include "Token.h"
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace lwanga {

ColorTheme ColorTheme::getDefault() {
    ColorTheme theme;
    theme.keyword = Color::BOLD + Color::BLUE;
    theme.type = Color::CYAN;
    theme.string = Color::GREEN;
    theme.number = Color::MAGENTA;
    theme.comment = Color::DIM + Color::BRIGHT_BLACK;
    theme.function = Color::BOLD + Color::YELLOW;
    theme.operator_ = Color::WHITE;
    theme.identifier = Color::WHITE;
    theme.unsafe = Color::BOLD + Color::RED;
    theme.asm_ = Color::BG_BLUE + Color::WHITE;
    theme.lineNumber = Color::DIM + Color::CYAN;
    theme.background = "";
    return theme;
}

ColorTheme ColorTheme::getDark() {
    ColorTheme theme;
    theme.keyword = Color::BOLD + Color::BRIGHT_BLUE;
    theme.type = Color::BRIGHT_CYAN;
    theme.string = Color::BRIGHT_GREEN;
    theme.number = Color::BRIGHT_MAGENTA;
    theme.comment = Color::DIM + Color::BRIGHT_BLACK;
    theme.function = Color::BOLD + Color::BRIGHT_YELLOW;
    theme.operator_ = Color::BRIGHT_WHITE;
    theme.identifier = Color::BRIGHT_WHITE;
    theme.unsafe = Color::BOLD + Color::BRIGHT_RED;
    theme.asm_ = Color::BG_BLUE + Color::BRIGHT_WHITE;
    theme.lineNumber = Color::DIM + Color::BRIGHT_CYAN;
    theme.background = "";
    return theme;
}

ColorTheme ColorTheme::getLight() {
    ColorTheme theme;
    theme.keyword = Color::BOLD + Color::BLUE;
    theme.type = Color::CYAN;
    theme.string = Color::GREEN;
    theme.number = Color::MAGENTA;
    theme.comment = Color::DIM + Color::BLACK;
    theme.function = Color::BOLD + Color::YELLOW;
    theme.operator_ = Color::BLACK;
    theme.identifier = Color::BLACK;
    theme.unsafe = Color::BOLD + Color::RED;
    theme.asm_ = Color::BG_CYAN + Color::BLACK;
    theme.lineNumber = Color::DIM + Color::BLUE;
    theme.background = "";
    return theme;
}

ColorTheme ColorTheme::getMonokai() {
    ColorTheme theme;
    theme.keyword = Color::BOLD + Color::MAGENTA;
    theme.type = Color::CYAN;
    theme.string = Color::YELLOW;
    theme.number = Color::MAGENTA;
    theme.comment = Color::DIM + Color::BRIGHT_BLACK;
    theme.function = Color::BOLD + Color::GREEN;
    theme.operator_ = Color::RED;
    theme.identifier = Color::WHITE;
    theme.unsafe = Color::BOLD + Color::RED;
    theme.asm_ = Color::BG_MAGENTA + Color::WHITE;
    theme.lineNumber = Color::DIM + Color::BRIGHT_BLACK;
    theme.background = "";
    return theme;
}

SyntaxHighlighter::SyntaxHighlighter(const ColorTheme& theme)
    : theme(theme), colorEnabled(true) {}

std::string SyntaxHighlighter::colorize(const std::string& text, const std::string& color) {
    if (!colorEnabled || color.empty()) {
        return text;
    }
    return color + text + Color::RESET;
}

bool SyntaxHighlighter::isKeyword(const std::string& word) {
    static const std::vector<std::string> keywords = {
        "fn", "return", "if", "else", "while", "asm", "syscall", "enc",
        "naked", "let", "mut", "struct", "const", "import", "unsafe", "as", "packed"
    };
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool SyntaxHighlighter::isType(const std::string& word) {
    static const std::vector<std::string> types = {
        "u8", "u16", "u32", "u64", "ptr", "register"
    };
    return std::find(types.begin(), types.end(), word) != types.end();
}

bool SyntaxHighlighter::isOperator(char c) {
    return c == '+' || c == '-' || c == '*' || c == '/' || c == '%' ||
           c == '&' || c == '|' || c == '^' || c == '~' || c == '!' ||
           c == '=' || c == '<' || c == '>' || c == '(' || c == ')' ||
           c == '{' || c == '}' || c == '[' || c == ']' || c == ';' ||
           c == ',' || c == ':' || c == '.';
}

std::string SyntaxHighlighter::highlightToken(const std::string& token, const std::string& tokenType) {
    if (tokenType == "keyword") {
        return colorize(token, theme.keyword);
    } else if (tokenType == "type") {
        return colorize(token, theme.type);
    } else if (tokenType == "string") {
        return colorize(token, theme.string);
    } else if (tokenType == "number") {
        return colorize(token, theme.number);
    } else if (tokenType == "comment") {
        return colorize(token, theme.comment);
    } else if (tokenType == "function") {
        return colorize(token, theme.function);
    } else if (tokenType == "operator") {
        return colorize(token, theme.operator_);
    } else if (tokenType == "unsafe") {
        return colorize(token, theme.unsafe);
    } else if (tokenType == "asm") {
        return colorize(token, theme.asm_);
    }
    return colorize(token, theme.identifier);
}

std::string SyntaxHighlighter::highlight(const std::string& source) {
    if (!colorEnabled) {
        return source;
    }

    Lexer lexer(source);
    std::ostringstream result;
    size_t lastPos = 0;
    
    while (true) {
        Token tok = lexer.nextToken();
        if (tok.isEOF()) break;
        
        // Find token position in source
        size_t tokPos = source.find(tok.lexeme, lastPos);
        if (tokPos != std::string::npos) {
            // Add any whitespace/comments before token
            if (tokPos > lastPos) {
                result << source.substr(lastPos, tokPos - lastPos);
            }
            
            // Colorize token based on type
            std::string colored;
            switch (tok.type) {
                case TokenType::TOK_FN:
                case TokenType::TOK_RETURN:
                case TokenType::TOK_IF:
                case TokenType::TOK_ELSE:
                case TokenType::TOK_WHILE:
                case TokenType::TOK_LET:
                case TokenType::TOK_MUT:
                case TokenType::TOK_STRUCT:
                case TokenType::TOK_CONST:
                case TokenType::TOK_IMPORT:
                case TokenType::TOK_AS:
                case TokenType::TOK_PACKED:
                case TokenType::TOK_SYSCALL:
                case TokenType::TOK_ENC:
                case TokenType::TOK_NAKED:
                    colored = colorize(tok.lexeme, theme.keyword);
                    break;
                    
                case TokenType::TOK_UNSAFE:
                    colored = colorize(tok.lexeme, theme.unsafe);
                    break;
                    
                case TokenType::TOK_ASM:
                    colored = colorize(tok.lexeme, theme.asm_);
                    break;
                    
                case TokenType::TOK_U8:
                case TokenType::TOK_U16:
                case TokenType::TOK_U32:
                case TokenType::TOK_U64:
                case TokenType::TOK_PTR:
                case TokenType::TOK_REGISTER:
                    colored = colorize(tok.lexeme, theme.type);
                    break;
                    
                case TokenType::TOK_NUMBER:
                    colored = colorize(tok.lexeme, theme.number);
                    break;
                    
                case TokenType::TOK_STRING:
                    colored = colorize(tok.lexeme, theme.string);
                    break;
                    
                case TokenType::TOK_IDENTIFIER:
                    colored = colorize(tok.lexeme, theme.identifier);
                    break;
                    
                default:
                    colored = colorize(tok.lexeme, theme.operator_);
                    break;
            }
            
            result << colored;
            lastPos = tokPos + tok.lexeme.length();
        }
    }
    
    // Add any remaining content
    if (lastPos < source.length()) {
        result << source.substr(lastPos);
    }
    
    return result.str();
}

std::string SyntaxHighlighter::getLineNumberPrefix(int lineNum, int maxLineNum, bool highlight) {
    std::ostringstream oss;
    int width = std::to_string(maxLineNum).length();
    
    if (highlight) {
        oss << colorize(std::string(width + 2, ' ') + "| ", Color::BOLD + Color::RED);
    } else {
        oss << colorize(std::to_string(lineNum), theme.lineNumber);
        oss << colorize(std::string(width - std::to_string(lineNum).length(), ' '), theme.lineNumber);
        oss << colorize(" | ", theme.lineNumber);
    }
    
    return oss.str();
}

std::string SyntaxHighlighter::highlightWithLineNumbers(const std::string& source, int startLine) {
    std::istringstream iss(source);
    std::ostringstream result;
    std::string line;
    int lineNum = startLine;
    int maxLineNum = startLine + std::count(source.begin(), source.end(), '\n');
    
    while (std::getline(iss, line)) {
        result << getLineNumberPrefix(lineNum, maxLineNum, false);
        result << highlight(line) << "\n";
        lineNum++;
    }
    
    return result.str();
}

std::string SyntaxHighlighter::highlightLines(const std::string& source, int startLine, int endLine, int highlightLine) {
    std::istringstream iss(source);
    std::ostringstream result;
    std::string line;
    int lineNum = 1;
    int maxLineNum = std::count(source.begin(), source.end(), '\n') + 1;
    
    while (std::getline(iss, line)) {
        if (lineNum >= startLine && lineNum <= endLine) {
            bool isHighlight = (lineNum == highlightLine);
            result << getLineNumberPrefix(lineNum, maxLineNum, isHighlight);
            
            if (isHighlight) {
                result << colorize(line, Color::BOLD + Color::WHITE);
            } else {
                result << highlight(line);
            }
            result << "\n";
        }
        lineNum++;
    }
    
    return result.str();
}

void SyntaxHighlighter::setColorEnabled(bool enabled) {
    colorEnabled = enabled;
}

bool SyntaxHighlighter::isColorEnabled() const {
    return colorEnabled;
}

} // namespace lwanga
