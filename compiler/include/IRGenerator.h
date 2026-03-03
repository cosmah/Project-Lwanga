#ifndef LWANGA_IR_GENERATOR_H
#define LWANGA_IR_GENERATOR_H

#include "AST.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DIBuilder.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <memory>
#include <unordered_map>
#include <string>

namespace lwanga {

// IR generation for Lwanga AST
class IRGenerator {
    llvm::Type* resolveLValueType(ExprAST* expr);
public:
    IRGenerator(const std::string& moduleName, const std::string& sourceFile = "", const std::string& targetTriple = "");
    ~IRGenerator();
    
    // Enable/disable debug info generation
    void setDebugInfo(bool enable) { generateDebugInfo = enable; }
    
    // Generate IR for entire program
    bool generate(ProgramAST* program);
    
    // Get the generated module
    llvm::Module* getModule() { return module.get(); }
    
    // Check if there were errors during generation
    bool hasErrors() const { return !errors.empty(); }
    
    // Get error messages
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    // LLVM context and module
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::Module> module;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    
    // Symbol table mapping variable names to LLVM values
    std::unordered_map<std::string, llvm::Value*> namedValues;
    
    // Track allocated types for variables (needed for LLVM 18 opaque pointers)
    std::unordered_map<std::string, llvm::Type*> namedTypes;
    // Track pointee types for pointer variables (needed for indexing opaque pointers)
    std::unordered_map<std::string, llvm::Type*> pointeeTypes;
    // Scope stacks for types
    std::vector<std::unordered_map<std::string, llvm::Type*>> typeScopeStack;
    std::vector<std::unordered_map<std::string, llvm::Type*>> pointeeScopeStack;
    
    // Track constants (map name to constant value)
    std::unordered_map<std::string, llvm::Constant*> constants;
    
    // Track struct definitions for field offset calculations
    std::unordered_map<std::string, StructAST*> structDefinitions;
    
    // Current function being generated
    llvm::Function* currentFunction;
    
    // Debug info generation
    bool generateDebugInfo;
    std::string sourceFilename;
    std::unique_ptr<llvm::DIBuilder> debugBuilder;
    llvm::DICompileUnit* compileUnit;
    llvm::DIFile* debugFile;
    std::vector<llvm::DIScope*> lexicalBlocks;
    
    // Error messages
    std::vector<std::string> errors;
    
    // Error reporting
    void reportError(const std::string& message);
    
    // Type conversion
    llvm::Type* convertType(const Type* type);
    
    // Generate declarations
    void generateStructDeclarations(ProgramAST* program);
    void generateFunctionDeclarations(ProgramAST* program);
    void generateConstantDeclarations(ProgramAST* program);
    
    // Generate _start function wrapper for freestanding executables
    void generateStartFunction();
    
    // Generate function
    void generateFunction(FunctionAST* func);
    
    // Generate statements
    void generateStatement(StmtAST* stmt);
    void generateVarDecl(VarDeclStmt* stmt);
    void generateAssignment(AssignmentStmt* stmt);
    void generateIf(IfStmt* stmt);
    void generateWhile(WhileStmt* stmt);
    void generateReturn(ReturnStmt* stmt);
    void generateExprStmt(ExprStmt* stmt);
    void generateUnsafeBlock(UnsafeBlockStmt* stmt);
    void generateAsm(AsmStmt* stmt);
    
    // Generate expressions
    llvm::Value* generateExpression(ExprAST* expr);
    llvm::Value* generateIntLiteral(IntLiteralExpr* expr);
    llvm::Value* generateStringLiteral(StringLiteralExpr* expr);
    llvm::Value* generateArrayLiteral(ArrayLiteralExpr* expr);
    llvm::Value* generateIdentifier(IdentifierExpr* expr);
    llvm::Value* generateBinary(BinaryExpr* expr);
    llvm::Value* generateUnary(UnaryExpr* expr);
    llvm::Value* generateCall(CallExpr* expr);
    llvm::Value* generateCast(CastExpr* expr);
    llvm::Value* generateStructInit(StructInitExpr* expr);
    llvm::Value* generateFieldAccess(FieldAccessExpr* expr);
    llvm::Value* generateArrayIndex(ArrayIndexExpr* expr);
    llvm::Value* generateSyscall(SyscallExpr* expr);
    llvm::Value* generateEncBlock(EncBlockExpr* expr);
    
    // Generate lvalue address (for address-of operator and assignments)
    llvm::Value* generateLValueAddress(ExprAST* expr);
    
    // Helper methods for address generation
    llvm::Value* generateFieldAddress(FieldAccessExpr* expr);
    llvm::Value* generateArrayElementAddress(ArrayIndexExpr* expr);
    
    // Evaluate constant expressions at compile time
    llvm::Constant* evaluateConstantExpression(ExprAST* expr);
    
    // Scope management
    void enterScope();
    void exitScope();
    
    // Debug info helpers
    void initializeDebugInfo();
    void finalizeDebugInfo();
    llvm::DIType* getDebugType(const Type* type);
    void emitLocation(uint32_t line, uint32_t column);
    
    // Helper to save/restore named values for nested scopes
    std::vector<std::unordered_map<std::string, llvm::Value*>> scopeStack;
};

} // namespace lwanga

#endif // LWANGA_IR_GENERATOR_H
