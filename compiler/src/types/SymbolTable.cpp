#include "SymbolTable.h"

namespace lwanga {

// Scope implementation

bool Scope::define(const std::string& name, std::unique_ptr<Symbol> symbol) {
    if (symbols.find(name) != symbols.end()) {
        return false; // Symbol already defined in this scope
    }
    symbols[name] = std::move(symbol);
    return true;
}

Symbol* Scope::lookup(const std::string& name) {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    return nullptr;
}

const Symbol* Scope::lookup(const std::string& name) const {
    auto it = symbols.find(name);
    if (it != symbols.end()) {
        return it->second.get();
    }
    return nullptr;
}

// SymbolTable implementation

SymbolTable::SymbolTable() {
    // Start with global scope
    scopes.emplace_back();
}

void SymbolTable::enterScope() {
    scopes.emplace_back();
}

void SymbolTable::exitScope() {
    if (scopes.size() > 1) {
        scopes.pop_back();
    }
}

bool SymbolTable::define(const std::string& name, std::unique_ptr<Symbol> symbol) {
    if (scopes.empty()) {
        return false;
    }
    return scopes.back().define(name, std::move(symbol));
}

Symbol* SymbolTable::lookup(const std::string& name) {
    // Search from innermost to outermost scope
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        Symbol* sym = it->lookup(name);
        if (sym != nullptr) {
            return sym;
        }
    }
    return nullptr;
}

const Symbol* SymbolTable::lookup(const std::string& name) const {
    // Search from innermost to outermost scope
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        const Symbol* sym = it->lookup(name);
        if (sym != nullptr) {
            return sym;
        }
    }
    return nullptr;
}

Symbol* SymbolTable::lookupCurrentScope(const std::string& name) {
    if (scopes.empty()) {
        return nullptr;
    }
    return scopes.back().lookup(name);
}

bool SymbolTable::existsInCurrentScope(const std::string& name) const {
    if (scopes.empty()) {
        return false;
    }
    return scopes.back().lookup(name) != nullptr;
}

Scope* SymbolTable::getCurrentScope() {
    if (scopes.empty()) {
        return nullptr;
    }
    return &scopes.back();
}

const Scope* SymbolTable::getCurrentScope() const {
    if (scopes.empty()) {
        return nullptr;
    }
    return &scopes.back();
}

std::vector<std::string> SymbolTable::getAllSymbols() const {
    std::vector<std::string> result;
    for (const auto& scope : scopes) {
        for (const auto& pair : scope.getSymbols()) {
            result.push_back(pair.first);
        }
    }
    return result;
}

} // namespace lwanga
