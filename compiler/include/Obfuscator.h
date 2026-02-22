#ifndef LWANGA_OBFUSCATOR_H
#define LWANGA_OBFUSCATOR_H

#include <llvm/IR/Module.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/IRBuilder.h>
#include <random>
#include <vector>

namespace lwanga {

// Obfuscation transformations for LLVM IR
class Obfuscator {
public:
    explicit Obfuscator(llvm::Module* module, unsigned seed = 0);
    
    // Apply all obfuscation passes
    void obfuscate();
    
    // Individual obfuscation passes
    void flattenControlFlow();
    void insertOpaquePredicates();
    void substituteInstructions();
    
    // Check if there were errors
    bool hasErrors() const { return !errorMessage.empty(); }
    
    // Get error message
    const std::string& getError() const { return errorMessage; }

private:
    llvm::Module* module;
    std::mt19937 rng;
    std::string errorMessage;
    
    // Control flow flattening helpers
    void flattenFunction(llvm::Function* func);
    
    // Opaque predicate helpers
    void insertOpaquePredicateInFunction(llvm::Function* func);
    llvm::Value* createAlwaysTruePredicate(llvm::IRBuilder<>& builder);
    llvm::Value* createAlwaysFalsePredicate(llvm::IRBuilder<>& builder);
    
    // Instruction substitution helpers
    void substituteInstructionsInFunction(llvm::Function* func);
    void substituteAdd(llvm::BinaryOperator* inst);
    void substituteSub(llvm::BinaryOperator* inst);
    void substituteXor(llvm::BinaryOperator* inst);
    
    // Utility
    void setError(const std::string& error);
    bool shouldObfuscateFunction(llvm::Function* func);
};

} // namespace lwanga

#endif // LWANGA_OBFUSCATOR_H
