#pragma once

#include <string>
#include <map>
#include <vector>

namespace lwanga {

// ANSI color codes
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    const std::string UNDERLINE = "\033[4m";
    
    // Foreground colors
    const std::string BLACK = "\033[30m";
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    const std::string WHITE = "\033[37m";
    
    // Bright colors
    const std::string BRIGHT_BLACK = "\033[90m";
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_MAGENTA = "\033[95m";
    const std::string BRIGHT_CYAN = "\033[96m";
    const std::string BRIGHT_WHITE = "\033[97m";
    
    // Background colors
    const std::string BG_BLACK = "\033[40m";
    const std::string BG_RED = "\033[41m";
    const std::string BG_GREEN = "\033[42m";
    const std::string BG_YELLOW = "\033[43m";
    const std::string BG_BLUE = "\033[44m";
    const std::string BG_MAGENTA = "\033[45m";
    const std::string BG_CYAN = "\033[46m";
    const std::string BG_WHITE = "\033[47m";
}

struct ColorTheme {
    std::string keyword;
    std::string type;
    std::string string;
    std::string number;
    std::string comment;
    std::string function;
    std::string operator_;
    std::string identifier;
    std::string unsafe;
    std::string asm_;
    std::string lineNumber;
    std::string background;
    
    static ColorTheme getDefault();
    static ColorTheme getDark();
    static ColorTheme getLight();
    static ColorTheme getMonokai();
};

class SyntaxHighlighter {
public:
    explicit SyntaxHighlighter(const ColorTheme& theme = ColorTheme::getDefault());
    
    // Highlight entire source code
    std::string highlight(const std::string& source);
    
    // Highlight with line numbers
    std::string highlightWithLineNumbers(const std::string& source, int startLine = 1);
    
    // Highlight specific lines (for error context)
    std::string highlightLines(const std::string& source, int startLine, int endLine, int highlightLine = -1);
    
    // Highlight a single token
    std::string highlightToken(const std::string& token, const std::string& tokenType);
    
    // Enable/disable colors
    void setColorEnabled(bool enabled);
    bool isColorEnabled() const;
    
private:
    ColorTheme theme;
    bool colorEnabled = true;
    
    // Helper methods
    std::string colorize(const std::string& text, const std::string& color);
    bool isKeyword(const std::string& word);
    bool isType(const std::string& word);
    bool isOperator(char c);
    std::string getLineNumberPrefix(int lineNum, int maxLineNum, bool highlight = false);
};

} // namespace lwanga
