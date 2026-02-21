#include "TypeSystem.h"
#include <sstream>
#include <unordered_map>

namespace lwanga {

bool TypeSystem::areTypesEqual(const Type* t1, const Type* t2) {
    if (t1 == nullptr || t2 == nullptr) {
        return t1 == t2;
    }
    
    if (t1->kind != t2->kind) {
        return false;
    }
    
    switch (t1->kind) {
        case TypeKind::U8:
        case TypeKind::U16:
        case TypeKind::U32:
        case TypeKind::U64:
        case TypeKind::Ptr:
            return true;
            
        case TypeKind::Array:
            return t1->arraySize == t2->arraySize &&
                   areTypesEqual(t1->elementType.get(), t2->elementType.get());
                   
        case TypeKind::Struct:
            return t1->structName == t2->structName;
            
        case TypeKind::FunctionPointer:
            if (t1->paramTypes.size() != t2->paramTypes.size()) {
                return false;
            }
            for (size_t i = 0; i < t1->paramTypes.size(); i++) {
                if (!areTypesEqual(t1->paramTypes[i].get(), t2->paramTypes[i].get())) {
                    return false;
                }
            }
            return areTypesEqual(t1->returnType.get(), t2->returnType.get());
    }
    
    return false;
}

bool TypeSystem::areTypesCompatible(const Type* t1, const Type* t2) {
    if (t1 == nullptr || t2 == nullptr) {
        return t1 == t2;
    }
    
    // Exact type equality is always compatible
    if (areTypesEqual(t1, t2)) {
        return true;
    }
    
    // Implicit conversions for numeric types
    // Allow widening conversions: smaller -> larger
    if (isNumericType(t1) && isNumericType(t2)) {
        size_t size1 = sizeOf(t1);
        size_t size2 = sizeOf(t2);
        // Allow implicit widening (u8 -> u16 -> u32 -> u64)
        return size1 <= size2;
    }
    
    // Pointer compatibility
    // Any pointer can be implicitly converted to ptr (generic pointer)
    if (t1->kind == TypeKind::Ptr && t2->kind == TypeKind::Ptr) {
        // Generic ptr is compatible with any pointer
        if (t1->pointeeType == nullptr || t2->pointeeType == nullptr) {
            return true;
        }
        // Typed pointers must match exactly
        return areTypesEqual(t1->pointeeType.get(), t2->pointeeType.get());
    }
    
    // Array to pointer decay
    if (t1->kind == TypeKind::Array && t2->kind == TypeKind::Ptr) {
        if (t2->pointeeType == nullptr) {
            return true; // Array decays to generic ptr
        }
        return areTypesEqual(t1->elementType.get(), t2->pointeeType.get());
    }
    
    // Function pointer compatibility
    if (t1->kind == TypeKind::FunctionPointer && t2->kind == TypeKind::Ptr) {
        return true; // Function pointers can decay to generic ptr
    }
    
    return false;
}

size_t TypeSystem::sizeOf(const Type* type, const std::unordered_map<std::string, StructAST*>* structRegistry) {
    if (type == nullptr) {
        return 0;
    }
    
    switch (type->kind) {
        case TypeKind::U8:
            return 1;
        case TypeKind::U16:
            return 2;
        case TypeKind::U32:
            return 4;
        case TypeKind::U64:
            return 8;
        case TypeKind::Ptr:
            return 8; // 64-bit pointers
        case TypeKind::Array:
            return sizeOf(type->elementType.get(), structRegistry) * type->arraySize;
        case TypeKind::Struct: {
            if (structRegistry) {
                auto it = structRegistry->find(type->structName);
                if (it != structRegistry->end()) {
                    return calculateStructSize(it->second, structRegistry);
                }
            }
            // If struct not found, return 0 to indicate error
            return 0;
        }
        case TypeKind::FunctionPointer:
            return 8; // Function pointers are 64-bit
    }
    
    return 0;
}

size_t TypeSystem::alignOf(const Type* type, const std::unordered_map<std::string, StructAST*>* structRegistry) {
    if (type == nullptr) {
        return 1;
    }
    
    switch (type->kind) {
        case TypeKind::U8:
            return 1;
        case TypeKind::U16:
            return 2;
        case TypeKind::U32:
            return 4;
        case TypeKind::U64:
            return 8;
        case TypeKind::Ptr:
            return 8;
        case TypeKind::Array:
            return alignOf(type->elementType.get(), structRegistry);
        case TypeKind::Struct: {
            if (structRegistry) {
                auto it = structRegistry->find(type->structName);
                if (it != structRegistry->end()) {
                    return calculateStructAlignment(it->second, structRegistry);
                }
            }
            // If struct not found, return 1 (minimal alignment)
            return 1;
        }
        case TypeKind::FunctionPointer:
            return 8;
    }
    
    return 1;
}

size_t TypeSystem::calculateStructSize(StructAST* structDef, const std::unordered_map<std::string, StructAST*>* structRegistry) {
    if (structDef->fields.empty()) {
        return 0;
    }
    
    size_t totalSize = 0;
    size_t maxAlignment = 1;
    
    // Calculate size with proper alignment for each field
    for (const auto& field : structDef->fields) {
        size_t fieldSize = sizeOf(field.type.get(), structRegistry);
        size_t fieldAlign = alignOf(field.type.get(), structRegistry);
        
        // Track maximum alignment for final struct alignment
        if (fieldAlign > maxAlignment) {
            maxAlignment = fieldAlign;
        }
        
        // Align current offset to field's alignment requirement
        if (!structDef->isPacked && totalSize % fieldAlign != 0) {
            totalSize += fieldAlign - (totalSize % fieldAlign);
        }
        
        totalSize += fieldSize;
    }
    
    // Align total size to struct's alignment (unless packed)
    if (!structDef->isPacked && totalSize % maxAlignment != 0) {
        totalSize += maxAlignment - (totalSize % maxAlignment);
    }
    
    return totalSize;
}

size_t TypeSystem::calculateStructAlignment(StructAST* structDef, const std::unordered_map<std::string, StructAST*>* structRegistry) {
    if (structDef->fields.empty()) {
        return 1;
    }
    
    // Packed structs have alignment of 1
    if (structDef->isPacked) {
        return 1;
    }
    
    // Struct alignment is the maximum alignment of its fields
    size_t maxAlignment = 1;
    for (const auto& field : structDef->fields) {
        size_t fieldAlign = alignOf(field.type.get(), structRegistry);
        if (fieldAlign > maxAlignment) {
            maxAlignment = fieldAlign;
        }
    }
    
    return maxAlignment;
}

bool TypeSystem::isNumericType(const Type* type) {
    if (type == nullptr) {
        return false;
    }
    
    return type->kind == TypeKind::U8 ||
           type->kind == TypeKind::U16 ||
           type->kind == TypeKind::U32 ||
           type->kind == TypeKind::U64;
}

bool TypeSystem::isPointerType(const Type* type) {
    return type != nullptr && type->kind == TypeKind::Ptr;
}

bool TypeSystem::canCast(const Type* from, const Type* to) {
    if (from == nullptr || to == nullptr) {
        return false;
    }
    
    // Numeric types can be cast to each other
    if (isNumericType(from) && isNumericType(to)) {
        return true;
    }
    
    // Pointers can be cast to numeric types and vice versa
    if ((isPointerType(from) && isNumericType(to)) ||
        (isNumericType(from) && isPointerType(to))) {
        return true;
    }
    
    // Pointers can be cast to pointers
    if (isPointerType(from) && isPointerType(to)) {
        return true;
    }
    
    return false;
}

std::unique_ptr<Type> TypeSystem::getBinaryOpResultType(const Type* left, const Type* right, BinaryOp op) {
    if (left == nullptr || right == nullptr) {
        return nullptr;
    }
    
    switch (op) {
        case BinaryOp::Add:
        case BinaryOp::Sub:
            // Pointer arithmetic: ptr + u64 -> ptr, ptr - ptr -> u64
            if (isPointerType(left) && isNumericType(right)) {
                return Type::makePtr();
            }
            if (isPointerType(left) && isPointerType(right) && op == BinaryOp::Sub) {
                return Type::makeU64();
            }
            // Numeric + Numeric -> Numeric (use left type)
            if (isNumericType(left) && isNumericType(right)) {
                return cloneType(left);
            }
            break;
            
        case BinaryOp::Mul:
        case BinaryOp::Div:
        case BinaryOp::Mod:
        case BinaryOp::BitAnd:
        case BinaryOp::BitOr:
        case BinaryOp::BitXor:
        case BinaryOp::LeftShift:
        case BinaryOp::RightShift:
            // Numeric operations require matching types
            if (isNumericType(left) && isNumericType(right)) {
                return cloneType(left);
            }
            break;
            
        case BinaryOp::Equal:
        case BinaryOp::NotEqual:
        case BinaryOp::Less:
        case BinaryOp::Greater:
        case BinaryOp::LessEqual:
        case BinaryOp::GreaterEqual:
            // Comparisons return u64 (boolean)
            if ((isNumericType(left) && isNumericType(right)) ||
                (isPointerType(left) && isPointerType(right))) {
                return Type::makeU64();
            }
            break;
    }
    
    return nullptr;
}

std::unique_ptr<Type> TypeSystem::getUnaryOpResultType(const Type* operand, UnaryOp op) {
    if (operand == nullptr) {
        return nullptr;
    }
    
    switch (op) {
        case UnaryOp::Negate:
        case UnaryOp::Not:
            if (isNumericType(operand)) {
                return cloneType(operand);
            }
            break;
            
        case UnaryOp::Deref:
            // Dereferencing a pointer gives the pointed-to type
            if (isPointerType(operand)) {
                if (operand->pointeeType) {
                    return cloneType(operand->pointeeType.get());
                }
                // If pointee type not tracked, return u64 as default
                return Type::makeU64();
            }
            break;
            
        case UnaryOp::AddressOf:
            // Taking address of anything gives a pointer to that type
            return Type::makePtrTo(cloneType(operand));
    }
    
    return nullptr;
}

std::unique_ptr<Type> TypeSystem::cloneType(const Type* type) {
    if (type == nullptr) {
        return nullptr;
    }
    
    auto cloned = std::make_unique<Type>(type->kind);
    
    if (type->elementType) {
        cloned->elementType = cloneType(type->elementType.get());
    }
    
    if (type->pointeeType) {
        cloned->pointeeType = cloneType(type->pointeeType.get());
    }
    
    cloned->arraySize = type->arraySize;
    cloned->structName = type->structName;
    
    for (const auto& paramType : type->paramTypes) {
        cloned->paramTypes.push_back(cloneType(paramType.get()));
    }
    
    if (type->returnType) {
        cloned->returnType = cloneType(type->returnType.get());
    }
    
    return cloned;
}

std::string TypeSystem::typeToString(const Type* type) {
    if (type == nullptr) {
        return "unknown";
    }
    
    switch (type->kind) {
        case TypeKind::U8: return "u8";
        case TypeKind::U16: return "u16";
        case TypeKind::U32: return "u32";
        case TypeKind::U64: return "u64";
        case TypeKind::Ptr: return "ptr";
        case TypeKind::Array: {
            std::ostringstream oss;
            oss << "[" << typeToString(type->elementType.get()) << "; " << type->arraySize << "]";
            return oss.str();
        }
        case TypeKind::Struct:
            return type->structName;
        case TypeKind::FunctionPointer: {
            std::ostringstream oss;
            oss << "fn(";
            for (size_t i = 0; i < type->paramTypes.size(); i++) {
                if (i > 0) oss << ", ";
                oss << typeToString(type->paramTypes[i].get());
            }
            oss << ") -> " << typeToString(type->returnType.get());
            return oss.str();
        }
    }
    
    return "unknown";
}

} // namespace lwanga
