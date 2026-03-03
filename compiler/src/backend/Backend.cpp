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
#include <cstring>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

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
    std::vector<std::string> linkers;
    if (targetTriple.find("linux") != std::string::npos) {
        linkers = {"ld.lld-18", "ld.lld", "ld"};
    } else if (targetTriple.find("windows") != std::string::npos) {
        linkers = {"lld-link-18", "lld-link"};
    }

    // Platform-aware linker discovery
#ifdef _WIN32
    char fullPath[MAX_PATH];
    bool foundLinker = false;
    for (const auto& l : linkers) {
        if (SearchPathA(nullptr, l.c_str(), ".exe", MAX_PATH, fullPath, nullptr)) {
            linkerPath = std::string(fullPath);
            foundLinker = true;
            break;
        }
    }
#else
    bool foundLinker = false;
    for (const auto& l : linkers) {
        std::string path = l;
        if (access(path.c_str(), X_OK) == 0) {
            linkerPath = path;
            foundLinker = true;
            break;
        }
        // Search $PATH
        const char* envPath = getenv("PATH");
        if (envPath) {
            std::stringstream ss(envPath);
            std::string segment;
            char sep = ':';
            while (std::getline(ss, segment, sep)) {
                std::string candidate = segment + "/" + l;
                if (access(candidate.c_str(), X_OK) == 0) {
                    linkerPath = candidate;
                    foundLinker = true;
                    break;
                }
            }
        }
        if (foundLinker) break;
    }
#endif
    if (!foundLinker) {
        setError("No suitable linker found (tried: " + (linkers.empty() ? "none" : linkers[0]) + "). Please install binutils or lld.");
        std::remove(objFile.c_str());
        return false;
    }

    // Build argument vector
    std::vector<std::string> args;
    if (targetTriple.find("linux") != std::string::npos) {
        args.push_back(linkerPath);
        args.push_back("-o");
        args.push_back(filename);
        args.push_back(objFile);
        if (targetTriple.find("x86_64") != std::string::npos) {
            args.push_back("-m");
            args.push_back("elf_x86_64");
        } else if (targetTriple.find("aarch64") != std::string::npos) {
            args.push_back("-m");
            args.push_back("aarch64linux");
        }
        args.push_back("-static");
        args.push_back("-nostdlib");
        // Entry point: try _start, fallback to main
        args.push_back("-e");
        args.push_back("_start");
    } else if (targetTriple.find("windows") != std::string::npos) {
        args.push_back(linkerPath);
        args.push_back("/OUT:" + filename + ".exe");
        args.push_back(objFile);
        args.push_back("/SUBSYSTEM:CONSOLE");
        args.push_back("/ENTRY:main");
        args.push_back("/FIXED");
        args.push_back("/NODEFAULTLIB");
    }

    // Convert args to char* array
    std::vector<char*> argv;
    for (auto& arg : args) argv.push_back(const_cast<char*>(arg.c_str()));
    argv.push_back(nullptr);

    // Spawn process
#ifdef _WIN32
    std::string cmdLine;
    for (size_t i = 0; i < args.size(); ++i) {
        if (i > 0) cmdLine += " ";
        std::string arg = args[i];
        bool needsQuotes = arg.empty() || arg.find_first_of(" \t\n\v\"") != std::string::npos;
        if (needsQuotes) {
            cmdLine += '"';
            for (size_t j = 0; j < arg.length(); ++j) {
                size_t backslashes = 0;
                while (j < arg.length() && arg[j] == '\\') {
                    backslashes++;
                    j++;
                }
                if (j == arg.length()) {
                    for (size_t k = 0; k < backslashes * 2; ++k) cmdLine += '\\';
                } else if (arg[j] == '"') {
                    for (size_t k = 0; k < backslashes * 2 + 1; ++k) cmdLine += '\\';
                    cmdLine += '"';
                } else {
                    for (size_t k = 0; k < backslashes; ++k) cmdLine += '\\';
                    cmdLine += arg[j];
                }
            }
            cmdLine += '"';
        } else {
            cmdLine += arg;
        }
    }
    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    BOOL success = CreateProcessA(nullptr, const_cast<char*>(cmdLine.c_str()), nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi);
    if (!success) {
        setError("Linker process spawn failed");
        std::remove(objFile.c_str());
        return false;
    }
    WaitForSingleObject(pi.hProcess, INFINITE);
    DWORD exitCode = 0;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    int result = exitCode;
#else
    pid_t pid = fork();
    if (pid == 0) {
        execvp(argv[0], argv.data());
        _exit(127);
    } else if (pid < 0) {
        setError("Failed to fork linker process: " + std::string(strerror(errno)));
        std::remove(objFile.c_str());
        return false;
    }
    int status = 0;
    waitpid(pid, &status, 0);
    int result = WIFEXITED(status) ? WEXITSTATUS(status) : status;
#endif
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
