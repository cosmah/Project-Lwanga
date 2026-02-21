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
    } else if (auto* unsafeBlock = dynamic_cast<UnsafeBlockStmt*>(stmt)) {
        generateUnsafeBlock(unsafeBlock);
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
    } else if (auto* structInit = dynamic_cast<StructInitExpr*>(expr)) {
        return generateStructInit(structInit);
    } else if (auto* fieldAccess = dynamic_cast<FieldAccessExpr*>(expr)) {
        return generateFieldAccess(fieldAccess);
    } else if (auto* arrayIndex = dynamic_cast<ArrayIndexExpr*>(expr)) {
        return generateArrayIndex(arrayIndex);
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
        auto typeIt = namedTypes.find(ident->name);
        if (typeIt != namedTypes.end()) {
            llvm::Type* arrayType = typeIt->second;
            if (auto* arrType = llvm::dyn_cast<llvm::ArrayType>(arrayType)) {
                elemType = arrType->getElementType();
            }
        }
    }
    
    // Load the element value with the correct type
    return builder->CreateLoad(elemType, elemAddr, "elemval");
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
    // Get the array pointer
    llvm::Value* arrayPtr = generateExpression(expr->array.get());
    llvm::Value* index = generateExpression(expr->index.get());
    
    if (!arrayPtr || !index) {
        return nullptr;
    }
    
    // Use GEP to compute element address
    return builder->CreateGEP(
        llvm::Type::getInt8Ty(*context),
        arrayPtr,
        index,
        "elemaddr"
    );
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

} // namespace lwanga
