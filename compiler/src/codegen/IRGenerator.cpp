#include "IRGenerator.h"
#include "Version.h"
#include <llvm/IR/Verifier.h>
#include <llvm/IR/InlineAsm.h>
#include <llvm/IR/DebugInfoMetadata.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/Path.h>
#include <iostream>

namespace lwanga {

IRGenerator::IRGenerator(const std::string& moduleName, const std::string& sourceFile, const std::string& targetTriple) 
    : currentFunction(nullptr), generateDebugInfo(false), 
      sourceFilename(sourceFile), compileUnit(nullptr), debugFile(nullptr) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(moduleName, *context);
    if (!targetTriple.empty()) {
        module->setTargetTriple(targetTriple);
    }
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
}

IRGenerator::~IRGenerator() = default;

void IRGenerator::reportError(const std::string& message) {
    errors.push_back(message);
}

bool IRGenerator::generate(ProgramAST* program) {
    if (!program) {
        reportError("Null program");
        return false;
    }
    
    // Initialize debug info if enabled
    if (generateDebugInfo) {
        initializeDebugInfo();
    }
    
    // First pass: Generate struct declarations
    generateStructDeclarations(program);
    
    // Second pass: Generate function declarations
    generateFunctionDeclarations(program);
    
    // Third pass: Generate constant declarations
    generateConstantDeclarations(program);
    
    // Fourth pass: Generate function bodies
    for (auto& func : program->functions) {
        generateFunction(func.get());
    }
    
    // Fifth pass: Generate _start function wrapper for freestanding executables
    generateStartFunction();
    
    // Finalize debug info if enabled
    if (generateDebugInfo) {
        finalizeDebugInfo();
    }
    
    // Verify the module
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    if (llvm::verifyModule(*module, &errorStream)) {
        reportError("Module verification failed: " + errorStr);
        return false;
    }
    
    return !hasErrors();
}

llvm::Type* IRGenerator::convertType(const Type* type) {
    if (!type) {
        return nullptr;
    }
    
    switch (type->kind) {
        case TypeKind::U8:
            return llvm::Type::getInt8Ty(*context);
        case TypeKind::U16:
            return llvm::Type::getInt16Ty(*context);
        case TypeKind::U32:
            return llvm::Type::getInt32Ty(*context);
        case TypeKind::U64:
            return llvm::Type::getInt64Ty(*context);
        case TypeKind::Ptr:
            // Generic pointer (i8*)
            return llvm::PointerType::get(llvm::Type::getInt8Ty(*context), 0);
        case TypeKind::Array: {
            llvm::Type* elementType = convertType(type->elementType.get());
            if (!elementType) return nullptr;
            return llvm::ArrayType::get(elementType, type->arraySize);
        }
        case TypeKind::Struct:
            // Look up struct type by name
            return llvm::StructType::getTypeByName(*context, type->structName);
        case TypeKind::FunctionPointer: {
            // Convert parameter types
            std::vector<llvm::Type*> paramTypes;
            for (const auto& paramType : type->paramTypes) {
                llvm::Type* llvmType = convertType(paramType.get());
                if (!llvmType) return nullptr;
                paramTypes.push_back(llvmType);
            }
            
            // Convert return type
            llvm::Type* returnType = convertType(type->returnType.get());
            if (!returnType) return nullptr;
            
            // Create function type and return pointer to it
            llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
            return llvm::PointerType::get(funcType, 0);
        }
        case TypeKind::Register:
            // Registers are represented as 64-bit integers in LLVM
            return llvm::Type::getInt64Ty(*context);
    }
    
    return nullptr;
}

void IRGenerator::generateStructDeclarations(ProgramAST* program) {
    // Create opaque struct types first
    for (auto& structDef : program->structs) {
        llvm::StructType::create(*context, structDef->name);
        // Store struct definition for later lookup
        structDefinitions[structDef->name] = structDef.get();
    }
    
    // Then set their bodies
    for (auto& structDef : program->structs) {
        llvm::StructType* structType = llvm::StructType::getTypeByName(*context, structDef->name);
        if (!structType) {
            reportError("Failed to create struct type: " + structDef->name);
            continue;
        }
        
        std::vector<llvm::Type*> fieldTypes;
        for (const auto& field : structDef->fields) {
            llvm::Type* fieldType = convertType(field.type.get());
            if (!fieldType) {
                reportError("Failed to convert field type in struct " + structDef->name);
                continue;
            }
            fieldTypes.push_back(fieldType);
        }
        
        structType->setBody(fieldTypes, structDef->isPacked);
    }
}

void IRGenerator::generateFunctionDeclarations(ProgramAST* program) {
    for (auto& func : program->functions) {
        // Convert parameter types
        std::vector<llvm::Type*> paramTypes;
        for (const auto& param : func->params) {
            llvm::Type* paramType = convertType(param.type.get());
            if (!paramType) {
                reportError("Failed to convert parameter type in function " + func->name);
                continue;
            }
            paramTypes.push_back(paramType);
        }
        
        // Convert return type
        llvm::Type* returnType = convertType(func->returnType.get());
        if (!returnType) {
            reportError("Failed to convert return type in function " + func->name);
            continue;
        }
        
        // Create function type
        llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
        
        // Create function
        llvm::Function* llvmFunc = llvm::Function::Create(
            funcType,
            llvm::Function::ExternalLinkage,
            func->name,
            module.get()
        );
        
        // Set parameter names
        size_t idx = 0;
        for (auto& arg : llvmFunc->args()) {
            arg.setName(func->params[idx++].name);
        }
        
        // Add naked attribute if needed
        if (func->isNaked) {
            llvmFunc->addFnAttr(llvm::Attribute::Naked);
        }
    }
}

void IRGenerator::generateConstantDeclarations(ProgramAST* program) {
    for (auto& constant : program->constants) {
        // Convert constant type
        llvm::Type* constType = convertType(constant->type.get());
        if (!constType) {
            reportError("Failed to convert constant type: " + constant->name);
            continue;
        }
        
        // Evaluate constant expression at compile time
        llvm::Constant* constValue = evaluateConstantExpression(constant->value.get());
        if (!constValue) {
            reportError("Failed to evaluate constant expression: " + constant->name);
            continue;
        }
        
        // Create global constant
        llvm::GlobalVariable* globalConst = new llvm::GlobalVariable(
            *module,
            constType,
            true,  // isConstant
            llvm::GlobalValue::InternalLinkage,
            constValue,
            constant->name
        );
        
        // Store in constants map
        constants[constant->name] = constValue;
    }
}

void IRGenerator::generateStartFunction() {
    // Generate _start function that calls main and exits
    // This is required for freestanding executables
    
    // Check if main function exists
    llvm::Function* mainFunc = module->getFunction("main");
    if (!mainFunc) {
        // No main function, don't generate _start
        return;
    }
    
    // Get target triple to determine platform
    std::string targetTriple = module->getTargetTriple();
    bool isLinux = targetTriple.find("linux") != std::string::npos || targetTriple.empty();
    
    if (!isLinux) {
        // Only generate _start for Linux targets
        return;
    }
    
    // Create _start function: void _start()
    llvm::FunctionType* startFuncType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context),
        false
    );
    
    llvm::Function* startFunc = llvm::Function::Create(
        startFuncType,
        llvm::Function::ExternalLinkage,
        "_start",
        module.get()
    );
    
    // Create entry block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", startFunc);
    builder->SetInsertPoint(entryBB);
    
    // Call main function
    llvm::Value* exitCode = builder->CreateCall(mainFunc, {}, "main_result");
    
    // Generate exit syscall
    // Linux x86_64: syscall 60 (exit) with exit code in rdi
    // Linux ARM64: syscall 93 (exit) with exit code in x0
    
    bool isX86_64 = targetTriple.find("x86_64") != std::string::npos || 
                    targetTriple.find("x86-64") != std::string::npos ||
                    targetTriple.empty(); // Default to x86_64
    bool isARM64 = targetTriple.find("aarch64") != std::string::npos ||
                   targetTriple.find("arm64") != std::string::npos;
    
    std::string asmStr;
    std::string constraints;
    uint64_t exitSyscallNum;
    
    if (isX86_64) {
        exitSyscallNum = 60; // exit syscall number for x86_64
        asmStr = "syscall";
        // Constraints: syscall number in rax, exit code in rdi
        constraints = "{rax},{rdi},~{rcx},~{r11},~{memory}";
    } else if (isARM64) {
        exitSyscallNum = 93; // exit syscall number for ARM64
        asmStr = "svc #0";
        // Constraints: syscall number in x8, exit code in x0
        constraints = "{x8},{x0},~{memory}";
    } else {
        // Fallback: just return (will likely crash, but better than nothing)
        builder->CreateRetVoid();
        return;
    }
    
    // Create inline assembly for exit syscall
    std::vector<llvm::Type*> asmParamTypes = {
        llvm::Type::getInt64Ty(*context),  // syscall number
        llvm::Type::getInt64Ty(*context)   // exit code
    };
    
    llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
        llvm::Type::getVoidTy(*context),
        asmParamTypes,
        false
    );
    
    llvm::InlineAsm* exitAsm = llvm::InlineAsm::get(
        asmFuncType,
        asmStr,
        constraints,
        true,  // hasSideEffects
        false, // isAlignStack
        llvm::InlineAsm::AD_ATT
    );
    
    // Call exit syscall with exit code from main
    llvm::Value* syscallNum = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), exitSyscallNum);
    builder->CreateCall(exitAsm, {syscallNum, exitCode});
    
    // Unreachable after exit syscall
    builder->CreateUnreachable();
}

