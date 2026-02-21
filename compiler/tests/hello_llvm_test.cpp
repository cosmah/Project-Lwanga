#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Support/raw_ostream.h>
#include <iostream>

using namespace llvm;

int main() {
    // Create LLVM context and module
    LLVMContext Context;
    Module TheModule("hello_llvm_test", Context);
    IRBuilder<> Builder(Context);
    
    // Create a simple function: i32 @add(i32 %a, i32 %b)
    FunctionType *FT = FunctionType::get(
        Type::getInt32Ty(Context),
        {Type::getInt32Ty(Context), Type::getInt32Ty(Context)},
        false
    );
    
    Function *AddFunc = Function::Create(
        FT,
        Function::ExternalLinkage,
        "add",
        TheModule
    );
    
    // Create basic block
    BasicBlock *BB = BasicBlock::Create(Context, "entry", AddFunc);
    Builder.SetInsertPoint(BB);
    
    // Get function arguments
    auto ArgIt = AddFunc->arg_begin();
    Value *A = &*ArgIt++;
    Value *B = &*ArgIt;
    A->setName("a");
    B->setName("b");
    
    // Create add instruction
    Value *Sum = Builder.CreateAdd(A, B, "sum");
    
    // Create return
    Builder.CreateRet(Sum);
    
    // Verify the function
    if (verifyFunction(*AddFunc, &errs())) {
        std::cerr << "Error: Function verification failed!\n";
        return 1;
    }
    
    // Print the module
    std::cout << "Generated LLVM IR:\n";
    TheModule.print(outs(), nullptr);
    
    std::cout << "\n✓ LLVM integration test passed!\n";
    return 0;
}
