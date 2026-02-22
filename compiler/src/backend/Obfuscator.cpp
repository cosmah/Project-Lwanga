#include "Obfuscator.h"
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Constants.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#include <chrono>

namespace lwanga {

Obfuscator::Obfuscator(llvm::Module* mod, unsigned seed)
    : module(mod) {
    
    // Use provided seed or current time
    if (seed == 0) {
        seed = std::chrono::system_clock::now().time_since_epoch().count();
    }
    rng.seed(seed);
}

void Obfuscator::obfuscate() {
    // Apply obfuscation passes in order
    flattenControlFlow();
    insertOpaquePredicates();
    substituteInstructions();
}

bool Obfuscator::shouldObfuscateFunction(llvm::Function* func) {
    // Don't obfuscate declarations or intrinsics
    if (func->isDeclaration()) {
        return false;
    }
    
    // Don't obfuscate very small functions (< 3 basic blocks)
    if (func->size() < 3) {
        return false;
    }
    
    return true;
}

void Obfuscator::flattenControlFlow() {
    std::vector<llvm::Function*> functionsToFlatten;
    
    // Collect functions to obfuscate
    for (auto& func : *module) {
        if (shouldObfuscateFunction(&func)) {
            functionsToFlatten.push_back(&func);
        }
    }
    
    // Flatten each function
    for (auto* func : functionsToFlatten) {
        flattenFunction(func);
    }
}

void Obfuscator::flattenFunction(llvm::Function* func) {
    // Collect all basic blocks except entry
    std::vector<llvm::BasicBlock*> blocks;
    llvm::BasicBlock* entry = &func->getEntryBlock();
    
    for (auto& bb : *func) {
        if (&bb != entry) {
            blocks.push_back(&bb);
        }
    }
    
    if (blocks.empty()) {
        return;
    }
    
    // Create state variable in entry block
    llvm::IRBuilder<> entryBuilder(entry, entry->begin());
    llvm::AllocaInst* stateVar = entryBuilder.CreateAlloca(
        entryBuilder.getInt32Ty(), nullptr, "state");
    
    // Initialize state to 0 (first block)
    entryBuilder.CreateStore(entryBuilder.getInt32(0), stateVar);
    
    // Create dispatch block
    llvm::LLVMContext& ctx = func->getContext();
    llvm::BasicBlock* dispatch = llvm::BasicBlock::Create(ctx, "dispatch", func);
    
    // Replace entry terminator with jump to dispatch
    if (entry->getTerminator()) {
        entry->getTerminator()->eraseFromParent();
    }
    entryBuilder.SetInsertPoint(entry);
    entryBuilder.CreateBr(dispatch);
    
    // Create switch in dispatch block
    llvm::IRBuilder<> dispatchBuilder(dispatch);
    llvm::Value* state = dispatchBuilder.CreateLoad(
        dispatchBuilder.getInt32Ty(), stateVar, "state_val");
    
    llvm::BasicBlock* defaultBlock = blocks[0];
    llvm::SwitchInst* switchInst = dispatchBuilder.CreateSwitch(
        state, defaultBlock, blocks.size());
    
    // Add cases for each block
    for (size_t i = 0; i < blocks.size(); i++) {
        switchInst->addCase(dispatchBuilder.getInt32(i), blocks[i]);
    }
    
    // Modify each block to update state and jump to dispatch
    for (size_t i = 0; i < blocks.size(); i++) {
        llvm::BasicBlock* bb = blocks[i];
        
        // Get terminator
        llvm::Instruction* terminator = bb->getTerminator();
        if (!terminator) {
            continue;
        }
        
        llvm::IRBuilder<> builder(terminator);
        
        if (auto* br = llvm::dyn_cast<llvm::BranchInst>(terminator)) {
            if (br->isUnconditional()) {
                // Find target block index
                llvm::BasicBlock* target = br->getSuccessor(0);
                auto it = std::find(blocks.begin(), blocks.end(), target);
                if (it != blocks.end()) {
                    int targetIdx = std::distance(blocks.begin(), it);
                    builder.CreateStore(builder.getInt32(targetIdx), stateVar);
                    br->setSuccessor(0, dispatch);
                }
            } else {
                // Conditional branch
                llvm::Value* cond = br->getCondition();
                llvm::BasicBlock* trueTarget = br->getSuccessor(0);
                llvm::BasicBlock* falseTarget = br->getSuccessor(1);
                
                auto trueIt = std::find(blocks.begin(), blocks.end(), trueTarget);
                auto falseIt = std::find(blocks.begin(), blocks.end(), falseTarget);
                
                if (trueIt != blocks.end() && falseIt != blocks.end()) {
                    int trueIdx = std::distance(blocks.begin(), trueIt);
                    int falseIdx = std::distance(blocks.begin(), falseIt);
                    
                    llvm::Value* stateVal = builder.CreateSelect(
                        cond,
                        builder.getInt32(trueIdx),
                        builder.getInt32(falseIdx)
                    );
                    builder.CreateStore(stateVal, stateVar);
                    
                    // Replace with unconditional branch to dispatch
                    llvm::BranchInst::Create(dispatch, terminator);
                    terminator->eraseFromParent();
                }
            }
        } else if (auto* ret = llvm::dyn_cast<llvm::ReturnInst>(terminator)) {
            // Don't modify return instructions
            continue;
        }
    }
}

void Obfuscator::insertOpaquePredicates() {
    for (auto& func : *module) {
        if (shouldObfuscateFunction(&func)) {
            insertOpaquePredicateInFunction(&func);
        }
    }
}

void Obfuscator::insertOpaquePredicateInFunction(llvm::Function* func) {
    std::vector<llvm::BasicBlock*> blocks;
    for (auto& bb : *func) {
        blocks.push_back(&bb);
    }
    
    // Insert opaque predicates in random blocks
    std::uniform_int_distribution<> dist(0, blocks.size() - 1);
    int numPredicates = std::min(3, static_cast<int>(blocks.size()));
    
    for (int i = 0; i < numPredicates; i++) {
        llvm::BasicBlock* bb = blocks[dist(rng)];
        
        // Skip if block is too small
        if (bb->size() < 2) {
            continue;
        }
        
        // Insert at random position in block
        auto it = bb->begin();
        std::advance(it, bb->size() / 2);
        
        llvm::IRBuilder<> builder(&*it);
        
        // Create opaque predicate (always true)
        llvm::Value* pred = createAlwaysTruePredicate(builder);
        
        // Create dummy blocks
        llvm::BasicBlock* trueBB = llvm::BasicBlock::Create(
            func->getContext(), "opaque_true", func);
        llvm::BasicBlock* falseBB = llvm::BasicBlock::Create(
            func->getContext(), "opaque_false", func);
        llvm::BasicBlock* mergeBB = llvm::BasicBlock::Create(
            func->getContext(), "opaque_merge", func);
        
        // Split current block
        llvm::BasicBlock* afterBB = bb->splitBasicBlock(&*it, "after_opaque");
        
        // Remove the unconditional branch created by split
        bb->getTerminator()->eraseFromParent();
        
        // Add conditional branch based on opaque predicate
        builder.SetInsertPoint(bb);
        builder.CreateCondBr(pred, trueBB, falseBB);
        
        // True branch (real path)
        builder.SetInsertPoint(trueBB);
        builder.CreateBr(mergeBB);
        
        // False branch (never taken, but adds complexity)
        builder.SetInsertPoint(falseBB);
        // Add some dummy operations
        llvm::Value* dummy = builder.CreateAdd(
            builder.getInt32(42),
            builder.getInt32(13)
        );
        builder.CreateBr(mergeBB);
        
        // Merge block
        builder.SetInsertPoint(mergeBB);
        builder.CreateBr(afterBB);
        
        break; // Only one predicate per function for now
    }
}

llvm::Value* Obfuscator::createAlwaysTruePredicate(llvm::IRBuilder<>& builder) {
    // Create a predicate that's always true but hard to analyze
    // Example: (x * x) >= 0 for any integer x
    llvm::Value* x = builder.getInt32(rng() % 100 + 1);
    llvm::Value* xSquared = builder.CreateMul(x, x);
    llvm::Value* zero = builder.getInt32(0);
    return builder.CreateICmpSGE(xSquared, zero);
}

llvm::Value* Obfuscator::createAlwaysFalsePredicate(llvm::IRBuilder<>& builder) {
    // Create a predicate that's always false but hard to analyze
    // Example: (x * x) < 0 for any integer x
    llvm::Value* x = builder.getInt32(rng() % 100 + 1);
    llvm::Value* xSquared = builder.CreateMul(x, x);
    llvm::Value* zero = builder.getInt32(0);
    return builder.CreateICmpSLT(xSquared, zero);
}

void Obfuscator::substituteInstructions() {
    for (auto& func : *module) {
        if (shouldObfuscateFunction(&func)) {
            substituteInstructionsInFunction(&func);
        }
    }
}

void Obfuscator::substituteInstructionsInFunction(llvm::Function* func) {
    std::vector<llvm::BinaryOperator*> toSubstitute;
    
    // Collect instructions to substitute
    for (auto& bb : *func) {
        for (auto& inst : bb) {
            if (auto* binOp = llvm::dyn_cast<llvm::BinaryOperator>(&inst)) {
                // Randomly decide whether to substitute (30% chance)
                if (rng() % 100 < 30) {
                    toSubstitute.push_back(binOp);
                }
            }
        }
    }
    
    // Substitute instructions
    for (auto* inst : toSubstitute) {
        switch (inst->getOpcode()) {
            case llvm::Instruction::Add:
                substituteAdd(inst);
                break;
            case llvm::Instruction::Sub:
                substituteSub(inst);
                break;
            case llvm::Instruction::Xor:
                substituteXor(inst);
                break;
            default:
                break;
        }
    }
}

void Obfuscator::substituteAdd(llvm::BinaryOperator* inst) {
    // Replace: a + b
    // With: (a - (-b))
    llvm::IRBuilder<> builder(inst);
    
    llvm::Value* a = inst->getOperand(0);
    llvm::Value* b = inst->getOperand(1);
    
    llvm::Value* negB = builder.CreateNeg(b);
    llvm::Value* result = builder.CreateSub(a, negB);
    
    inst->replaceAllUsesWith(result);
    inst->eraseFromParent();
}

void Obfuscator::substituteSub(llvm::BinaryOperator* inst) {
    // Replace: a - b
    // With: a + (-b)
    llvm::IRBuilder<> builder(inst);
    
    llvm::Value* a = inst->getOperand(0);
    llvm::Value* b = inst->getOperand(1);
    
    llvm::Value* negB = builder.CreateNeg(b);
    llvm::Value* result = builder.CreateAdd(a, negB);
    
    inst->replaceAllUsesWith(result);
    inst->eraseFromParent();
}

void Obfuscator::substituteXor(llvm::BinaryOperator* inst) {
    // Replace: a ^ b
    // With: (a | b) & ~(a & b)
    llvm::IRBuilder<> builder(inst);
    
    llvm::Value* a = inst->getOperand(0);
    llvm::Value* b = inst->getOperand(1);
    
    llvm::Value* aOrB = builder.CreateOr(a, b);
    llvm::Value* aAndB = builder.CreateAnd(a, b);
    llvm::Value* notAAndB = builder.CreateNot(aAndB);
    llvm::Value* result = builder.CreateAnd(aOrB, notAAndB);
    
    inst->replaceAllUsesWith(result);
    inst->eraseFromParent();
}

void Obfuscator::setError(const std::string& error) {
    errorMessage = error;
}

} // namespace lwanga