void IRGenerator::generateFunction(FunctionAST* func) {
    // Get the function
    llvm::Function* llvmFunc = module->getFunction(func->name);
    if (!llvmFunc) {
        reportError("Function not found: " + func->name);
        return;
    }
    
    currentFunction = llvmFunc;
    
    // Create debug info for function if enabled
    llvm::DISubprogram* debugFunc = nullptr;
    if (generateDebugInfo && debugBuilder) {
        // Create function type for debug info
        llvm::SmallVector<llvm::Metadata*, 8> debugParamTypes;
        
        // Return type
        llvm::DIType* returnType = getDebugType(func->returnType.get());
        debugParamTypes.push_back(returnType);
        
        // Parameter types
        for (const auto& param : func->params) {
            llvm::DIType* paramType = getDebugType(param.type.get());
            debugParamTypes.push_back(paramType);
        }
        
        llvm::DISubroutineType* debugFuncType = debugBuilder->createSubroutineType(
            debugBuilder->getOrCreateTypeArray(debugParamTypes)
        );
        
        // Create subprogram using source location from AST
        debugFunc = debugBuilder->createFunction(
            debugFile,                    // Scope
            func->name,                   // Name
            llvm::StringRef(),            // Linkage name
            debugFile,                    // File
            func->loc.line > 0 ? func->loc.line : 1,  // Line number
            debugFuncType,                // Type
            func->loc.line > 0 ? func->loc.line : 1,  // Scope line
            llvm::DINode::FlagPrototyped, // Flags
            llvm::DISubprogram::SPFlagDefinition
        );
        
        llvmFunc->setSubprogram(debugFunc);
        lexicalBlocks.push_back(debugFunc);
        
        // Emit location for function start
        emitLocation(func->loc.line > 0 ? func->loc.line : 1, func->loc.column);
    }
    
    // Create entry basic block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", llvmFunc);
    builder->SetInsertPoint(entryBB);
    
    // Clear named values and types
    namedValues.clear();
    namedTypes.clear();
    pointeeTypes.clear();
    
    // Allocate space for parameters and store them
    unsigned argIdx = 0;
    for (auto& arg : llvmFunc->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            arg.getType(),
            nullptr,
            arg.getName()
        );
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
        namedTypes[std::string(arg.getName())] = arg.getType();
        
        // Track pointee type for pointer parameters
        if (argIdx < func->params.size() && func->params[argIdx].type->kind == TypeKind::Ptr) {
            pointeeTypes[std::string(arg.getName())] = convertType(func->params[argIdx].type->pointeeType.get());
        }
        
        // Create debug info for parameter
        if (generateDebugInfo && debugBuilder && debugFunc && argIdx < func->params.size()) {
            llvm::DILocalVariable* debugParam = debugBuilder->createParameterVariable(
                debugFunc,                                    // Scope
                func->params[argIdx].name,                   // Name
                argIdx + 1,                                  // Arg number (1-based)
                debugFile,                                   // File
                func->loc.line > 0 ? func->loc.line : 1,    // Line
                getDebugType(func->params[argIdx].type.get()) // Type
            );
            
            debugBuilder->insertDeclare(
                alloca,
                debugParam,
                debugBuilder->createExpression(),
                llvm::DILocation::get(*context, func->loc.line > 0 ? func->loc.line : 1, 0, debugFunc),
                builder->GetInsertBlock()
            );
        }
        
        argIdx++;
    }
    
    // Generate function body
    for (auto& stmt : func->body) {
        generateStatement(stmt.get());
    }
    
    // If the last block doesn't have a terminator, add a default return
    if (!builder->GetInsertBlock()->getTerminator()) {
        if (llvmFunc->getReturnType()->isVoidTy()) {
            builder->CreateRetVoid();
        } else {
            // Return zero as default
            builder->CreateRet(llvm::ConstantInt::get(llvmFunc->getReturnType(), 0));
        }
    }
    
    // Pop debug scope
    if (generateDebugInfo && !lexicalBlocks.empty()) {
        lexicalBlocks.pop_back();
    }
    
    // Verify the function
    std::string errorStr;
    llvm::raw_string_ostream errorStream(errorStr);
    if (llvm::verifyFunction(*llvmFunc, &errorStream)) {
        reportError("Function verification failed for " + func->name + ": " + errorStr);
    }
    
    currentFunction = nullptr;
}

void IRGenerator::enterScope() {
    scopeStack.push_back(namedValues);
}

void IRGenerator::exitScope() {
    if (!scopeStack.empty()) {
        namedValues = scopeStack.back();
        scopeStack.pop_back();
    }
}

void IRGenerator::generateStatement(StmtAST* stmt) {
    // If the current block already has a terminator, don't generate more code
    if (builder->GetInsertBlock()->getTerminator()) {
        return;
    }
    
    if (auto* varDecl = dynamic_cast<VarDeclStmt*>(stmt)) {
        generateVarDecl(varDecl);
    } else if (auto* assignment = dynamic_cast<AssignmentStmt*>(stmt)) {
        generateAssignment(assignment);
    } else if (auto* ifStmt = dynamic_cast<IfStmt*>(stmt)) {
        generateIf(ifStmt);
    } else if (auto* whileStmt = dynamic_cast<WhileStmt*>(stmt)) {
        generateWhile(whileStmt);
    } else if (auto* returnStmt = dynamic_cast<ReturnStmt*>(stmt)) {
        generateReturn(returnStmt);
    } else if (auto* exprStmt = dynamic_cast<ExprStmt*>(stmt)) {
        generateExprStmt(exprStmt);
    } else if (auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(stmt)) {
        generateUnsafeBlock(unsafeBlock);
    } else if (auto* asmStmt = dynamic_cast<AsmStmt*>(stmt)) {
        generateAsm(asmStmt);
    }
}

void IRGenerator::generateVarDecl(VarDeclStmt* stmt) {
    // Check if this is a register variable
    if (stmt->type->kind == TypeKind::Register) {
        // For register variables, the variable name IS the register name
        // Store the register name in a map for later use
        // We don't allocate stack space for registers
        
        // Store register info
        llvm::Type* regType = llvm::Type::getInt64Ty(*context);
        namedTypes[stmt->name] = regType;
        
        // Mark this as a register variable by storing nullptr in namedValues
        // We'll handle reads/writes specially in generateIdentifier and generateAssignment
        namedValues[stmt->name] = nullptr; // nullptr indicates register variable
        
        // Store the register name in the type for later use
        stmt->type->registerName = stmt->name;
        
        // If there's an initializer, we need to write to the register
        if (stmt->initializer) {
            llvm::Value* initValue = generateExpression(stmt->initializer.get());
            if (initValue) {
                // Generate inline assembly to write to the register
                std::string asmStr = "mov $0, %" + stmt->name;
                llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
                    llvm::Type::getVoidTy(*context),
                    {llvm::Type::getInt64Ty(*context)},
                    false
                );
                llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
                    asmFuncType,
                    asmStr,
                    "r,~{" + stmt->name + "}",  // input constraint and clobber
                    true  // has side effects
                );
                builder->CreateCall(inlineAsm, {initValue});
            }
        }
        return;
    }
    
    llvm::Type* varType = convertType(stmt->type.get());
    if (!varType) {
        reportError("Failed to convert variable type: " + stmt->name);
        return;
    }
    
    // Allocate space on the stack
    llvm::AllocaInst* alloca = builder->CreateAlloca(varType, nullptr, stmt->name);
    
    // Store initializer if present
    if (stmt->initializer) {
        llvm::Value* initValue = generateExpression(stmt->initializer.get());
        if (initValue) {
            builder->CreateStore(initValue, alloca);
        }
    }
    
    // Add to named values and types
    namedValues[stmt->name] = alloca;
    namedTypes[stmt->name] = varType;
    
    // Track pointee type for pointer variables
    if (stmt->type->kind == TypeKind::Ptr && stmt->type->pointeeType) {
        pointeeTypes[stmt->name] = convertType(stmt->type->pointeeType.get());
    }
}

