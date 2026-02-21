#ifndef LWANGA_TYPE_SYSTEM_H
#define LWANGA_TYPE_SYSTEM_H

#include "AST.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace lwanga {

// Type utilities and operations
class TypeSystem {
public:
    // Type equality
    static bool areTypesEqual(const Type* t1, const Type* t2);
    
    // Type compatibility (for assignments and operations)
    static bool areTypesCompatible(const Type* t1, const Type* t2);
    
    // Size calculation in bytes (requires struct registry for struct types)
    static size_t sizeOf(const Type* type, const std::unordered_map<std::string, StructAST*>* structRegistry = nullptr);
    
    // Alignment calculation in bytes (requires struct registry for struct types)
    static size_t alignOf(const Type* type, const std::unordered_map<std::string, StructAST*>* structRegistry = nullptr);
    
    // Check if type is numeric
    static bool isNumericType(const Type* type);
    
    // Check if type is pointer
    static bool isPointerType(const Type* type);
    
    // Check if cast is valid
    static bool canCast(const Type* from, const Type* to);
    
    // Get result type of binary operation
    static std::unique_ptr<Type> getBinaryOpResultType(const Type* left, const Type* right, BinaryOp op);
    
    // Get result type of unary operation
    static std::unique_ptr<Type> getUnaryOpResultType(const Type* operand, UnaryOp op);
    
    // Clone a type
    static std::unique_ptr<Type> cloneType(const Type* type);
    
    // Type to string for error messages
    static std::string typeToString(const Type* type);

private:
    // Calculate struct size with proper field layout
    static size_t calculateStructSize(StructAST* structDef, const std::unordered_map<std::string, StructAST*>* structRegistry);
    
    // Calculate struct alignment
    static size_t calculateStructAlignment(StructAST* structDef, const std::unordered_map<std::string, StructAST*>* structRegistry);
};

} // namespace lwanga

#endif // LWANGA_TYPE_SYSTEM_H
