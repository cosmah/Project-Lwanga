#include "TypeChecker.h"
#include <iostream>

namespace lwanga {

TypeChecker::TypeChecker() 
    : currentFunction(nullptr), inUnsafeBlock(false) {
}

void TypeChecker::reportError(const std::string& message, uint32_t line, uint32_t column) {
    errors.push_back(TypeError(message, line, column));
}

bool TypeChecker::check(ProgramAST* program) {
    if (program == nullptr) {
        reportError("Null program");
        return false;
    }
    
    // First pass: collect all declarations
    collectDeclarations(program);
    
    // Second pass: type check all functions
    for (auto& func : program->functions) {
        checkFunction(func.get());
    }
    
    return !hasErrors();
}

void TypeChecker::collectDeclarations(ProgramAST* program) {
    // Collect structs first (they may be used in function signatures)
    for (auto& structDef : program->structs) {
        collectStruct(structDef.get());
    }
    
    // Collect constants
    for (auto& constant : program->constants) {
        collectConstant(constant.get());
    }
    
    // Collect functions
    for (auto& func : program->functions) {
        collectFunction(func.get());
    }
}

void TypeChecker::collectFunction(FunctionAST* func) {
    // Create function symbol
    auto funcType = std::make_unique<Type>(TypeKind::FunctionPointer);
    
    for (auto& param : func->params) {
        funcType->paramTypes.push_back(TypeSystem::cloneType(param.type.get()));
    }
    
    funcType->returnType = TypeSystem::cloneType(func->returnType.get());
    
    auto symbol = std::make_unique<Symbol>(
        func->name,
        SymbolKind::Function,
        std::move(funcType)
    );
    
    if (!symbolTable.define(func->name, std::move(symbol))) {
        reportError("Function '" + func->name + "' already defined");
    }
}

void TypeChecker::collectStruct(StructAST* structDef) {
    // Store struct definition for later lookup
    structDefinitions[structDef->name] = structDef;
    
    auto structType = std::make_unique<Type>(TypeKind::Struct);
    structType->structName = structDef->name;
    
    auto symbol = std::make_unique<Symbol>(
        structDef->name,
        SymbolKind::Struct,
        std::move(structType)
    );
    
    if (!symbolTable.define(structDef->name, std::move(symbol))) {
        reportError("Struct '" + structDef->name + "' already defined");
    }
}

void TypeChecker::collectConstant(ConstantAST* constant) {
    auto symbol = std::make_unique<Symbol>(
        constant->name,
        SymbolKind::Constant,
        TypeSystem::cloneType(constant->type.get()),
        false
    );
    
    if (!symbolTable.define(constant->name, std::move(symbol))) {
        reportError("Constant '" + constant->name + "' already defined");
    }
}

void TypeChecker::checkFunction(FunctionAST* func) {
    currentFunction = func;
    
    // Enter function scope
    symbolTable.enterScope();
    
    // Add parameters to scope
    for (auto& param : func->params) {
        auto symbol = std::make_unique<Symbol>(
            param.name,
            SymbolKind::Parameter,
            TypeSystem::cloneType(param.type.get()),
            true // Parameters are mutable
        );
        
        if (!symbolTable.define(param.name, std::move(symbol))) {
            reportError("Parameter '" + param.name + "' already defined");
        }
    }
    
    // Check function body
    for (auto& stmt : func->body) {
        checkStatement(stmt.get());
    }
    
    // Verify all paths return (unless it's a naked function or return type is void/u64 with value 0)
    // Naked functions use inline assembly to control execution flow
    if (!func->isNaked && (func->returnType->kind != TypeKind::U64 || !func->body.empty())) {
        if (!verifyAllPathsReturn(func->body)) {
            reportError("Not all code paths return a value in function '" + func->name + "'");
        }
    }
    
    symbolTable.exitScope();
    currentFunction = nullptr;
}

void TypeChecker::checkStatement(StmtAST* stmt) {
    if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt)) {
        checkVarDecl(varDecl);
    } else if (auto* assignment = dynamic_cast<AssignmentStmt*>(stmt)) {
        checkAssignment(assignment);
    } else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        checkIf(ifStmt);
    } else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        checkWhile(whileStmt);
    } else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        checkReturn(returnStmt);
    } else if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        checkExprStmt(exprStmt);
    } else if (auto* asmStmt = dynamic_cast<AsmStmt*>(stmt)) {
        checkAsmStmt(asmStmt);
    } else if (auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(stmt)) {
        checkUnsafeBlock(unsafeBlock);
    }
}