void IRGenerator::generateAssignment(AssignmentStmt* stmt) {
    // Check if target is a register variable
    if (auto* ident = dynamic_cast<IdentifierExpr*>(stmt->target.get())) {
        auto valueIt = namedValues.find(ident->name);
        if (valueIt != namedValues.end() && valueIt->second == nullptr) {
            // This is a register variable - write to it using inline assembly
            llvm::Value* value = generateExpression(stmt->value.get());
            if (!value) {
                return;
            }
            
            std::string asmStr = "mov $0, %" + ident->name;
            llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
                llvm::Type::getVoidTy(*context),
                {llvm::Type::getInt64Ty(*context)},
                false
            );
            llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
                asmFuncType,
                asmStr,
                "r,~{" + ident->name + "}",  // input constraint and clobber
                true  // has side effects
            );
            builder->CreateCall(inlineAsm, {value});
            return;
        }
    }
    
    // Get the target address using the lvalue address generator
    llvm::Value* target = generateLValueAddress(stmt->target.get());
    if (!target) {
        return;
    }
    
    // Generate value
    llvm::Value* value = generateExpression(stmt->value.get());
    if (!value) {
        return;
    }
    
    // Store value
    builder->CreateStore(value, target);
}

void IRGenerator::generateIf(IfStmt* stmt) {
    // Generate condition
    llvm::Value* condValue = generateExpression(stmt->condition.get());
    if (!condValue) {
        return;
    }
    
    // Convert condition to boolean (i1)
    condValue = builder->CreateICmpNE(
        condValue,
        llvm::ConstantInt::get(condValue->getType(), 0),
        "ifcond"
    );
    
    // Create basic blocks
    llvm::BasicBlock* thenBB = llvm::BasicBlock::Create(*context, "then", currentFunction);
    llvm::BasicBlock* elseBB = stmt->elseBlock.empty() ? nullptr : 
                                llvm::BasicBlock::Create(*context, "else", currentFunction);
    llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(*context, "ifcont", currentFunction);
    
    // Branch
    if (elseBB) {
        builder->CreateCondBr(condValue, thenBB, elseBB);
    } else {
        builder->CreateCondBr(condValue, thenBB, mergeBB);
    }
    
    // Generate then block
    builder->SetInsertPoint(thenBB);
    enterScope();
    for (auto& s : stmt->thenBlock) {
        generateStatement(s.get());
    }
    exitScope();
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(mergeBB);
    }
    
    // Generate else block if present
    if (elseBB) {
        builder->SetInsertPoint(elseBB);
        enterScope();
        for (auto& s : stmt->elseBlock) {
            generateStatement(s.get());
        }
        exitScope();
        if (!builder->GetInsertBlock()->getTerminator()) {
            builder->CreateBr(mergeBB);
        }
    }
    
    // Continue with merge block
    builder->SetInsertPoint(mergeBB);
}

void IRGenerator::generateWhile(WhileStmt* stmt) {
    // Create basic blocks
    llvm::BasicBlock* condBB = llvm::BasicBlock::Create(*context, "whilecond", currentFunction);
    llvm::BasicBlock* bodyBB = llvm::BasicBlock::Create(*context, "whilebody", currentFunction);
    llvm::BasicBlock* afterBB = llvm::BasicBlock::Create(*context, "afterwhile", currentFunction);
    
    // Branch to condition
    builder->CreateBr(condBB);
    
    // Generate condition
    builder->SetInsertPoint(condBB);
    llvm::Value* condValue = generateExpression(stmt->condition.get());
    if (!condValue) {
        return;
    }
    
    // Convert condition to boolean
    condValue = builder->CreateICmpNE(
        condValue,
        llvm::ConstantInt::get(condValue->getType(), 0),
        "whilecond"
    );
    
    builder->CreateCondBr(condValue, bodyBB, afterBB);
    
    // Generate body
    builder->SetInsertPoint(bodyBB);
    enterScope();
    for (auto& s : stmt->body) {
        generateStatement(s.get());
    }
    exitScope();
    if (!builder->GetInsertBlock()->getTerminator()) {
        builder->CreateBr(condBB);
    }
    
    // Continue after loop
    builder->SetInsertPoint(afterBB);
}

void IRGenerator::generateReturn(ReturnStmt* stmt) {
    if (stmt->value) {
        llvm::Value* retValue = generateExpression(stmt->value.get());
        if (retValue) {
            builder->CreateRet(retValue);
        }
    } else {
        builder->CreateRetVoid();
    }
}

void IRGenerator::generateExprStmt(ExprStmt* stmt) {
    generateExpression(stmt->expr.get());
}

void IRGenerator::generateUnsafeBlock(UnsafeBlockStmt* stmt) {
    // Unsafe blocks are just scoped statement lists
    // They don't generate any special IR - the "unsafe" is a compile-time check
    enterScope();
    for (auto& s : stmt->body) {
        generateStatement(s.get());
    }
    exitScope();
}

void IRGenerator::generateAsm(AsmStmt* stmt) {
    // Generate inline assembly with full register access and constraint support
    // 
    // Supports two modes:
    // 1. Basic asm: just assembly code (backward compatible)
    // 2. Extended asm: assembly code with input/output operands and clobbers
    
    // Get target triple to determine platform
    std::string targetTriple = module->getTargetTriple();
    bool isX86_64 = targetTriple.find("x86_64") != std::string::npos || 
                    targetTriple.find("x86-64") != std::string::npos ||
                    targetTriple.empty(); // Default to x86_64
    bool isARM64 = targetTriple.find("aarch64") != std::string::npos ||
                   targetTriple.find("arm64") != std::string::npos;
    
    // Use Intel syntax for inline assembly (no % or $ prefixes needed)
    llvm::InlineAsm::AsmDialect dialect = llvm::InlineAsm::AD_Intel;
    
    // Check if this is extended asm (has operands or clobbers)
    bool isExtendedAsm = !stmt->outputs.empty() || !stmt->inputs.empty() || !stmt->clobbers.empty();
    
    if (!isExtendedAsm) {
        // Basic asm: no inputs/outputs, just side effects
        llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
            llvm::Type::getVoidTy(*context),
            false
        );
        
        llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
            asmFuncType,
            stmt->asmCode,
            "",  // empty constraints
            true,  // hasSideEffects
            false, // isAlignStack
            dialect
        );
        
        builder->CreateCall(inlineAsm);
        return;
    }
    
    // Extended asm: build constraint string and handle operands
    std::string constraints;
    std::vector<llvm::Value*> asmArgs;
    std::vector<llvm::Type*> asmArgTypes;
    std::vector<llvm::Value*> outputAddrs;
    std::vector<llvm::Type*> outputTypes;
    
    // Process output operands
    for (size_t i = 0; i < stmt->outputs.size(); i++) {
        const auto& output = stmt->outputs[i];
        
        // Get the lvalue address for output
        llvm::Value* addr = generateLValueAddress(output.expr.get());
        if (!addr) {
            reportError("Invalid output operand in asm statement");
            return;
        }
        
        // Add constraint (outputs come first)
        if (!constraints.empty()) {
            constraints += ",";
        }
        constraints += output.constraint;
        
        // Track output addresses and types for later storage
        outputAddrs.push_back(addr);
        
        // Determine type of the output variable
        llvm::Type* varType = llvm::Type::getInt64Ty(*context); // Fallback
        if (auto* alloca = llvm::dyn_cast<llvm::AllocaInst>(addr)) {
            varType = alloca->getAllocatedType();
        } else if (addr->getType()->isPointerTy()) {
            // Check if we can deduce type from namedTypes
            if (auto* ident = dynamic_cast<IdentifierExpr*>(output.expr.get())) {
                if (namedTypes.count(ident->name)) varType = namedTypes[ident->name];
            }
        }
        outputTypes.push_back(varType);
        
        // If it's an indirect (memory) output, it needs an argument
        if (output.constraint.find("*m") != std::string::npos) {
            asmArgs.push_back(addr);
            asmArgTypes.push_back(addr->getType());
        }
    }
    
    // Process input operands
    for (size_t i = 0; i < stmt->inputs.size(); i++) {
        const auto& input = stmt->inputs[i];
        
        // Generate the input value
        llvm::Value* value = generateExpression(input.expr.get());
        if (!value) {
            reportError("Invalid input operand in asm statement");
            return;
        }
        
        // Add constraint
        if (!constraints.empty()) {
            constraints += ",";
        }
        constraints += input.constraint;
        
        asmArgs.push_back(value);
        asmArgTypes.push_back(value->getType());
    }
    
    // Add clobbers to constraints
    for (const auto& clobber : stmt->clobbers) {
        if (!constraints.empty()) {
            constraints += ",";
        }
        constraints += "~{" + clobber + "}";
    }
    
    // Determine return type based on direct outputs
    llvm::Type* returnType;
    std::vector<llvm::Type*> directOutputTypes;
    for (size_t i = 0; i < stmt->outputs.size(); i++) {
        if (stmt->outputs[i].constraint.find("*m") == std::string::npos) {
            directOutputTypes.push_back(outputTypes[i]);
        }
    }
    
    if (directOutputTypes.empty()) {
        returnType = llvm::Type::getVoidTy(*context);
    } else if (directOutputTypes.size() == 1) {
        returnType = directOutputTypes[0];
    } else {
        returnType = llvm::StructType::get(*context, directOutputTypes);
    }
    
    // Create function type for inline assembly
    llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
        returnType,
        asmArgTypes,
        false
    );
    
    // Create inline assembly
    llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
        asmFuncType,
        stmt->asmCode,
        constraints,
        true,  // hasSideEffects
        false, // isAlignStack
        dialect
    );
    
    // Call the inline assembly
    llvm::Value* result = builder->CreateCall(inlineAsm, asmArgs);
    
    // Store results back to output operands
    if (!directOutputTypes.empty()) {
        if (directOutputTypes.size() == 1) {
            // Find which output was direct
            for (size_t i = 0; i < stmt->outputs.size(); i++) {
                if (stmt->outputs[i].constraint.find("*m") == std::string::npos) {
                    builder->CreateStore(result, outputAddrs[i]);
                    break;
                }
            }
        } else {
            // Multiple outputs: extract from struct
            size_t directIdx = 0;
            for (size_t i = 0; i < stmt->outputs.size(); i++) {
                if (stmt->outputs[i].constraint.find("*m") == std::string::npos) {
                    llvm::Value* extracted = builder->CreateExtractValue(result, directIdx++);
                    builder->CreateStore(extracted, outputAddrs[i]);
                }
            }
        }
    }
}

