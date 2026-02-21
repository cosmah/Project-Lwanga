#ifndef LWANGA_TYPE_CHECKER_H
#define LWANGA_TYPE_CHECKER_H

#include "AST.h"
#include "SymbolTable.h"
#include "TypeSystem.h"
#include <vector>
#include <string>
#include <memory>

namespace lwanga {

struct TypeError {
    std::string message;
    uint32_t line;
    uint32_t column;
    
    TypeError(const std::string& msg, uint32_t ln = 0, uint32_t col = 0)
        : message(msg), line(ln), column(col) {}
};

class TypeChecker {
public:
    TypeChecker();
    
    // Main entry point
    bool check(ProgramAST* program);
    
    // Get collected errors
    const std::vector<TypeError>& getErrors() const { return errors; }
    bool hasErrors() const { return !errors.empty(); }

private:
    SymbolTable symbolTable;
    std::vector<TypeError> errors;
    FunctionAST* currentFunction;
    bool inUnsafeBlock;
    
    // Struct definitions registry
    std::unordered_map<std::string, StructAST*> structDefinitions;
    
    // Error reporting
    void reportError(const std::string& message, uint32_t line = 0, uint32_t column = 0);
    
    // First pass: collect all top-level declarations
    void collectDeclarations(ProgramAST* program);
    void collectFunction(FunctionAST* func);
    void collectStruct(StructAST* structDef);
    void collectConstant(ConstantAST* constant);
    
    // Second pass: type check everything
    void checkFunction(FunctionAST* func);
    void checkStatement(StmtAST* stmt);
    Type* checkExpression(ExprAST* expr);
    
    // Statement type checking
    void checkVarDecl(VarDeclStmt* stmt);
    void checkAssignment(AssignmentStmt* stmt);
    void checkIf(IfStmt* stmt);
    void checkWhile(WhileStmt* stmt);
    void checkReturn(ReturnStmt* stmt);
    void checkExprStmt(ExprStmt* stmt);
    void checkAsmStmt(AsmStmt* stmt);
    void checkUnsafeBlock(UnsafeBlockStmt* stmt);
    
    // Expression type checking
    Type* checkIntLiteral(IntLiteralExpr* expr);
    Type* checkStringLiteral(StringLiteralExpr* expr);
    Type* checkIdentifier(IdentifierExpr* expr);
    Type* checkBinary(BinaryExpr* expr);
    Type* checkUnary(UnaryExpr* expr);
    Type* checkCall(CallExpr* expr);
    Type* checkSyscall(SyscallExpr* expr);
    Type* checkEncBlock(EncBlockExpr* expr);
    Type* checkCast(CastExpr* expr);
    Type* checkArrayIndex(ArrayIndexExpr* expr);
    Type* checkFieldAccess(FieldAccessExpr* expr);
    Type* checkStructInit(StructInitExpr* expr);
    
    // Helper methods
    bool verifyAllPathsReturn(const std::vector<std::unique_ptr<StmtAST>>& stmts);
    bool statementReturns(StmtAST* stmt);
};

} // namespace lwanga

#endif // LWANGA_TYPE_CHECKER_H
