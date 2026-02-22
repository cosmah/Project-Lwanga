#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <chrono>
#include <llvm/Support/CommandLine.h>
#include <llvm/Support/InitLLVM.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include "Preprocessor.h"
#include "Lexer.h"
#include "Parser.h"
#include "TypeChecker.h"
#include "IRGenerator.h"
#include "Backend.h"
#include "ModuleLoader.h"

using namespace llvm;
using namespace lwanga;

// Command line options
static cl::opt<std::string> InputFilename(cl::Positional, 
    cl::desc("<input file>"), 
    cl::Required);

static cl::opt<std::string> OutputFilename("o",
    cl::desc("Specify output filename"),
    cl::value_desc("filename"),
    cl::init("a.out"));

static cl::opt<unsigned> OptLevel("O",
    cl::desc("Optimization level (0-3)"),
    cl::value_desc("level"),
    cl::init(0));

static cl::opt<bool> JITMode("jit",
    cl::desc("Execute code immediately using JIT"),
    cl::init(false));

static cl::opt<bool> Verbose("v",
    cl::desc("Enable verbose output"),
    cl::init(false));

static cl::opt<std::string> TargetTriple("target",
    cl::desc("Target triple for cross-compilation"),
    cl::value_desc("triple"),
    cl::init(""));

static cl::opt<bool> DebugInfo("g",
    cl::desc("Generate debug information"),
    cl::init(false));

static cl::opt<bool> Obfuscate("obfuscate",
    cl::desc("Enable code obfuscation"),
    cl::init(false));

static cl::opt<bool> PIC("pic",
    cl::desc("Generate position-independent code"),
    cl::init(false));

// Helper function to read file contents
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file: " << path << "\n";
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

// Helper function to display error with source context
void displayError(const std::string& message, uint32_t line, uint32_t column, 
                  const std::string& source) {
    std::cerr << "Error";
    if (line > 0) {
        std::cerr << " at line " << line;
        if (column > 0) {
            std::cerr << ", column " << column;
        }
    }
    std::cerr << ": " << message << "\n";
    
    // Show source context if available
    if (line > 0 && !source.empty()) {
        std::istringstream sourceStream(source);
        std::string currentLine;
        uint32_t currentLineNum = 1;
        
        while (std::getline(sourceStream, currentLine)) {
            if (currentLineNum == line) {
                std::cerr << "  " << currentLine << "\n";
                if (column > 0) {
                    std::cerr << "  ";
                    for (uint32_t i = 1; i < column; ++i) {
                        std::cerr << " ";
                    }
                    std::cerr << "^\n";
                }
                break;
            }
            currentLineNum++;
        }
    }
}

