#include "Formatter.h"
#include "../../../compiler/include/Lexer.h"
#include "../../../compiler/include/Parser.h"
#include "../../../compiler/include/Preprocessor.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <filesystem>

using namespace lwanga;

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_CYAN    "\033[36m"

struct Options {
    std::vector<std::string> files;
    bool check = false;          // --check: check if files are formatted
    bool write = false;          // --write: write formatted output to files
    bool addHeader = false;      // --add-header: add logo header
    bool removeHeader = false;   // --remove-header: remove logo header
    bool recursive = false;      // --recursive: process directories recursively
    std::string configFile = ".lwangafmt.toml";
    std::string logoStyle = "";  // --logo-style: override logo style
    bool verbose = false;        // --verbose: verbose output
    bool color = true;           // --color: colorize output
};

void printUsage(const char* progName) {
    std::cout << COLOR_BOLD << "lwangafmt" << COLOR_RESET << " - Lwanga Code Formatter\n\n";
    std::cout << "Usage: " << progName << " [options] <files...>\n\n";
    std::cout << "Options:\n";
    std::cout << "  --check              Check if files are formatted (exit 1 if not)\n";
    std::cout << "  --write              Write formatted output to files\n";
    std::cout << "  --add-header         Add logo header to files\n";
    std::cout << "  --remove-header      Remove logo header from files\n";
    std::cout << "  --recursive          Process directories recursively\n";
    std::cout << "  --logo-style STYLE   Logo style: minimal, block, banner, none\n";
    std::cout << "  --config FILE        Config file (default: .lwangafmt.toml)\n";
    std::cout << "  --verbose, -v        Verbose output\n";
    std::cout << "  --no-color           Disable colored output\n";
    std::cout << "  --help, -h           Show this help\n\n";
    std::cout << "Examples:\n";
    std::cout << "  " << progName << " --write src/**/*.lwanga\n";
    std::cout << "  " << progName << " --check main.lwanga\n";
    std::cout << "  " << progName << " --add-header --logo-style=banner main.lwanga\n";
}

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to write file: " + path);
    }
    file << content;
}

bool isFormatted(const std::string& original, const std::string& formatted) {
    return original == formatted;
}