llvm::Value* IRGenerator::generateExpression(ExprAST* expr) {
    if (auto* intLit = dynamic_cast<IntLiteralExpr*>(expr)) {
        return generateIntLiteral(intLit);
    } else if (auto* strLit = dynamic_cast<StringLiteralExpr*>(expr)) {
        return generateStringLiteral(strLit);
    } else if (auto* arrayLit = dynamic_cast<ArrayLiteralExpr*>(expr)) {
        return generateArrayLiteral(arrayLit);
    } else if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        return generateIdentifier(ident);
    } else if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        return generateBinary(binary);
    } else if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        return generateUnary(unary);
    } else if (auto* call = dynamic_cast<CallExpr*>(expr)) {
        return generateCall(call);
    } else if (auto* cast = dynamic_cast<CastExpr*>(expr)) {
        return generateCast(cast);
    } else if (auto* structInit = dynamic_cast<StructInitExpr*>(expr)) {
        return generateStructInit(structInit);
    } else if (auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(expr)) {
        return generateFieldAccess(fieldAccess);
    } else if (auto* arrayIndex = dynamic_cast<ArrayIndexExpr*>(expr)) {
        return generateArrayIndex(arrayIndex);
    } else if (auto* syscall = dynamic_cast<SyscallExpr*>(expr)) {
        return generateSyscall(syscall);
    } else if (auto* encBlock = dynamic_cast<EncBlockExpr*>(expr)) {
        return generateEncBlock(encBlock);
    }
    
    return nullptr;
}

llvm::Value* IRGenerator::generateIntLiteral(IntLiteralExpr* expr) {
    return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), expr->value);
}

llvm::Value* IRGenerator::generateStringLiteral(StringLiteralExpr* expr) {
    // Create global string constant
    llvm::Constant* strConstant = llvm::ConstantDataArray::getString(*context, expr->value);
    llvm::GlobalVariable* strGlobal = new llvm::GlobalVariable(
        *module,
        strConstant->getType(),
        true,
        llvm::GlobalValue::PrivateLinkage,
        strConstant,
        ".str"
    );
    
    // Return pointer to first element
    return builder->CreatePointerCast(
        strGlobal,
        llvm::PointerType::get(llvm::Type::getInt8Ty(*context), 0)
    );
}

llvm::Value* IRGenerator::generateArrayLiteral(ArrayLiteralExpr* expr) {
    // Generate values for all elements
    std::vector<llvm::Constant*> elementValues;
    llvm::Type* elementType = nullptr;
    
    for (auto& elem : expr->elements) {
        llvm::Value* val = generateExpression(elem.get());
        if (!val) return nullptr;
        
        // All elements must be constants for array literals
        llvm::Constant* constVal = llvm::dyn_cast<llvm::Constant>(val);
        if (!constVal) {
            // For non-constant initializers, we need to allocate and initialize at runtime
            // This is handled in variable declaration
            return nullptr;
        }
        
        if (!elementType) {
            elementType = constVal->getType();
        }
        
        elementValues.push_back(constVal);
    }
    
    if (elementValues.empty()) {
        return nullptr;
    }
    
    // Create array type and constant
    llvm::ArrayType* arrayType = llvm::ArrayType::get(elementType, elementValues.size());
    llvm::Constant* arrayConstant = llvm::ConstantArray::get(arrayType, elementValues);
    
    return arrayConstant;
}

llvm::Value* IRGenerator::generateIdentifier(IdentifierExpr* expr) {
    // Check if it's a constant first
    auto constIt = constants.find(expr->name);
    if (constIt != constants.end()) {
        return constIt->second;
    }
    
    // Check if it's a function name (for function pointer assignments)
    llvm::Function* func = module->getFunction(expr->name);
    if (func) {
        // Return the function as a value (function pointer)
        return func;
    }
    
    // Otherwise it's a variable
    llvm::Value* value = namedValues[expr->name];
    if (value == nullptr) {
        // Check if this is a register variable (nullptr indicates register)
        auto typeIt = namedTypes.find(expr->name);
        if (typeIt != namedTypes.end()) {
            // This is a register variable - read from it using inline assembly
            std::string asmStr = "mov %" + expr->name + ", $0";
            llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
                llvm::Type::getInt64Ty(*context),
                {},
                false
            );
            llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
                asmFuncType,
                asmStr,
                "=r,~{" + expr->name + "}",  // output constraint and clobber
                true  // has side effects
            );
            return builder->CreateCall(inlineAsm, {});
        }
        
        reportError("Undefined variable: " + expr->name);
        return nullptr;
    }
    
    // Get the type we allocated
    llvm::Type* varType = namedTypes[expr->name];
    if (!varType) {
        reportError("Type not found for variable: " + expr->name);
        return nullptr;
    }
    
    // Load the value
    return builder->CreateLoad(varType, value, expr->name);
}

