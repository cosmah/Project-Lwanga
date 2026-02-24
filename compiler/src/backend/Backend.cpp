#include "Backend.h"
#include "Obfuscator.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Verifier.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Transforms/InstCombine/InstCombine.h>
#include <llvm/Transforms/Scalar.h>
#include <llvm/Transforms/Scalar/GVN.h>
#include <llvm/Transforms/Utils.h>
#include <llvm/Transforms/IPO.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/ExecutionEngine/MCJIT.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm-c/TargetMachine.h>
#include <llvm/Transforms/Utils/Local.h>
#include <system_error>
#include <cstdlib>

namespace lwanga {

Backend::Backend(llvm::Module* mod, unsigned opt)
    : module(mod), optLevel(opt), pic(false), debugInfo(false), enableObfuscation(false) {
    
    // Use native target by default
    char* tripleStr = LLVMGetDefaultTargetTriple();
    targetTriple = std::string(tripleStr);
    free(tripleStr);  // LLVMGetDefaultTargetTriple returns malloc'd string
    module->setTargetTriple(targetTriple);
    
    // Initialize target machine
    initializeTargetMachine();
}

Backend::~Backend() {}

bool Backend::setTargetTriple(const std::string& triple) {
    // Validate supported targets
    // Supported: x86_64-linux, aarch64-linux, x86_64-windows, aarch64-windows
    bool isSupported = false;
    
    // Check for supported architectures and platforms
    if (triple.find("x86_64") != std::string::npos || triple.find("x86-64") != std::string::npos) {
        // x86_64 architecture
        if (triple.find("linux") != std::string::npos || triple.find("windows") != std::string::npos) {
            isSupported = true;
        }
    } else if (triple.find("aarch64") != std::string::npos || triple.find("arm64") != std::string::npos) {
        // ARM64 architecture
        if (triple.find("linux") != std::string::npos || triple.find("windows") != std::string::npos) {
            isSupported = true;
        }
    }
    
    if (!isSupported) {
        setError("Unsupported target triple: " + triple + 
                 "\nSupported targets: x86_64-linux-gnu, aarch64-linux-gnu, x86_64-windows, aarch64-windows");
        return false;
    }
    
    targetTriple = triple;
    module->setTargetTriple(triple);
    
    // Reinitialize target machine with new triple
    return initializeTargetMachine();
}

void Backend::setPositionIndependent(bool isPIC) {
    pic = isPIC;
}

void Backend::setDebugInfo(bool debug) {
    debugInfo = debug;
}

void Backend::setObfuscate(bool obfuscate) {
    enableObfuscation = obfuscate;
}

void Backend::obfuscate() {
    if (!enableObfuscation) {
        return;
    }
    
    // In LLVM 18+, the legacy DemoteRegisterToMemory pass is removed.
    // We manually demote instructions to the stack to achieve the same result
    // for the control flow flattener.
    for (auto& func : *module) {
        if (!func.isDeclaration()) {
            std::vector<llvm::PHINode*> phis;
            std::vector<llvm::Instruction*> toDemote;
            
            for (auto& bb : func) {
                for (auto& inst : bb) {
                    if (auto* phi = llvm::dyn_cast<llvm::PHINode>(&inst)) {
                        phis.push_back(phi);
                    } else if (inst.getType()->isVoidTy() || llvm::isa<llvm::AllocaInst>(&inst)) {
                        continue;
                    } else if (inst.isUsedOutsideOfBlock(&bb) || !inst.isTerminator()) {
                        // For the control flow flattener, we want most things in memory
                        toDemote.push_back(&inst);
                    }
                }
            }
            
            for (auto* phi : phis) {
                llvm::DemotePHIToStack(phi);
            }
            for (auto* inst : toDemote) {
                // Check if still has users after PHI demotion
                if (!inst->use_empty()) {
                    llvm::DemoteRegToStack(*inst);
                }
            }
        }
    }
    
    Obfuscator obfuscator(module);
    obfuscator.obfuscate();
    
    if (obfuscator.hasErrors()) {
        setError("Obfuscation failed: " + obfuscator.getError());
    }
}

bool Backend::initializeTargetMachine() {
    // Initialize targets
    llvm::InitializeAllTargetInfos();
    llvm::InitializeAllTargets();
    llvm::InitializeAllTargetMCs();
    llvm::InitializeAllAsmParsers();
    llvm::InitializeAllAsmPrinters();
    
    // Get the target
    std::string error;
    const llvm::Target* target = llvm::TargetRegistry::lookupTarget(targetTriple, error);
    
    if (!target) {
        setError("Failed to lookup target: " + error);
        return false;
    }
    
    // Set up target options
    llvm::TargetOptions options;
    
    // Set relocation model
    llvm::Reloc::Model relocModel = pic ? llvm::Reloc::PIC_ : llvm::Reloc::Static;
    
    // Create target machine
    targetMachine.reset(target->createTargetMachine(
        targetTriple,
        "generic",  // CPU
        "",         // Features
        options,
        relocModel,
        llvm::CodeModel::Small,
        llvm::CodeGenOptLevel::None
    ));
    
    if (!targetMachine) {
        setError("Failed to create target machine");
        return false;
    }
    
    // Set data layout
    module->setDataLayout(targetMachine->createDataLayout());
    
    return true;
}

void Backend::optimize() {
    // Target machine should already be initialized in constructor
    if (!targetMachine) {
        setError("Target machine not initialized");
        return;
    }
    
    runOptimizationPasses();
}

void Backend::runOptimizationPasses() {
    // Create pass managers
    llvm::legacy::PassManager modulePassManager;
    llvm::legacy::FunctionPassManager functionPassManager(module);
    
    // Configure optimization based on level
    if (optLevel == 0) {
        // -O0: No optimization, just verify
        modulePassManager.add(llvm::createVerifierPass());
    } else if (optLevel >= 1) {
        // -O1+: Basic and advanced optimizations
        // Function-level optimizations
        functionPassManager.add(llvm::createPromoteMemoryToRegisterPass());
        functionPassManager.add(llvm::createInstructionCombiningPass());
        functionPassManager.add(llvm::createReassociatePass());
        functionPassManager.add(llvm::createGVNPass());
        functionPassManager.add(llvm::createCFGSimplificationPass());
        
        if (optLevel >= 2) {
            // Additional optimizations for -O2 and -O3
            functionPassManager.add(llvm::createEarlyCSEPass());
        }
        
        // Module-level optimizations
        modulePassManager.add(llvm::createDeadCodeEliminationPass());
        
        modulePassManager.add(llvm::createVerifierPass());
    }
    
    // Run function passes
    functionPassManager.doInitialization();
    for (auto& func : *module) {
        if (!func.isDeclaration()) {
            functionPassManager.run(func);
        }
    }
    functionPassManager.doFinalization();
    
    // Run module passes
    modulePassManager.run(*module);
}

bool Backend::generateObjectFile(const std::string& filename) {
    if (!initializeTargetMachine()) {
        return false;
    }
    
    // Open output file
    std::error_code ec;
    llvm::raw_fd_ostream dest(filename, ec, llvm::sys::fs::OF_None);
    
    if (ec) {
        setError("Could not open file: " + ec.message());
        return false;
    }
    
    // Create pass manager for code generation
    llvm::legacy::PassManager passManager;
    
    // Add passes to emit object code
    if (targetMachine->addPassesToEmitFile(passManager, dest, nullptr,
                                          llvm::CodeGenFileType::ObjectFile)) {
        setError("TargetMachine can't emit object file");
        return false;
    }
    
    // Run the passes
    passManager.run(*module);
    dest.flush();
    
    return true;
}

bool Backend::generateExecutable(const std::string& filename) {
    // First generate object file
    std::string objFile = filename + ".o";
    if (!generateObjectFile(objFile)) {
        return false;
    }
    
    // Link the object file into an executable
    std::string linkerCmd;
    std::string linkerPath;
    
    // 1. Try to find a suitable linker
    std::vector<std::string> linkers;
    if (targetTriple.find("linux") != std::string::npos) {
        linkers = {"ld.lld-18", "ld.lld", "ld"};
    } else if (targetTriple.find("windows") != std::string::npos) {
        linkers = {"lld-link-18", "lld-link"};
    }
    
    bool foundLinker = false;
    for (const auto& l : linkers) {
        if (std::system(("which " + l + " > /dev/null 2>&1").c_str()) == 0) {
            linkerPath = l;
            foundLinker = true;
            break;
        }
    }
    
    if (!foundLinker) {
        setError("No suitable linker found (tried: " + (linkers.empty() ? "none" : linkers[0]) + "). Please install binutils or lld.");
        std::remove(objFile.c_str());
        return false;
    }
    
    // 2. Construct linker command
    if (targetTriple.find("linux") != std::string::npos) {
        linkerCmd = linkerPath + " -o " + filename + " " + objFile;
        
        // Add architecture-specific flags
        if (targetTriple.find("x86_64") != std::string::npos) {
            linkerCmd += " -m elf_x86_64";
        } else if (targetTriple.find("aarch64") != std::string::npos) {
            linkerCmd += " -m aarch64linux";
        }
        
        // Static linking, no standard library for minimal footprint
        linkerCmd += " -static -nostdlib";
        
        // Try _start as entry point, fallback to main
        // We do this by creating a command that tries both
        std::string baseCmd = linkerCmd;
        linkerCmd = "(" + baseCmd + " -e _start || " + baseCmd + " -e main) 2>/dev/null";
        
    } else if (targetTriple.find("windows") != std::string::npos) {
        linkerCmd = linkerPath + " /OUT:" + filename + ".exe " + objFile + 
                   " /SUBSYSTEM:CONSOLE /ENTRY:main /FIXED /NODEFAULTLIB";
    }
    
    // Execute linker
    int result = std::system(linkerCmd.c_str());
    
    // Clean up object file
    std::remove(objFile.c_str());
    
    if (result != 0) {
        setError("Linking failed with exit code: " + std::to_string(result));
        return false;
    }
    
    return true;
}

int Backend::executeJIT() {
    // Initialize JIT
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    
    // Verify module
    std::string verifyError;
    llvm::raw_string_ostream verifyStream(verifyError);
    if (llvm::verifyModule(*module, &verifyStream)) {
        setError("Module verification failed: " + verifyError);
        return -1;
    }
    
    // Clone the module for JIT execution (ExecutionEngine takes ownership)
    // This prevents double-free when IRGenerator's module is destroyed
    std::unique_ptr<llvm::Module> jitModule = llvm::CloneModule(*module);
    
    // Create execution engine
    std::string error;
    llvm::ExecutionEngine* engine = llvm::EngineBuilder(std::move(jitModule))
        .setErrorStr(&error)
        .setEngineKind(llvm::EngineKind::JIT)
        .create();
    
    if (!engine) {
        setError("Failed to create execution engine: " + error);
        return -1;
    }
    
    // Find main function
    llvm::Function* mainFunc = engine->FindFunctionNamed("main");
    if (!mainFunc) {
        setError("No main function found");
        delete engine;
        return -1;
    }
    
    // Execute main function
    std::vector<llvm::GenericValue> args;
    llvm::GenericValue result = engine->runFunction(mainFunc, args);
    
    // Get result before cleaning up
    int exitCode = static_cast<int>(result.IntVal.getZExtValue());
    
    // Clean up
    delete engine;
    
    // Return result as integer
    return exitCode;
}

void Backend::setError(const std::string& error) {
    errorMessage = error;
}

} // namespace lwanga
