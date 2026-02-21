#include "IRGenerator.h"
#include <llvm/IR/Verifier.h>
#include <iostream>

namespace lwanga {

IRGenerator::IRGenerator(const std::string& moduleName) 
    : currentFunction(nullptr) {
    context = std::make_unique<llvm::LLVMContext>();
    module = std::make_unique<llvm::Module>(moduleName, *context);
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
    }
    
    return nullptr;
}

void IRGenerator::generateStructDeclarations(ProgramAST* program) {
    // Create opaque struct types first
    for (auto& structDef : program->structs) {
        llvm::StructType::create(*context, structDef->name);
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

void IRGenerator::generateFunction(FunctionAST* func) {
    // Get the function
    llvm::Function* llvmFunc = module->getFunction(func->name);
    if (!llvmFunc) {
        reportError("Function not found: " + func->name);
        return;
    }
    
    currentFunction = llvmFunc;
    
    // Create entry basic block
    llvm::BasicBlock* entryBB = llvm::BasicBlock::Create(*context, "entry", llvmFunc);
    builder->SetInsertPoint(entryBB);
    
    // Clear named values
    namedValues.clear();
    namedTypes.clear();
    
    // Allocate space for parameters and store them
    for (auto& arg : llvmFunc->args()) {
        llvm::AllocaInst* alloca = builder->CreateAlloca(
            arg.getType(),
            nullptr,
            arg.getName()
        );
        builder->CreateStore(&arg, alloca);
        namedValues[std::string(arg.getName())] = alloca;
        namedTypes[std::string(arg.getName())] = arg.getType();
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
    }
}

void IRGenerator::generateVarDecl(VarDeclStmt* stmt) {
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
}

void IRGenerator::generateAssignment(AssignmentStmt* stmt) {
    // Get the target address
    llvm::Value* target = nullptr;
    
    if (auto* ident = dynamic_cast<IdentifierExpr*>(stmt->target.get())) {
        target = namedValues[ident->name];
        if (!target) {
            reportError("Undefined variable: " + ident->name);
            return;
        }
    } else {
        reportError("Complex lvalues not yet supported");
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

llvm::Value* IRGenerator::generateExpression(ExprAST* expr) {
    if (auto* intLit = dynamic_cast<IntLiteralExpr*>(expr)) {
        return generateIntLiteral(intLit);
    } else if (auto* strLit = dynamic_cast<StringLiteralExpr*>(expr)) {
        return generateStringLiteral(strLit);
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

llvm::Value* IRGenerator::generateIdentifier(IdentifierExpr* expr) {
    // Check if it's a constant first
    auto constIt = constants.find(expr->name);
    if (constIt != constants.end()) {
        return constIt->second;
    }
    
    // Otherwise it's a variable
    llvm::Value* value = namedValues[expr->name];
    if (!value) {
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
    llvm::Value* operand = generateExpression(expr->operand.get());
    if (!operand) {
        return nullptr;
    }
    
    switch (expr->op) {
        case UnaryOp::Negate:
            return builder->CreateNeg(operand, "negtmp");
        case UnaryOp::Not:
            return builder->CreateNot(operand, "nottmp");
        case UnaryOp::Deref:
            // Dereference pointer - load from address
            return builder->CreateLoad(
                llvm::Type::getInt64Ty(*context),
                operand,
                "dereftmp"
            );
        case UnaryOp::AddressOf:
            reportError("Address-of operator not yet implemented");
            return nullptr;
    }
    
    return nullptr;
}

llvm::Value* IRGenerator::generateCall(CallExpr* expr) {
    // Get function name
    auto* calleeIdent = dynamic_cast<IdentifierExpr*>(expr->callee.get());
    if (!calleeIdent) {
        reportError("Complex function calls not yet supported");
        return nullptr;
    }
    
    // Look up function
    llvm::Function* calleeFunc = module->getFunction(calleeIdent->name);
    if (!calleeFunc) {
        reportError("Undefined function: " + calleeIdent->name);
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
    
    return builder->CreateCall(calleeFunc, args, "calltmp");
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
        
        // Perform constant folding using available methods in LLVM 18
        switch (binary->op) {
            case BinaryOp::Add:
                return llvm::ConstantExpr::getAdd(left, right);
            case BinaryOp::Sub:
                return llvm::ConstantExpr::getSub(left, right);
            case BinaryOp::Mul:
                return llvm::ConstantExpr::getMul(left, right);
            case BinaryOp::Div:
            case BinaryOp::Mod:
            case BinaryOp::BitAnd:
            case BinaryOp::BitOr:
            case BinaryOp::LeftShift:
            case BinaryOp::RightShift:
                // These operations are not directly available in ConstantExpr in LLVM 18
                // For production, we'd need to evaluate these at compile time differently
                // For now, report error for unsupported constant operations
                reportError("Unsupported constant operation");
                return nullptr;
            case BinaryOp::BitXor:
                return llvm::ConstantExpr::getXor(left, right);
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

} // namespace lwanga