int main(int argc, char **argv) {
    // Initialize LLVM
    InitLLVM X(argc, argv);
    
    // Initialize native target for JIT
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();
    InitializeNativeTargetAsmParser();
    
    // Initialize all targets for cross-compilation
    InitializeAllTargets();
    InitializeAllTargetMCs();
    InitializeAllAsmPrinters();
    InitializeAllAsmParsers();
    
    // Parse command line options
    cl::ParseCommandLineOptions(argc, argv, "Lwanga Compiler\n");
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    if (Verbose) {
        std::cout << "Lwanga Compiler v0.1.0\n";
        std::cout << "Input file: " << InputFilename << "\n";
        std::cout << "Output file: " << OutputFilename << "\n";
        std::cout << "Optimization level: -O" << OptLevel << "\n";
        if (JITMode) {
            std::cout << "Mode: JIT execution\n";
        }
        if (!TargetTriple.empty()) {
            std::cout << "Target: " << TargetTriple << "\n";
        }
        if (Obfuscate) {
            std::cout << "Obfuscation: enabled\n";
        }
        if (PIC) {
            std::cout << "Position-independent code: enabled\n";
        }
        if (DebugInfo) {
            std::cout << "Debug info: enabled\n";
        }
        std::cout << "\n";
    }
    
    // Stage 1: Module Loading
    if (Verbose) {
        std::cout << "[1/6] Loading modules...\n";
    }
    
    ModuleLoader loader;
    auto program = loader.loadModule(InputFilename);
    
    if (loader.hasErrors()) {
        std::cerr << "Module loading failed:\n";
        for (const auto& error : loader.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    
    if (!program) {
        std::cerr << "Error: Failed to load module\n";
        return 1;
    }
    
    if (Verbose) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime).count();
        std::cout << "  Loaded in " << elapsed << "ms\n\n";
    }
    
    // Stage 2: Type Checking
    if (Verbose) {
        std::cout << "[2/6] Type checking...\n";
    }
    
    auto typeCheckStart = std::chrono::high_resolution_clock::now();
    
    TypeChecker typeChecker;
    if (!typeChecker.check(program.get())) {
        std::cerr << "Type checking failed:\n";
        std::string source = readFile(InputFilename);
        for (const auto& error : typeChecker.getErrors()) {
            displayError(error.message, error.line, error.column, source);
        }
        return 1;
    }
    
    if (Verbose) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - typeCheckStart).count();
        std::cout << "  Type checked in " << elapsed << "ms\n\n";
    }
    
    // Stage 3: IR Generation
    if (Verbose) {
        std::cout << "[3/6] Generating LLVM IR...\n";
    }
    
    auto irGenStart = std::chrono::high_resolution_clock::now();
    
    IRGenerator irGen("lwanga_module", InputFilename);
    irGen.setDebugInfo(DebugInfo);
    
    if (!irGen.generate(program.get())) {
        std::cerr << "IR generation failed:\n";
        for (const auto& error : irGen.getErrors()) {
            std::cerr << "  " << error << "\n";
        }
        return 1;
    }
    
    if (Verbose) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - irGenStart).count();
        std::cout << "  Generated IR in " << elapsed << "ms\n\n";
    }
    
    // Stage 4: Backend Setup
    if (Verbose) {
        std::cout << "[4/6] Setting up backend...\n";
    }
    
    Backend backend(irGen.getModule(), OptLevel);
    
    // Set target triple if specified
    if (!TargetTriple.empty()) {
        if (!backend.setTargetTriple(TargetTriple)) {
            std::cerr << "Error: " << backend.getError() << "\n";
            return 1;
        }
    }
    
    // Configure backend options
    backend.setPositionIndependent(PIC);
    backend.setDebugInfo(DebugInfo);
    backend.setObfuscate(Obfuscate);
    
    if (Verbose) {
        std::cout << "  Backend configured\n\n";
    }
    
    // Stage 5: Optimization and Obfuscation
    if (Verbose) {
        std::cout << "[5/6] Optimizing";
        if (Obfuscate) {
            std::cout << " and obfuscating";
        }
        std::cout << "...\n";
    }
    
    auto optStart = std::chrono::high_resolution_clock::now();
    
    // Run optimization passes
    backend.optimize();
    
    // Run obfuscation passes if enabled
    if (Obfuscate) {
        backend.obfuscate();
    }
    
    if (Verbose) {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - optStart).count();
        std::cout << "  Completed in " << elapsed << "ms\n\n";
    }
    
    // Stage 6: Code Generation or JIT Execution
    if (JITMode) {
        if (Verbose) {
            std::cout << "[6/6] Executing with JIT...\n\n";
        }
        
        int exitCode = backend.executeJIT();
        
        if (backend.hasErrors()) {
            std::cerr << "JIT execution failed: " << backend.getError() << "\n";
            return 1;
        }
        
        if (Verbose) {
            auto totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - startTime).count();
            std::cout << "\nProgram exited with code " << exitCode << "\n";
            std::cout << "Total time: " << totalElapsed << "ms\n";
        }
        
        return exitCode;
    } else {
        if (Verbose) {
            std::cout << "[6/6] Generating executable...\n";
        }
        
        auto codegenStart = std::chrono::high_resolution_clock::now();
        
        if (!backend.generateExecutable(OutputFilename)) {
            std::cerr << "Code generation failed: " << backend.getError() << "\n";
            return 1;
        }
        
        if (Verbose) {
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - codegenStart).count();
            std::cout << "  Generated in " << elapsed << "ms\n\n";
        }
        
        auto totalElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - startTime).count();
        
        if (Verbose) {
            std::cout << "Compilation successful!\n";
            std::cout << "Output: " << OutputFilename << "\n";
            std::cout << "Total time: " << totalElapsed << "ms\n";
        } else {
            std::cout << "Compilation successful: " << OutputFilename << "\n";
        }
    }
    
    return 0;
}