llvm::Value* IRGenerator::generateBinary(BinaryExpr* expr) {
    llvm::Value* left = generateExpression(expr->left.get());
    llvm::Value* right = generateExpression(expr->right.get());
    
    if (!left || !right) {
        return nullptr;
    }
    
    // Handle pointer arithmetic
    if (expr->op == BinaryOp::Add && left->getType()->isPointerTy()) {
        // Pointer + integer: use GEP (GetElementPtr)
        return builder->CreateGEP(
            llvm::Type::getInt8Ty(*context),
            left,
            right,
            "ptradd"
        );
    }
    
    if (expr->op == BinaryOp::Sub && left->getType()->isPointerTy()) {
        if (right->getType()->isPointerTy()) {
            // Pointer - pointer: calculate byte difference
            llvm::Value* leftInt = builder->CreatePtrToInt(left, llvm::Type::getInt64Ty(*context));
            llvm::Value* rightInt = builder->CreatePtrToInt(right, llvm::Type::getInt64Ty(*context));
            return builder->CreateSub(leftInt, rightInt, "ptrdiff");
        } else {
            // Pointer - integer: use negative GEP
            llvm::Value* negRight = builder->CreateNeg(right, "negoffset");
            return builder->CreateGEP(
                llvm::Type::getInt8Ty(*context),
                left,
                negRight,
                "ptrsub"
            );
        }
    }
    
    switch (expr->op) {
        case BinaryOp::Add:
            return builder->CreateAdd(left, right, "addtmp");
        case BinaryOp::Sub:
            return builder->CreateSub(left, right, "subtmp");
        case BinaryOp::Mul:
            return builder->CreateMul(left, right, "multmp");
        case BinaryOp::Div:
            return builder->CreateUDiv(left, right, "divtmp");
        case BinaryOp::Mod:
            return builder->CreateURem(left, right, "modtmp");
        case BinaryOp::BitAnd:
            return builder->CreateAnd(left, right, "andtmp");
        case BinaryOp::BitOr:
            return builder->CreateOr(left, right, "ortmp");
        case BinaryOp::BitXor:
            return builder->CreateXor(left, right, "xortmp");
        case BinaryOp::LeftShift:
            return builder->CreateShl(left, right, "shltmp");
        case BinaryOp::RightShift:
            return builder->CreateLShr(left, right, "shrtmp");
        case BinaryOp::Equal:
            return builder->CreateICmpEQ(left, right, "eqtmp");
        case BinaryOp::NotEqual:
            return builder->CreateICmpNE(left, right, "netmp");
        case BinaryOp::Less:
            return builder->CreateICmpULT(left, right, "lttmp");
        case BinaryOp::Greater:
            return builder->CreateICmpUGT(left, right, "gttmp");
        case BinaryOp::LessEqual:
            return builder->CreateICmpULE(left, right, "letmp");
        case BinaryOp::GreaterEqual:
            return builder->CreateICmpUGE(left, right, "getmp");
    }
    
    return nullptr;
}

llvm::Value* IRGenerator::generateUnary(UnaryExpr* expr) {
    switch (expr->op) {
        case UnaryOp::Negate:
        case UnaryOp::Not: {
            llvm::Value* operand = generateExpression(expr->operand.get());
            if (!operand) {
                return nullptr;
            }
            
            if (expr->op == UnaryOp::Negate) {
                return builder->CreateNeg(operand, "negtmp");
            } else {
                return builder->CreateNot(operand, "nottmp");
            }
        }
        
        case UnaryOp::Deref: {
            llvm::Value* operand = generateExpression(expr->operand.get());
            if (!operand) {
                return nullptr;
            }
            
            // Dereference pointer - load from address
            // For generic pointers, load as u64
            return builder->CreateLoad(
                llvm::Type::getInt64Ty(*context),
                operand,
                "dereftmp"
            );
        }
        
        case UnaryOp::AddressOf: {
            // Address-of operator: get the address of an lvalue
            return generateLValueAddress(expr->operand.get());
        }
    }
    
    return nullptr;
}

llvm::Value* IRGenerator::generateCall(CallExpr* expr) {
    // Generate the callee expression (could be identifier or any expression)
    llvm::Value* calleeValue = generateExpression(expr->callee.get());
    if (!calleeValue) {
        return nullptr;
    }
    
    // Generate arguments
    std::vector<llvm::Value*> args;
    for (auto& arg : expr->args) {
        llvm::Value* argValue = generateExpression(arg.get());
        if (!argValue) {
            return nullptr;
        }
        args.push_back(argValue);
    }
    
    // Check if callee is a function or function pointer
    if (llvm::isa<llvm::Function>(calleeValue)) {
        // Direct call to a function
        llvm::Function* calleeFunc = llvm::cast<llvm::Function>(calleeValue);
        return builder->CreateCall(calleeFunc, args, "calltmp");
    } else if (calleeValue->getType()->isPointerTy()) {
        // Indirect call through function pointer
        // The value might be loaded from a variable, so we need to get the function type
        llvm::Type* ptrType = calleeValue->getType();
        
        // For function pointers, LLVM represents them as pointers to function types
        // We need to extract the function type
        if (auto* funcPtrType = llvm::dyn_cast<llvm::PointerType>(ptrType)) {
            // In LLVM, function pointers are opaque pointers in newer versions
            // We need to determine the function type from context
            
            // Try to get function type from the callee expression
            // For now, we'll construct it from the arguments and assume u64 return
            std::vector<llvm::Type*> paramTypes;
            for (auto* arg : args) {
                paramTypes.push_back(arg->getType());
            }
            
            // Default to u64 return type (we'll improve this with type information later)
            llvm::Type* returnType = llvm::Type::getInt64Ty(*context);
            llvm::FunctionType* funcType = llvm::FunctionType::get(returnType, paramTypes, false);
            
            return builder->CreateCall(funcType, calleeValue, args, "indirectcall");
        }
    }
    
    reportError("Invalid callee in function call");
    return nullptr;
}

llvm::Value* IRGenerator::generateCast(CastExpr* expr) {
    llvm::Value* value = generateExpression(expr->expr.get());
    if (!value) {
        return nullptr;
    }
    
    llvm::Type* targetType = convertType(expr->targetType.get());
    if (!targetType) {
        reportError("Failed to convert cast target type");
        return nullptr;
    }
    
    // Perform appropriate cast based on types
    if (value->getType()->isIntegerTy() && targetType->isIntegerTy()) {
        // Integer to integer cast
        return builder->CreateIntCast(value, targetType, false, "casttmp");
    } else if (value->getType()->isPointerTy() && targetType->isIntegerTy()) {
        // Pointer to integer cast
        return builder->CreatePtrToInt(value, targetType, "casttmp");
    } else if (value->getType()->isIntegerTy() && targetType->isPointerTy()) {
        // Integer to pointer cast
        return builder->CreateIntToPtr(value, targetType, "casttmp");
    } else if (value->getType()->isPointerTy() && targetType->isPointerTy()) {
        // Pointer to pointer cast
        return builder->CreatePointerCast(value, targetType, "casttmp");
    }
    
    reportError("Unsupported cast operation");
    return nullptr;
}

llvm::Value* IRGenerator::generateStructInit(StructInitExpr* expr) {
    // Look up struct type
    llvm::StructType* structType = llvm::StructType::getTypeByName(*context, expr->structName);
    if (!structType) {
        reportError("Undefined struct type: " + expr->structName);
        return nullptr;
    }
    
    // Look up struct definition
    auto it = structDefinitions.find(expr->structName);
    if (it == structDefinitions.end()) {
        reportError("Struct definition not found: " + expr->structName);
        return nullptr;
    }
    StructAST* structDef = it->second;
    
    // Allocate space for the struct on the stack
    llvm::AllocaInst* structAlloca = builder->CreateAlloca(structType, nullptr, "structtmp");
    
    // Initialize each field
    for (const auto& fieldInit : expr->fields) {
        // Find field index
        int fieldIndex = -1;
        for (size_t i = 0; i < structDef->fields.size(); i++) {
            if (structDef->fields[i].name == fieldInit.first) {
                fieldIndex = static_cast<int>(i);
                break;
            }
        }
        
        if (fieldIndex == -1) {
            reportError("Unknown field '" + fieldInit.first + "' in struct " + expr->structName);
            continue;
        }
        
        // Generate field value
        llvm::Value* fieldValue = generateExpression(fieldInit.second.get());
        if (!fieldValue) {
            continue;
        }
        
        // Get pointer to field using GEP
        llvm::Value* fieldPtr = builder->CreateStructGEP(
            structType,
            structAlloca,
            fieldIndex,
            fieldInit.first
        );
        
        // Store value in field
        builder->CreateStore(fieldValue, fieldPtr);
    }
    
    // Load and return the struct value
    return builder->CreateLoad(structType, structAlloca, "structval");
}

llvm::Value* IRGenerator::generateFieldAccess(FieldAccessExpr* expr) {
    // Get the address of the field
    llvm::Value* fieldAddr = generateFieldAddress(expr);
    if (!fieldAddr) {
        return nullptr;
    }
    
    // Determine the field type by looking up the struct definition
    // We need to get the struct type from the object expression
    llvm::Type* structType = nullptr;
    std::string structName;
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr->object.get())) {
        auto typeIt = namedTypes.find(ident->name);
        if (typeIt != namedTypes.end()) {
            structType = typeIt->second;
            if (auto* st = llvm::dyn_cast<llvm::StructType>(structType)) {
                structName = st->getName().str();
            }
        }
    }
    
    if (!structType || structName.empty()) {
        reportError("Cannot determine struct type for field access");
        return nullptr;
    }
    
    // Look up struct definition
    auto it = structDefinitions.find(structName);
    if (it == structDefinitions.end()) {
        reportError("Struct definition not found: " + structName);
        return nullptr;
    }
    StructAST* structDef = it->second;
    
    // Find the field and its type
    llvm::Type* fieldType = nullptr;
    for (size_t i = 0; i < structDef->fields.size(); i++) {
        if (structDef->fields[i].name == expr->fieldName) {
            fieldType = convertType(structDef->fields[i].type.get());
            break;
        }
    }
    
    if (!fieldType) {
        reportError("Field type not found for: " + expr->fieldName);
        return nullptr;
    }
    
    // Load the field value with the correct type
    return builder->CreateLoad(fieldType, fieldAddr, "fieldval");
}