void TypeChecker::checkVarDecl(VarDeclStmt* stmt) {
    // Check if variable already exists in current scope
    if (symbolTable.existsInCurrentScope(stmt->name)) {
        reportError("Variable '" + stmt->name + "' already defined in this scope");
        return;
    }
    
    // Check initializer type if present
    if (stmt->initializer) {
        Type* initType = checkExpression(stmt->initializer.get());
        if (initType && !TypeSystem::areTypesCompatible(stmt->type.get(), initType)) {
            reportError("Type mismatch in variable initialization: expected " +
                       TypeSystem::typeToString(stmt->type.get()) + ", got " +
                       TypeSystem::typeToString(initType));
        }
    }
    
    // Add variable to symbol table
    auto symbol = std::make_unique<Symbol>(
        stmt->name,
        SymbolKind::Variable,
        TypeSystem::cloneType(stmt->type.get()),
        stmt->isMutable
    );
    
    symbolTable.define(stmt->name, std::move(symbol));
}

void TypeChecker::checkAssignment(AssignmentStmt* stmt) {
    // Check target is an lvalue
    Type* targetType = checkExpression(stmt->target.get());
    Type* valueType = checkExpression(stmt->value.get());
    
    if (!targetType || !valueType) {
        return;
    }
    
    // Check if target is mutable
    if (auto* ident = dynamic_cast<IdentifierExpr*>(stmt->target.get())) {
        Symbol* sym = symbolTable.lookup(ident->name);
        if (sym && !sym->isMutable) {
            reportError("Cannot assign to immutable variable '" + ident->name + "'");
        }
    }
    
    // Check type compatibility
    if (!TypeSystem::areTypesCompatible(targetType, valueType)) {
        reportError("Type mismatch in assignment: expected " +
                   TypeSystem::typeToString(targetType) + ", got " +
                   TypeSystem::typeToString(valueType));
    }
}

void TypeChecker::checkIf(IfStmt* stmt) {
    // Check condition
    Type* condType = checkExpression(stmt->condition.get());
    if (condType && !TypeSystem::isNumericType(condType)) {
        reportError("If condition must be numeric type");
    }
    
    // Check then block
    symbolTable.enterScope();
    for (auto& s : stmt->thenBlock) {
        checkStatement(s.get());
    }
    symbolTable.exitScope();
    
    // Check else block
    if (!stmt->elseBlock.empty()) {
        symbolTable.enterScope();
        for (auto& s : stmt->elseBlock) {
            checkStatement(s.get());
        }
        symbolTable.exitScope();
    }
}

void TypeChecker::checkWhile(WhileStmt* stmt) {
    // Check condition
    Type* condType = checkExpression(stmt->condition.get());
    if (condType && !TypeSystem::isNumericType(condType)) {
        reportError("While condition must be numeric type");
    }
    
    // Check body
    symbolTable.enterScope();
    for (auto& s : stmt->body) {
        checkStatement(s.get());
    }
    symbolTable.exitScope();
}

void TypeChecker::checkReturn(ReturnStmt* stmt) {
    if (!currentFunction) {
        reportError("Return statement outside of function");
        return;
    }
    
    if (stmt->value) {
        Type* returnType = checkExpression(stmt->value.get());
        if (returnType && !TypeSystem::areTypesCompatible(currentFunction->returnType.get(), returnType)) {
            reportError("Return type mismatch: expected " +
                       TypeSystem::typeToString(currentFunction->returnType.get()) + ", got " +
                       TypeSystem::typeToString(returnType));
        }
    }
}

void TypeChecker::checkExprStmt(ExprStmt* stmt) {
    checkExpression(stmt->expr.get());
}

void TypeChecker::checkAsmStmt(AsmStmt* stmt) {
    if (!inUnsafeBlock && !currentFunction->isNaked) {
        reportError("Inline assembly must be in unsafe block or naked function");
    }
}

void TypeChecker::checkUnsafeBlock(UnsafeBlockStmt* stmt) {
    bool wasInUnsafe = inUnsafeBlock;
    inUnsafeBlock = true;
    
    symbolTable.enterScope();
    for (auto& s : stmt->body) {
        checkStatement(s.get());
    }
    symbolTable.exitScope();
    
    inUnsafeBlock = wasInUnsafe;
}

