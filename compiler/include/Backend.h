#ifndef LWANGA_BACKEND_H
#define LWANGA_BACKEND_H

#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Target/TargetMachine.h>
#include <memory>
#include <string>

namespace lwanga {

// Backend for code generation and optimization
class Backend {
public:
    Backend(llvm::Module* module, unsigned optLevel = 0);
    ~Backend();
    
    // Set target triple (for cross-compilation)
    bool setTargetTriple(const std::string& triple);
    
    // Set code generation options
    void setPositionIndependent(bool pic);
    void setDebugInfo(bool debug);
    void setObfuscate(bool obfuscate);
    
    // Run optimization passes
    void optimize();
    
    // Run obfuscation passes
    void obfuscate();
    
    // Generate object file
    bool generateObjectFile(const std::string& filename);
    
    // Generate static executable
    bool generateExecutable(const std::string& filename);
    
    // JIT compile and execute
    int executeJIT();
    
    // Check if there were errors
    bool hasErrors() const { return !errorMessage.empty(); }
    
    // Get error message
    const std::string& getError() const { return errorMessage; }

private:
    llvm::Module* module;
    unsigned optLevel;
    bool pic;
    bool debugInfo;
    bool enableObfuscation;
    std::string targetTriple;
    std::unique_ptr<llvm::TargetMachine> targetMachine;
    std::string errorMessage;
    
    // Initialize target machine
    bool initializeTargetMachine();
    
    // Run optimization passes based on optimization level
    void runOptimizationPasses();
    
    // Set error message
    void setError(const std::string& error);
};

} // namespace lwanga

#endif // LWANGA_BACKEND_H