llvm::Value* IRGenerator::generateArrayIndex(ArrayIndexExpr* expr) {
    // Get the address of the array element
    llvm::Value* elemAddr = generateArrayElementAddress(expr);
    if (!elemAddr) {
        return nullptr;
    }
    
    // Determine the element type
    // Try to get the array type from the array expression
    llvm::Type* elemType = llvm::Type::getInt64Ty(*context); // Default to u64
    
    // If the array is a simple identifier, look up its type
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr->array.get())) {
        auto it = namedTypes.find(ident->name);
        if (it != namedTypes.end()) {
            llvm::Type* arrayType = it->second;
            if (auto* arrType = llvm::dyn_cast<llvm::ArrayType>(arrayType)) {
                elemType = arrType->getElementType();
            } else {
                // Check if it's a pointer with a known pointee type
                auto pIt = pointeeTypes.find(ident->name);
                if (pIt != pointeeTypes.end() && pIt->second) {
                    elemType = pIt->second;
                }
            }
        }
    }
    
    // Load the element value with the correct type
    return builder->CreateLoad(elemType, elemAddr, "elemval");
}

llvm::Value* IRGenerator::generateSyscall(SyscallExpr* expr) {
    // Generate syscall using platform-specific inline assembly
    // For x86_64 Linux: syscall instruction with args in rdi, rsi, rdx, r10, r8, r9
    // For ARM64 Linux: svc #0 instruction
    
    // Get target triple to determine platform
    std::string targetTriple = module->getTargetTriple();
    bool isX86_64 = targetTriple.find("x86_64") != std::string::npos || 
                    targetTriple.find("x86-64") != std::string::npos ||
                    targetTriple.empty(); // Default to x86_64 if no triple set
    bool isARM64 = targetTriple.find("aarch64") != std::string::npos ||
                   targetTriple.find("arm64") != std::string::npos;
    
    if (!isX86_64 && !isARM64) {
        reportError("Syscalls only supported on x86_64 and ARM64 platforms");
        return nullptr;
    }
    
    // Generate syscall number (now an expression that can be a constant or literal)
    llvm::Value* syscallNum = generateExpression(expr->syscallNumber.get());
    if (!syscallNum) {
        reportError("Failed to generate syscall number");
        return nullptr;
    }
    
    // Ensure syscall number is u64
    if (syscallNum->getType() != llvm::Type::getInt64Ty(*context)) {
        if (syscallNum->getType()->isIntegerTy()) {
            syscallNum = builder->CreateZExt(syscallNum, llvm::Type::getInt64Ty(*context), "syscall_num_ext");
        } else {
            reportError("Syscall number must be an integer type");
            return nullptr;
        }
    }
    
    // Generate arguments (up to 6 arguments supported)
    std::vector<llvm::Value*> args;
    args.push_back(syscallNum);
    
    for (size_t i = 0; i < expr->args.size() && i < 6; i++) {
        llvm::Value* arg = generateExpression(expr->args[i].get());
        if (!arg) {
            return nullptr;
        }
        
        // Ensure argument is u64
        if (arg->getType() != llvm::Type::getInt64Ty(*context)) {
            if (arg->getType()->isIntegerTy()) {
                arg = builder->CreateZExt(arg, llvm::Type::getInt64Ty(*context), "syscall_arg_ext");
            } else if (arg->getType()->isPointerTy()) {
                arg = builder->CreatePtrToInt(arg, llvm::Type::getInt64Ty(*context), "syscall_arg_ptr");
            } else {
                reportError("Syscall argument must be integer or pointer type");
                return nullptr;
            }
        }
        
        args.push_back(arg);
    }
    
    // Pad with zeros if fewer than 6 arguments
    while (args.size() < 7) { // syscall number + 6 args
        args.push_back(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0));
    }
    
    // Create inline assembly based on platform
    std::string asmStr;
    std::string constraints;
    
    if (isX86_64) {
        // x86_64 Linux syscall ABI:
        // syscall number in rax, args in rdi, rsi, rdx, r10, r8, r9
        // Result returned in rax
        asmStr = "syscall";
        
        // Use explicit register constraints to avoid LLVM reordering
        // Format: output constraint, input constraints for each register
        constraints = "={rax},{rax},{rdi},{rsi},{rdx},{r10},{r8},{r9},~{rcx},~{r11},~{memory}";
    } else { // ARM64
        // ARM64 Linux syscall ABI:
        // syscall number in x8, args in x0-x5
        // Result returned in x0
        asmStr = "svc #0";
        
        // Use explicit register constraints
        constraints = "={x0},{x8},{x0},{x1},{x2},{x3},{x4},{x5},~{memory}";
    }
    
    // Create function type for inline assembly
    std::vector<llvm::Type*> asmParamTypes(7, llvm::Type::getInt64Ty(*context));
    llvm::FunctionType* asmFuncType = llvm::FunctionType::get(
        llvm::Type::getInt64Ty(*context),
        asmParamTypes,
        false
    );
    
    // Create inline assembly
    llvm::InlineAsm* inlineAsm = llvm::InlineAsm::get(
        asmFuncType,
        asmStr,
        constraints,
        true,  // hasSideEffects
        false, // isAlignStack
        llvm::InlineAsm::AD_ATT // Dialect (AT&T syntax)
    );
    
    // Call the inline assembly
    return builder->CreateCall(inlineAsm, args, "syscall_result");
}

llvm::Value* IRGenerator::generateLValueAddress(ExprAST* expr) {
    // Generate the address of an lvalue expression
    // This is used for the address-of operator (&) and for assignments
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        // Simple variable: return its alloca
        llvm::Value* value = namedValues[ident->name];
        if (!value) {
            reportError("Undefined variable in address-of: " + ident->name);
            return nullptr;
        }
        return value;
    }
    
    if (auto* deref = dynamic_cast<UnaryExpr*>(expr)) {
        if (deref->op == UnaryOp::Deref) {
            // Dereference: the address is the pointer value itself
            return generateExpression(deref->operand.get());
        }
    }
    
    if (auto* arrayIndex = dynamic_cast<ArrayIndexExpr*>(expr)) {
        // Array indexing: compute the element address
        return generateArrayElementAddress(arrayIndex);
    }
    
    if (auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(expr)) {
        // Field access: compute field address
        return generateFieldAddress(fieldAccess);
    }
    
    reportError("Expression is not an lvalue");
    return nullptr;
}

llvm::Value* IRGenerator::generateFieldAddress(FieldAccessExpr* expr) {
    // Get the address of the struct
    llvm::Value* structAddr = generateLValueAddress(expr->object.get());
    if (!structAddr) {
        return nullptr;
    }
    
    // Determine the struct type from the object expression
    llvm::Type* ptrType = structAddr->getType();
    if (!ptrType->isPointerTy()) {
        reportError("Field access on non-pointer type");
        return nullptr;
    }
    
    // In LLVM 18 with opaque pointers, we need to track the pointee type separately
    // Look it up from namedTypes if it's a simple variable
    llvm::Type* structType = nullptr;
    std::string structName;
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr->object.get())) {
        auto typeIt = namedTypes.find(ident->name);
        if (typeIt != namedTypes.end()) {
            structType = typeIt->second;
            if (auto* st = llvm::dyn_cast<llvm::StructType>(structType)) {
                structName = st->getName().str();
            }
        }
    }
    
    if (!structType || structName.empty()) {
        reportError("Cannot determine struct type for field access");
        return nullptr;
    }
    
    // Look up struct definition
    auto it = structDefinitions.find(structName);
    if (it == structDefinitions.end()) {
        reportError("Struct definition not found: " + structName);
        return nullptr;
    }
    StructAST* structDef = it->second;
    
    // Find field index
    int fieldIndex = -1;
    for (size_t i = 0; i < structDef->fields.size(); i++) {
        if (structDef->fields[i].name == expr->fieldName) {
            fieldIndex = static_cast<int>(i);
            break;
        }
    }
    
    if (fieldIndex == -1) {
        reportError("Unknown field '" + expr->fieldName + "' in struct " + structName);
        return nullptr;
    }
    
    // Use GEP to get field address
    return builder->CreateStructGEP(
        structType,
        structAddr,
        fieldIndex,
        expr->fieldName
    );
}