// Expression type checking implementation

Type* TypeChecker::checkExpression(ExprAST* expr) {
    if (auto* intLit = dynamic_cast<IntLiteralExpr*>(expr)) {
        return checkIntLiteral(intLit);
    } else if (auto* strLit = dynamic_cast<StringLiteralExpr*>(expr)) {
        return checkStringLiteral(strLit);
    } else if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        return checkIdentifier(ident);
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        return checkBinary(binary);
    } else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        return checkUnary(unary);
    } else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        return checkCall(call);
    } else if (auto* syscall = dynamic_cast<SyscallExpr*>(expr)) {
        return checkSyscall(syscall);
    } else if (auto* encBlock = dynamic_cast<EncBlockExpr*>(expr)) {
        return checkEncBlock(encBlock);
    } else if (auto* cast = dynamic_cast<CastExpr*>(expr)) {
        return checkCast(cast);
    } else if (auto* arrayIndex = dynamic_cast<ArrayIndexExpr*>(expr)) {
        return checkArrayIndex(arrayIndex);
    } else if (auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(expr)) {
        return checkFieldAccess(fieldAccess);
    } else if (auto* structInit = dynamic_cast<StructInitExpr*>(expr)) {
        return checkStructInit(structInit);
    }
    
    return nullptr;
}

Type* TypeChecker::checkIntLiteral(IntLiteralExpr* expr) {
    // Integer literals are u64 by default
    return Type::makeU64().release();
}

Type* TypeChecker::checkStringLiteral(StringLiteralExpr* expr) {
    // String literals are pointers
    return Type::makePtr().release();
}

Type* TypeChecker::checkIdentifier(IdentifierExpr* expr) {
    Symbol* sym = symbolTable.lookup(expr->name);
    if (!sym) {
        reportError("Undefined identifier '" + expr->name + "'");
        return nullptr;
    }
    
    return TypeSystem::cloneType(sym->type.get()).release();
}

Type* TypeChecker::checkBinary(BinaryExpr* expr) {
    Type* leftType = checkExpression(expr->left.get());
    Type* rightType = checkExpression(expr->right.get());
    
    if (!leftType || !rightType) {
        return nullptr;
    }
    
    auto resultType = TypeSystem::getBinaryOpResultType(leftType, rightType, expr->op);
    if (!resultType) {
        reportError("Invalid operand types for binary operator: " +
                   TypeSystem::typeToString(leftType) + " and " +
                   TypeSystem::typeToString(rightType));
        return nullptr;
    }
    
    return resultType.release();
}

Type* TypeChecker::checkUnary(UnaryExpr* expr) {
    Type* operandType = checkExpression(expr->operand.get());
    
    if (!operandType) {
        return nullptr;
    }
    
    // Check if operation requires unsafe block
    if ((expr->op == UnaryOp::Deref || expr->op == UnaryOp::AddressOf) && !inUnsafeBlock) {
        reportError("Pointer operations must be in unsafe block");
    }
    
    auto resultType = TypeSystem::getUnaryOpResultType(operandType, expr->op);
    if (!resultType) {
        reportError("Invalid operand type for unary operator: " +
                   TypeSystem::typeToString(operandType));
        return nullptr;
    }
    
    return resultType.release();
}

Type* TypeChecker::checkCall(CallExpr* expr) {
    // Get callee type
    Type* calleeType = checkExpression(expr->callee.get());
    if (!calleeType) {
        return nullptr;
    }
    
    // Check if callee is a function
    if (calleeType->kind != TypeKind::FunctionPointer) {
        reportError("Expression is not callable");
        return nullptr;
    }
    
    // Check argument count
    if (expr->args.size() != calleeType->paramTypes.size()) {
        reportError("Function call argument count mismatch: expected " +
                   std::to_string(calleeType->paramTypes.size()) + ", got " +
                   std::to_string(expr->args.size()));
        return nullptr;
    }
    
    // Check argument types
    for (size_t i = 0; i < expr->args.size(); i++) {
        Type* argType = checkExpression(expr->args[i].get());
        if (argType && !TypeSystem::areTypesCompatible(calleeType->paramTypes[i].get(), argType)) {
            reportError("Argument " + std::to_string(i + 1) + " type mismatch: expected " +
                       TypeSystem::typeToString(calleeType->paramTypes[i].get()) + ", got " +
                       TypeSystem::typeToString(argType));
        }
    }
    
    return TypeSystem::cloneType(calleeType->returnType.get()).release();
}

