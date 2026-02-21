#ifndef LWANGA_SYMBOL_TABLE_H
#define LWANGA_SYMBOL_TABLE_H

#include "AST.h"
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

namespace lwanga {

enum class SymbolKind {
    Variable,
    Function,
    Struct,
    Constant,
    Parameter
};

struct Symbol {
    std::string name;
    SymbolKind kind;
    std::unique_ptr<Type> type;
    bool isMutable;
    uint32_t definedAtLine;
    
    Symbol(const std::string& n, SymbolKind k, std::unique_ptr<Type> t, 
           bool mut = false, uint32_t line = 0)
        : name(n), kind(k), type(std::move(t)), isMutable(mut), definedAtLine(line) {}
};

class Scope {
public:
    Scope() = default;
    
    // Add a symbol to this scope
    bool define(const std::string& name, std::unique_ptr<Symbol> symbol);
    
    // Look up a symbol in this scope only
    Symbol* lookup(const std::string& name);
    const Symbol* lookup(const std::string& name) const;
    
    // Get all symbols in this scope
    const std::unordered_map<std::string, std::unique_ptr<Symbol>>& getSymbols() const {
        return symbols;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<Symbol>> symbols;
};

class SymbolTable {
public:
    SymbolTable();
    
    // Scope management
    void enterScope();
    void exitScope();
    size_t getCurrentScopeDepth() const { return scopes.size(); }
    
    // Symbol operations
    bool define(const std::string& name, std::unique_ptr<Symbol> symbol);
    Symbol* lookup(const std::string& name);
    const Symbol* lookup(const std::string& name) const;
    Symbol* lookupCurrentScope(const std::string& name);
    
    // Check if symbol exists in current scope
    bool existsInCurrentScope(const std::string& name) const;
    
    // Get current scope
    Scope* getCurrentScope();
    const Scope* getCurrentScope() const;

private:
    std::vector<Scope> scopes;
};

} // namespace lwanga

#endif // LWANGA_SYMBOL_TABLE_H
