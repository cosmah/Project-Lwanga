#pragma once

#include <string>
#include <iostream>

namespace lwanga {

// ANSI color codes
namespace Color {
    const std::string RESET = "\033[0m";
    const std::string BOLD = "\033[1m";
    const std::string DIM = "\033[2m";
    
    const std::string RED = "\033[31m";
    const std::string GREEN = "\033[32m";
    const std::string YELLOW = "\033[33m";
    const std::string BLUE = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN = "\033[36m";
    
    const std::string BRIGHT_RED = "\033[91m";
    const std::string BRIGHT_GREEN = "\033[92m";
    const std::string BRIGHT_YELLOW = "\033[93m";
    const std::string BRIGHT_BLUE = "\033[94m";
    const std::string BRIGHT_CYAN = "\033[96m";
}

inline void printLogo() {
    std::cout << Color::BRIGHT_CYAN << Color::BOLD
              << "  ╦  ╦ ╦╔═╗╔╗╔╔═╗╔═╗\n"
              << "  ║  ║║║╠═╣║║║║ ╦╠═╣\n"
              << "  ╩═╝╚╩╝╩ ╩╝╚╝╚═╝╩ ╩\n"
              << Color::RESET
              << Color::DIM << "  Security-Focused Systems Language\n" << Color::RESET
              << "\n";
}

inline void printError(const std::string& message) {
    std::cerr << Color::BOLD << Color::RED << "error: " << Color::RESET << message << "\n";
}

inline void printWarning(const std::string& message) {
    std::cerr << Color::BOLD << Color::YELLOW << "warning: " << Color::RESET << message << "\n";
}

inline void printInfo(const std::string& message) {
    std::cout << Color::BOLD << Color::BLUE << "info: " << Color::RESET << message << "\n";
}

inline void printSuccess(const std::string& message) {
    std::cout << Color::BOLD << Color::GREEN << "success: " << Color::RESET << message << "\n";
}

} // namespace lwanga
