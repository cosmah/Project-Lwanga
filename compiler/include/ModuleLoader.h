#ifndef LWANGA_MODULE_LOADER_H
#define LWANGA_MODULE_LOADER_H

#include "AST.h"
#include "Parser.h"
#include "TypeChecker.h"
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <filesystem>

namespace lwanga {

// Module loader handles importing and linking multiple source files
class ModuleLoader {
public:
    ModuleLoader();
    
    // Load a module and all its dependencies
    // Returns the merged program AST or nullptr on error
    std::unique_ptr<ProgramAST> loadModule(const std::string& mainFilePath);
    
    // Check if there were errors during loading
    bool hasErrors() const { return !errors.empty(); }
    
    // Get error messages
    const std::vector<std::string>& getErrors() const { return errors; }

private:
    // Track loaded modules to prevent circular imports
    std::unordered_set<std::string> loadingModules;  // Currently being loaded
    std::unordered_set<std::string> loadedModules;   // Fully loaded
    
    // Store parsed modules
    std::unordered_map<std::string, std::unique_ptr<ProgramAST>> modules;
    
    // Error messages
    std::vector<std::string> errors;
    
    // Load a single module file
    std::unique_ptr<ProgramAST> loadSingleModule(const std::string& filePath, 
                                                  const std::string& importedFrom);
    
    // Resolve module path relative to importing file
    std::string resolveModulePath(const std::string& importPath, 
                                   const std::string& importingFile);
    
    // Merge multiple program ASTs into one
    std::unique_ptr<ProgramAST> mergeModules(std::vector<std::unique_ptr<ProgramAST>>& moduleList);
    
    // Report error
    void reportError(const std::string& message);
    
    // Read file contents
    std::string readFile(const std::string& path);
};

} // namespace lwanga

#endif // LWANGA_MODULE_LOADER_H