void collectFiles(const std::string& path, std::vector<std::string>& files, bool recursive) {
    if (std::filesystem::is_regular_file(path)) {
        if (path.ends_with(".lwanga") || path.ends_with(".lwangah")) {
            files.push_back(path);
        }
    } else if (std::filesystem::is_directory(path)) {
        if (recursive) {
            for (const auto& entry : std::filesystem::recursive_directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().string();
                    if (filename.ends_with(".lwanga") || filename.ends_with(".lwangah")) {
                        files.push_back(filename);
                    }
                }
            }
        } else {
            for (const auto& entry : std::filesystem::directory_iterator(path)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().string();
                    if (filename.ends_with(".lwanga") || filename.ends_with(".lwangah")) {
                        files.push_back(filename);
                    }
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    Options opts;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printUsage(argv[0]);
            return 0;
        } else if (arg == "--check") {
            opts.check = true;
        } else if (arg == "--write") {
            opts.write = true;
        } else if (arg == "--add-header") {
            opts.addHeader = true;
        } else if (arg == "--remove-header") {
            opts.removeHeader = true;
        } else if (arg == "--recursive") {
            opts.recursive = true;
        } else if (arg == "--verbose" || arg == "-v") {
            opts.verbose = true;
        } else if (arg == "--no-color") {
            opts.color = false;
        } else if (arg == "--logo-style") {
            if (i + 1 < argc) {
                opts.logoStyle = argv[++i];
            }
        } else if (arg.substr(0, 13) == "--logo-style=") {
            opts.logoStyle = arg.substr(13);
        } else if (arg == "--config" && i + 1 < argc) {
            opts.configFile = argv[++i];
        } else if (arg[0] != '-') {
            opts.files.push_back(arg);
        } else {
            std::cerr << COLOR_RED << "Unknown option: " << arg << COLOR_RESET << "\n";
            return 1;
        }
    }
    
    if (opts.files.empty()) {
        std::cerr << COLOR_RED << "Error: No files specified" << COLOR_RESET << "\n\n";
        printUsage(argv[0]);
        return 1;
    }
    
    // Collect all files to process
    std::vector<std::string> allFiles;
    for (const auto& path : opts.files) {
        collectFiles(path, allFiles, opts.recursive);
    }
    
    if (allFiles.empty()) {
        std::cerr << COLOR_YELLOW << "Warning: No .lwanga files found" << COLOR_RESET << "\n";
        return 0;
    }
    
    // Load configuration
    FormatterConfig config = FormatterConfig::getDefault();
    if (std::filesystem::exists(opts.configFile)) {
        config = FormatterConfig::loadFromFile(opts.configFile);
    }
    
    // Override logo style if specified
    if (!opts.logoStyle.empty()) {
        config.logoStyle = opts.logoStyle;
    }
    
    // Enable logo for add-header
    if (opts.addHeader) {
        config.addLogo = true;
        config.includeMetadata = true;
    }
    
    Formatter formatter(config);
    
    int filesProcessed = 0;
    int filesNeedFormatting = 0;
    int errors = 0;
    
    // Process each file
    for (const auto& filepath : allFiles) {
        try {
            if (opts.verbose) {
                std::cout << COLOR_CYAN << "Processing: " << filepath << COLOR_RESET << "\n";
            }
            
            // Read source file
            std::string source = readFile(filepath);
            
            // Preprocess
            Preprocessor preprocessor(source);
            std::string preprocessed = preprocessor.process();
            
            // Parse
            Lexer lexer(preprocessed);
            Parser parser(lexer);
            auto program = parser.parse();
            
            if (parser.hasErrors()) {
                std::cerr << COLOR_RED << "Parse errors in " << filepath << ":" << COLOR_RESET << "\n";
                for (const auto& error : parser.getErrors()) {
                    std::cerr << "  Line " << error.line << ": " << error.message << "\n";
                }
                errors++;
                continue;
            }
            
            // Format
            std::string formatted;
            if (opts.addHeader) {
                formatted = formatter.generateHeader(filepath) + "\n";
            }
            formatted += formatter.format(program.get());
            
            // Check mode
            if (opts.check) {
                if (!isFormatted(source, formatted)) {
                    std::cout << COLOR_YELLOW << "✗ " << filepath << " needs formatting" << COLOR_RESET << "\n";
                    filesNeedFormatting++;
                } else if (opts.verbose) {
                    std::cout << COLOR_GREEN << "✓ " << filepath << " is formatted" << COLOR_RESET << "\n";
                }
            }
            // Write mode
            else if (opts.write) {
                writeFile(filepath, formatted);
                std::cout << COLOR_GREEN << "✓ Formatted: " << filepath << COLOR_RESET << "\n";
            }
            // Print mode (default)
            else {
                std::cout << formatted;
            }
            
            filesProcessed++;
            
        } catch (const std::exception& e) {
            std::cerr << COLOR_RED << "Error processing " << filepath << ": " 
                     << e.what() << COLOR_RESET << "\n";
            errors++;
        }
    }
    
    // Summary
    if (opts.verbose || opts.check) {
        std::cout << "\n" << COLOR_BOLD << "Summary:" << COLOR_RESET << "\n";
        std::cout << "  Files processed: " << filesProcessed << "\n";
        if (opts.check) {
            std::cout << "  Files need formatting: " << filesNeedFormatting << "\n";
        }
        if (errors > 0) {
            std::cout << COLOR_RED << "  Errors: " << errors << COLOR_RESET << "\n";
        }
    }
    
    // Exit code
    if (errors > 0) {
        return 1;
    }
    if (opts.check && filesNeedFormatting > 0) {
        return 1;
    }
    
    return 0;
}