llvm::Value* IRGenerator::generateArrayElementAddress(ArrayIndexExpr* expr) {
    llvm::Value* arrayPtr = nullptr;
    llvm::Type* elementType = llvm::Type::getInt8Ty(*context);
    
    // Check if it's a direct array variable access
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr->array.get())) {
        auto it = namedTypes.find(ident->name);
        if (it != namedTypes.end()) {
            llvm::Type* type = it->second;
            if (type->isArrayTy()) {
                // It's a stack-allocated array: [N x T]
                // We want its address (the alloca) and use two indices: [0, index]
                arrayPtr = namedValues[ident->name];
                elementType = type->getArrayElementType();
                
                llvm::Value* index = generateExpression(expr->index.get());
                if (!arrayPtr || !index) return nullptr;
                
                std::vector<llvm::Value*> indices = {
                    llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
                    index
                };
                
                return builder->CreateGEP(type, arrayPtr, indices, "elemaddr");
            } else {
                // It's a pointer or other type: evaluate it normally (loads the pointer)
                arrayPtr = generateExpression(expr->array.get());
                
                // Try to determine element type from pointeeTypes map
                auto pIt = pointeeTypes.find(ident->name);
                if (pIt != pointeeTypes.end() && pIt->second) {
                    elementType = pIt->second;
                }
            }
        }
    }
    
    // Fallback for general expressions (e.g. results of calls or pointer arithmetic)
    if (!arrayPtr) {
        arrayPtr = generateExpression(expr->array.get());
    }
    
    llvm::Value* index = generateExpression(expr->index.get());
    if (!arrayPtr || !index) return nullptr;
    
    // For pointers, use single index GEP
    return builder->CreateGEP(elementType, arrayPtr, index, "elemaddr");
}

llvm::Constant* IRGenerator::evaluateConstantExpression(ExprAST* expr) {
    // Evaluate expressions at compile time for constants
    if (auto* intLit = dynamic_cast<IntLiteralExpr*>(expr)) {
        return llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), intLit->value);
    }
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(expr)) {
        // Check if it's a constant
        auto it = constants.find(ident->name);
        if (it != constants.end()) {
            return it->second;
        }
        reportError("Undefined constant: " + ident->name);
        return nullptr;
    }
    
    if (auto* binary = dynamic_cast<BinaryExpr*>(expr)) {
        llvm::Constant* left = evaluateConstantExpression(binary->left.get());
        llvm::Constant* right = evaluateConstantExpression(binary->right.get());
        
        if (!left || !right) {
            return nullptr;
        }
        
        // For operations not directly available in ConstantExpr, evaluate at compile time
        // by extracting integer values and computing the result
        auto* leftInt = llvm::dyn_cast<llvm::ConstantInt>(left);
        auto* rightInt = llvm::dyn_cast<llvm::ConstantInt>(right);
        
        if (!leftInt || !rightInt) {
            reportError("Constant expression must be integer type");
            return nullptr;
        }
        
        uint64_t leftVal = leftInt->getZExtValue();
        uint64_t rightVal = rightInt->getZExtValue();
        uint64_t result = 0;
        
        switch (binary->op) {
            case BinaryOp::Add:
                return llvm::ConstantExpr::getAdd(left, right);
            case BinaryOp::Sub:
                return llvm::ConstantExpr::getSub(left, right);
            case BinaryOp::Mul:
                return llvm::ConstantExpr::getMul(left, right);
            case BinaryOp::Div:
                if (rightVal == 0) {
                    reportError("Division by zero in constant expression");
                    return nullptr;
                }
                result = leftVal / rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::Mod:
                if (rightVal == 0) {
                    reportError("Modulo by zero in constant expression");
                    return nullptr;
                }
                result = leftVal % rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::BitAnd:
                result = leftVal & rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::BitOr:
                result = leftVal | rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::BitXor:
                return llvm::ConstantExpr::getXor(left, right);
            case BinaryOp::LeftShift:
                result = leftVal << rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::RightShift:
                result = leftVal >> rightVal;
                return llvm::ConstantInt::get(leftInt->getType(), result);
            case BinaryOp::Equal:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_EQ, left, right);
            case BinaryOp::NotEqual:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_NE, left, right);
            case BinaryOp::Less:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_ULT, left, right);
            case BinaryOp::Greater:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_UGT, left, right);
            case BinaryOp::LessEqual:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_ULE, left, right);
            case BinaryOp::GreaterEqual:
                return llvm::ConstantExpr::getICmp(llvm::CmpInst::ICMP_UGE, left, right);
        }
    }
    
    if (auto* unary = dynamic_cast<UnaryExpr*>(expr)) {
        llvm::Constant* operand = evaluateConstantExpression(unary->operand.get());
        if (!operand) {
            return nullptr;
        }
        
        switch (unary->op) {
            case UnaryOp::Negate:
                return llvm::ConstantExpr::getNeg(operand);
            case UnaryOp::Not:
                return llvm::ConstantExpr::getNot(operand);
            default:
                return nullptr;
        }
    }
    
    if (auto* cast = dynamic_cast<CastExpr*>(expr)) {
        llvm::Constant* value = evaluateConstantExpression(cast->expr.get());
        if (!value) {
            return nullptr;
        }
        
        llvm::Type* targetType = convertType(cast->targetType.get());
        if (!targetType) {
            return nullptr;
        }
        
        // Perform constant cast
        if (value->getType()->isIntegerTy() && targetType->isIntegerTy()) {
            // For integer constants, we can just create a new constant with the target type
            if (auto* constInt = llvm::dyn_cast<llvm::ConstantInt>(value)) {
                uint64_t intValue = constInt->getZExtValue();
                return llvm::ConstantInt::get(targetType, intValue);
            }
            return nullptr;
        } else if (value->getType()->isPointerTy() && targetType->isIntegerTy()) {
            return llvm::ConstantExpr::getPtrToInt(value, targetType);
        } else if (value->getType()->isIntegerTy() && targetType->isPointerTy()) {
            return llvm::ConstantExpr::getIntToPtr(value, targetType);
        } else if (value->getType()->isPointerTy() && targetType->isPointerTy()) {
            return llvm::ConstantExpr::getPointerCast(value, targetType);
        }
    }
    
    return nullptr;
}

