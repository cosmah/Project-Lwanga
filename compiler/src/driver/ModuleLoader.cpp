#include "ModuleLoader.h"
#include "Lexer.h"
#include "Preprocessor.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace lwanga {

ModuleLoader::ModuleLoader() {}

std::unique_ptr<ProgramAST> ModuleLoader::loadModule(const std::string& mainFilePath) {
    // Clear state
    loadingModules.clear();
    loadedModules.clear();
    modules.clear();
    errors.clear();
    
    // Normalize the main file path
    std::filesystem::path mainPath = std::filesystem::absolute(mainFilePath);
    std::string mainPathStr = mainPath.string();
    
    // Load the main module and all its dependencies
    auto mainModule = loadSingleModule(mainPathStr, "");
    if (!mainModule) {
        return nullptr;
    }
    
    // Collect all loaded modules
    std::vector<std::unique_ptr<ProgramAST>> allModules;
    allModules.push_back(std::move(mainModule));
    
    // Add all other loaded modules
    for (auto& [path, module] : modules) {
        if (path != mainPathStr) {
            allModules.push_back(std::move(module));
        }
    }
    
    // Merge all modules into one program
    return mergeModules(allModules);
}

std::unique_ptr<ProgramAST> ModuleLoader::loadSingleModule(const std::string& filePath,
                                                            const std::string& importedFrom) {
    // Check for circular imports
    if (loadingModules.find(filePath) != loadingModules.end()) {
        reportError("Circular import detected: " + filePath + 
                   (importedFrom.empty() ? "" : " (imported from " + importedFrom + ")"));
        return nullptr;
    }
    
    // Check if already loaded
    if (loadedModules.find(filePath) != loadedModules.end()) {
        // Return a copy of the already loaded module
        return nullptr;  // Already processed, skip
    }
    
    // Mark as currently loading
    loadingModules.insert(filePath);
    
    // Read the source file
    std::string source = readFile(filePath);
    if (source.empty() && !errors.empty()) {
        return nullptr;
    }
    
    // Preprocess the source
    Preprocessor preprocessor(source);
    std::string preprocessedSource = preprocessor.process();
    
    // Create lexer and parser
    Lexer lexer(preprocessedSource);
    Parser parser(lexer);
    auto program = parser.parse();
    
    if (parser.hasErrors()) {
        for (const auto& error : parser.getErrors()) {
            reportError(filePath + ": Line " + std::to_string(error.line) + 
                       ": " + error.message);
        }
        return nullptr;
    }
    
    // Process imports recursively
    for (const auto& import : program->imports) {
        std::string importPath = resolveModulePath(import->path, filePath);
        
        auto importedModule = loadSingleModule(importPath, filePath);
        if (importedModule) {
            modules[importPath] = std::move(importedModule);
        } else if (hasErrors()) {
            // If there was an error loading the import, propagate it
            loadingModules.erase(filePath);
            return nullptr;
        }
    }
    
    // Mark as fully loaded
    loadingModules.erase(filePath);
    loadedModules.insert(filePath);
    
    return program;
}

std::string ModuleLoader::resolveModulePath(const std::string& importPath,
                                            const std::string& importingFile) {
    // Get the directory of the importing file
    std::filesystem::path importingPath(importingFile);
    std::filesystem::path importingDir = importingPath.parent_path();
    
    // Resolve the import path relative to the importing file's directory
    std::filesystem::path resolvedPath = importingDir / importPath;
    
    // Normalize the path (resolve .., ., etc.)
    resolvedPath = std::filesystem::absolute(resolvedPath);
    
    return resolvedPath.string();
}

std::unique_ptr<ProgramAST> ModuleLoader::mergeModules(
    std::vector<std::unique_ptr<ProgramAST>>& moduleList) {
    
    if (moduleList.empty()) {
        reportError("No modules to merge");
        return nullptr;
    }
    
    // Use the first module as the base
    auto mergedProgram = std::move(moduleList[0]);
    
    // Merge all other modules into it
    for (size_t i = 1; i < moduleList.size(); i++) {
        auto& module = moduleList[i];
        if (!module) continue;
        
        // Merge structs
        for (auto& structDef : module->structs) {
            mergedProgram->structs.push_back(std::move(structDef));
        }
        
        // Merge functions
        for (auto& func : module->functions) {
            mergedProgram->functions.push_back(std::move(func));
        }
        
        // Merge constants
        for (auto& constant : module->constants) {
            mergedProgram->constants.push_back(std::move(constant));
        }
        
        // Note: We don't merge imports as they've already been processed
    }
    
    return mergedProgram;
}

void ModuleLoader::reportError(const std::string& message) {
    errors.push_back(message);
}

std::string ModuleLoader::readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        reportError("Failed to open file: " + path);
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

} // namespace lwanga