Type* TypeChecker::checkSyscall(SyscallExpr* expr) {
    if (!inUnsafeBlock) {
        reportError("Syscall must be in unsafe block");
    }
    
    // Check all arguments
    for (auto& arg : expr->args) {
        checkExpression(arg.get());
    }
    
    // Syscalls return u64
    return Type::makeU64().release();
}

Type* TypeChecker::checkEncBlock(EncBlockExpr* expr) {
    // Encrypted blocks return pointers
    return Type::makePtr().release();
}

Type* TypeChecker::checkCast(CastExpr* expr) {
    Type* exprType = checkExpression(expr->expr.get());
    
    if (!exprType) {
        return nullptr;
    }
    
    if (!TypeSystem::canCast(exprType, expr->targetType.get())) {
        reportError("Invalid cast from " + TypeSystem::typeToString(exprType) +
                   " to " + TypeSystem::typeToString(expr->targetType.get()));
        return nullptr;
    }
    
    return TypeSystem::cloneType(expr->targetType.get()).release();
}

Type* TypeChecker::checkArrayIndex(ArrayIndexExpr* expr) {
    Type* arrayType = checkExpression(expr->array.get());
    Type* indexType = checkExpression(expr->index.get());
    
    if (!arrayType || !indexType) {
        return nullptr;
    }
    
    if (!inUnsafeBlock) {
        reportError("Array indexing must be in unsafe block");
    }
    
    if (!TypeSystem::isNumericType(indexType)) {
        reportError("Array index must be numeric type");
    }
    
    // Return element type if array, otherwise u64
    if (arrayType->kind == TypeKind::Array && arrayType->elementType) {
        return TypeSystem::cloneType(arrayType->elementType.get()).release();
    }
    
    // For pointer indexing, return pointee type if available
    if (arrayType->kind == TypeKind::Ptr && arrayType->pointeeType) {
        return TypeSystem::cloneType(arrayType->pointeeType.get()).release();
    }
    
    // Default to u64 if type information not available
    return Type::makeU64().release();
}

Type* TypeChecker::checkFieldAccess(FieldAccessExpr* expr) {
    Type* objectType = checkExpression(expr->object.get());
    
    if (!objectType) {
        return nullptr;
    }
    
    if (objectType->kind != TypeKind::Struct) {
        reportError("Field access on non-struct type");
        return nullptr;
    }
    
    // Look up struct definition
    auto it = structDefinitions.find(objectType->structName);
    if (it == structDefinitions.end()) {
        reportError("Struct definition not found for '" + objectType->structName + "'");
        return nullptr;
    }
    
    StructAST* structDef = it->second;
    
    // Find the field
    for (const auto& field : structDef->fields) {
        if (field.name == expr->fieldName) {
            return TypeSystem::cloneType(field.type.get()).release();
        }
    }
    
    reportError("Struct '" + objectType->structName + "' has no field named '" + expr->fieldName + "'");
    return nullptr;
}

Type* TypeChecker::checkStructInit(StructInitExpr* expr) {
    // Look up struct definition
    Symbol* structSym = symbolTable.lookup(expr->structName);
    if (!structSym || structSym->kind != SymbolKind::Struct) {
        reportError("Undefined struct '" + expr->structName + "'");
        return nullptr;
    }
    
    // Check field initializers
    for (auto& field : expr->fields) {
        checkExpression(field.second.get());
    }
    
    return TypeSystem::cloneType(structSym->type.get()).release();
}

bool TypeChecker::verifyAllPathsReturn(const std::vector<std::unique_ptr<StmtAST>>& stmts) {
    if (stmts.empty()) {
        return false;
    }
    
    // Check if last statement returns
    return statementReturns(stmts.back().get());
}

bool TypeChecker::statementReturns(StmtAST* stmt) {
    if (dynamic_cast<ReturnStmt*>(stmt)) {
        return true;
    }
    
    if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        // If statement returns if both branches return
        if (!ifStmt->elseBlock.empty()) {
            return verifyAllPathsReturn(ifStmt->thenBlock) &&
                   verifyAllPathsReturn(ifStmt->elseBlock);
        }
    }
    
    if (auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(stmt)) {
        // Unsafe block returns if its body returns
        return verifyAllPathsReturn(unsafeBlock->body);
    }
    
    return false;
}

} // namespace lwanga