llvm::Value* IRGenerator::generateEncBlock(EncBlockExpr* expr) {
    // Generate encrypted string literal with runtime decryption
    // 
    // Implementation:
    // 1. Generate random XOR key at compile time
    // 2. Encrypt string bytes: encrypted[i] = plaintext[i] ^ key[i % key_len]
    // 3. Store encrypted bytes and key as global arrays
    // 4. Generate inline decryption code
    // 5. Return pointer to decrypted buffer
    
    const std::string& plaintext = expr->value;
    size_t len = plaintext.length();
    
    // Generate random XOR key (16 bytes for reasonable security)
    const size_t keyLen = 16;
    std::vector<uint8_t> key(keyLen);
    
    // Use a simple pseudo-random generator for compile-time key generation
    // In production, this should use a cryptographically secure RNG
    static uint32_t seed = 0x12345678;  // Fixed seed for deterministic builds
    for (size_t i = 0; i < keyLen; i++) {
        seed = seed * 1103515245 + 12345;  // Linear congruential generator
        key[i] = (seed >> 16) & 0xFF;
    }
    
    // Encrypt the string
    std::vector<uint8_t> encrypted(len);
    for (size_t i = 0; i < len; i++) {
        encrypted[i] = static_cast<uint8_t>(plaintext[i]) ^ key[i % keyLen];
    }
    
    // Create global array for encrypted data
    std::vector<llvm::Constant*> encryptedBytes;
    for (uint8_t byte : encrypted) {
        encryptedBytes.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), byte));
    }
    
    llvm::ArrayType* encArrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), len);
    llvm::Constant* encArrayInit = llvm::ConstantArray::get(encArrayType, encryptedBytes);
    
    llvm::GlobalVariable* encGlobal = new llvm::GlobalVariable(
        *module,
        encArrayType,
        true,  // isConstant
        llvm::GlobalValue::PrivateLinkage,
        encArrayInit,
        ".enc_data"
    );
    
    // Create global array for XOR key
    std::vector<llvm::Constant*> keyBytes;
    for (uint8_t byte : key) {
        keyBytes.push_back(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), byte));
    }
    
    llvm::ArrayType* keyArrayType = llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), keyLen);
    llvm::Constant* keyArrayInit = llvm::ConstantArray::get(keyArrayType, keyBytes);
    
    llvm::GlobalVariable* keyGlobal = new llvm::GlobalVariable(
        *module,
        keyArrayType,
        true,  // isConstant
        llvm::GlobalValue::PrivateLinkage,
        keyArrayInit,
        ".enc_key"
    );
    
    // Allocate buffer for decrypted string on the stack
    llvm::AllocaInst* decryptedBuffer = builder->CreateAlloca(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), len + 1),  // +1 for null terminator
        nullptr,
        "decrypted"
    );
    
    // Generate decryption loop
    // for (i = 0; i < len; i++) {
    //     decrypted[i] = encrypted[i] ^ key[i % keyLen];
    // }
    // decrypted[len] = 0;  // null terminator
    
    llvm::BasicBlock* loopCondBB = llvm::BasicBlock::Create(*context, "enc_loop_cond", currentFunction);
    llvm::BasicBlock* loopBodyBB = llvm::BasicBlock::Create(*context, "enc_loop_body", currentFunction);
    llvm::BasicBlock* loopEndBB = llvm::BasicBlock::Create(*context, "enc_loop_end", currentFunction);
    
    // Allocate loop counter
    llvm::AllocaInst* counterAlloca = builder->CreateAlloca(llvm::Type::getInt64Ty(*context), nullptr, "enc_i");
    builder->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0), counterAlloca);
    
    // Branch to loop condition
    builder->CreateBr(loopCondBB);
    
    // Loop condition: i < len
    builder->SetInsertPoint(loopCondBB);
    llvm::Value* counter = builder->CreateLoad(llvm::Type::getInt64Ty(*context), counterAlloca, "i");
    llvm::Value* lenValue = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), len);
    llvm::Value* cond = builder->CreateICmpULT(counter, lenValue, "enc_cond");
    builder->CreateCondBr(cond, loopBodyBB, loopEndBB);
    
    // Loop body: decrypted[i] = encrypted[i] ^ key[i % keyLen]
    builder->SetInsertPoint(loopBodyBB);
    
    // Load encrypted[i]
    llvm::Value* encPtr = builder->CreateGEP(encArrayType, encGlobal, {
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
        counter
    }, "enc_ptr");
    llvm::Value* encByte = builder->CreateLoad(llvm::Type::getInt8Ty(*context), encPtr, "enc_byte");
    
    // Calculate i % keyLen
    llvm::Value* keyLenValue = llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), keyLen);
    llvm::Value* keyIndex = builder->CreateURem(counter, keyLenValue, "key_idx");
    
    // Load key[i % keyLen]
    llvm::Value* keyPtr = builder->CreateGEP(keyArrayType, keyGlobal, {
        llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
        keyIndex
    }, "key_ptr");
    llvm::Value* keyByte = builder->CreateLoad(llvm::Type::getInt8Ty(*context), keyPtr, "key_byte");
    
    // XOR: decrypted[i] = encrypted[i] ^ key[i % keyLen]
    llvm::Value* decByte = builder->CreateXor(encByte, keyByte, "dec_byte");
    
    // Store to decrypted[i]
    llvm::Value* decPtr = builder->CreateGEP(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), len + 1),
        decryptedBuffer,
        {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
            counter
        },
        "dec_ptr"
    );
    builder->CreateStore(decByte, decPtr);
    
    // Increment counter: i++
    llvm::Value* nextCounter = builder->CreateAdd(counter, llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 1), "next_i");
    builder->CreateStore(nextCounter, counterAlloca);
    
    // Branch back to condition
    builder->CreateBr(loopCondBB);
    
    // After loop: add null terminator
    builder->SetInsertPoint(loopEndBB);
    llvm::Value* nullTermPtr = builder->CreateGEP(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), len + 1),
        decryptedBuffer,
        {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
            lenValue
        },
        "null_term_ptr"
    );
    builder->CreateStore(llvm::ConstantInt::get(llvm::Type::getInt8Ty(*context), 0), nullTermPtr);
    
    // Return pointer to decrypted buffer (cast to i8*)
    llvm::Value* bufferPtr = builder->CreateGEP(
        llvm::ArrayType::get(llvm::Type::getInt8Ty(*context), len + 1),
        decryptedBuffer,
        {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0),
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(*context), 0)
        },
        "enc_result"
    );
    
    return bufferPtr;
}

// Debug info helpers
void IRGenerator::initializeDebugInfo() {
    // Create debug info builder
    debugBuilder = std::make_unique<llvm::DIBuilder>(*module);
    
    // Get absolute path for source file
    llvm::SmallString<256> absPath;
    if (!sourceFilename.empty()) {
        llvm::sys::fs::real_path(sourceFilename, absPath);
    } else {
        absPath = "unknown.lwanga";
    }
    
    // Extract directory and filename
    llvm::StringRef filename = llvm::sys::path::filename(absPath);
    llvm::SmallString<256> directory = absPath;
    llvm::sys::path::remove_filename(directory);
    
    // Create debug file
    debugFile = debugBuilder->createFile(filename, directory);
    
    // Create compile unit
    compileUnit = debugBuilder->createCompileUnit(
        llvm::dwarf::DW_LANG_C,  // Use C language tag (closest to Lwanga)
        debugFile,
        "Lwanga Compiler v" LWANGA_VERSION,  // Producer
        false,  // isOptimized (will be set by backend)
        "",     // Flags
        0       // Runtime version
    );
    
    // Set module debug info
    module->addModuleFlag(llvm::Module::Warning, "Debug Info Version", 
                         llvm::DEBUG_METADATA_VERSION);
    module->addModuleFlag(llvm::Module::Warning, "Dwarf Version", 4);
}

void IRGenerator::finalizeDebugInfo() {
    if (debugBuilder) {
        debugBuilder->finalize();
    }
}

llvm::DIType* IRGenerator::getDebugType(const Type* type) {
    if (!type || !debugBuilder) {
        return nullptr;
    }
    
    switch (type->kind) {
        case TypeKind::U8:
            return debugBuilder->createBasicType("u8", 8, llvm::dwarf::DW_ATE_unsigned);
        case TypeKind::U16:
            return debugBuilder->createBasicType("u16", 16, llvm::dwarf::DW_ATE_unsigned);
        case TypeKind::U32:
            return debugBuilder->createBasicType("u32", 32, llvm::dwarf::DW_ATE_unsigned);
        case TypeKind::U64:
            return debugBuilder->createBasicType("u64", 64, llvm::dwarf::DW_ATE_unsigned);
        case TypeKind::Ptr:
            return debugBuilder->createPointerType(
                debugBuilder->createBasicType("u8", 8, llvm::dwarf::DW_ATE_unsigned),
                64  // Pointer size in bits
            );
        case TypeKind::Array: {
            llvm::DIType* elemType = getDebugType(type->elementType.get());
            if (!elemType) return nullptr;
            
            llvm::Metadata* subscript = debugBuilder->getOrCreateSubrange(0, type->arraySize);
            llvm::DINodeArray subscripts = debugBuilder->getOrCreateArray({subscript});
            
            return debugBuilder->createArrayType(
                type->arraySize * elemType->getSizeInBits(),
                elemType->getAlignInBits(),
                elemType,
                subscripts
            );
        }
        case TypeKind::Struct:
            // For structs, we'd need to create a composite type
            // For now, return a basic opaque type
            return debugBuilder->createBasicType(type->structName, 64, llvm::dwarf::DW_ATE_unsigned);
        case TypeKind::FunctionPointer:
            // Function pointers are represented as generic pointers
            return debugBuilder->createPointerType(nullptr, 64);
        case TypeKind::Register:
            return debugBuilder->createBasicType("register", 64, llvm::dwarf::DW_ATE_unsigned);
    }
    
    return nullptr;
}

void IRGenerator::emitLocation(uint32_t line, uint32_t column) {
    if (!generateDebugInfo || !debugBuilder || lexicalBlocks.empty()) {
        return;
    }
    
    llvm::DIScope* scope = lexicalBlocks.back();
    llvm::DILocation* loc = llvm::DILocation::get(
        *context,
        line,
        column,
        scope
    );
    
    builder->SetCurrentDebugLocation(loc);
}

} // namespace lwanga
