#include "SyntaxHighlighter.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <unistd.h>

void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " [OPTIONS] [FILE]\n\n"
              << "Syntax highlighter for Lwanga source code\n\n"
              << "Options:\n"
              << "  -t, --theme THEME    Color theme (default, dark, light, monokai)\n"
              << "  -n, --line-numbers   Show line numbers\n"
              << "  -s, --start LINE     Start line number (default: 1)\n"
              << "  -l, --lines N        Show only N lines\n"
              << "  -h, --highlight LINE Highlight specific line\n"
              << "  --no-color           Disable colors\n"
              << "  --help               Show this help message\n\n"
              << "Examples:\n"
              << "  " << progName << " file.lwanga\n"
              << "  " << progName << " -n file.lwanga\n"
              << "  " << progName << " -t monokai file.lwanga\n"
              << "  " << progName << " -n -h 10 file.lwanga\n"
              << "  cat file.lwanga | " << progName << "\n";
}

std::string readFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file '" << filename << "'\n";
        exit(1);
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::string readStdin() {
    std::ostringstream buffer;
    buffer << std::cin.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::string filename;
    std::string themeName = "default";
    bool showLineNumbers = false;
    bool noColor = false;
    int startLine = 1;
    int numLines = -1;
    int highlightLine = -1;
    
    // Parse arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "-t" || arg == "--theme") {
            if (i + 1 < argc) {
                themeName = argv[++i];
            } else {
                std::cerr << "Error: --theme requires an argument\n";
                return 1;
            }
        } else if (arg == "-n" || arg == "--line-numbers") {
            showLineNumbers = true;
        } else if (arg == "-s" || arg == "--start") {
            if (i + 1 < argc) {
                startLine = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: --start requires an argument\n";
                return 1;
            }
        } else if (arg == "-l" || arg == "--lines") {
            if (i + 1 < argc) {
                numLines = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: --lines requires an argument\n";
                return 1;
            }
        } else if (arg == "-h" || arg == "--highlight") {
            if (i + 1 < argc) {
                highlightLine = std::atoi(argv[++i]);
            } else {
                std::cerr << "Error: --highlight requires an argument\n";
                return 1;
            }
        } else if (arg == "--no-color") {
            noColor = true;
        } else if (arg[0] != '-') {
            filename = arg;
        } else {
            std::cerr << "Error: Unknown option '" << arg << "'\n";
            printUsage(argv[0]);
            return 1;
        }
    }
    
    // Get color theme
    lwanga::ColorTheme theme;
    if (themeName == "default") {
        theme = lwanga::ColorTheme::getDefault();
    } else if (themeName == "dark") {
        theme = lwanga::ColorTheme::getDark();
    } else if (themeName == "light") {
        theme = lwanga::ColorTheme::getLight();
    } else if (themeName == "monokai") {
        theme = lwanga::ColorTheme::getMonokai();
    } else {
        std::cerr << "Error: Unknown theme '" << themeName << "'\n";
        std::cerr << "Available themes: default, dark, light, monokai\n";
        return 1;
    }
    
    // Disable colors if not a TTY or --no-color specified
    if (noColor || !isatty(STDOUT_FILENO)) {
        noColor = true;
    }
    
    // Read source code
    std::string source;
    if (filename.empty()) {
        source = readStdin();
    } else {
        source = readFile(filename);
    }
    
    // Create highlighter
    lwanga::SyntaxHighlighter highlighter(theme);
    highlighter.setColorEnabled(!noColor);
    
    // Highlight and output
    std::string output;
    if (showLineNumbers) {
        if (highlightLine > 0) {
            // Show context around highlighted line
            int contextLines = 3;
            int start = std::max(1, highlightLine - contextLines);
            int end = highlightLine + contextLines;
            output = highlighter.highlightLines(source, start, end, highlightLine);
        } else if (numLines > 0) {
            // Show specific number of lines
            std::istringstream iss(source);
            std::ostringstream limited;
            std::string line;
            int count = 0;
            while (std::getline(iss, line) && count < numLines) {
                limited << line << "\n";
                count++;
            }
            output = highlighter.highlightWithLineNumbers(limited.str(), startLine);
        } else {
            output = highlighter.highlightWithLineNumbers(source, startLine);
        }
    } else {
        output = highlighter.highlight(source);
    }
    
    std::cout << output;
    
    return 0;
}
