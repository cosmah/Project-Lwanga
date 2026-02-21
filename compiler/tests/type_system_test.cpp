#include "TypeSystem.h"
#include "AST.h"
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace lwanga;

void testPrimitiveSizes() {
    assert(TypeSystem::sizeOf(Type::makeU8().get()) == 1);
    assert(TypeSystem::sizeOf(Type::makeU16().get()) == 2);
    assert(TypeSystem::sizeOf(Type::makeU32().get()) == 4);
    assert(TypeSystem::sizeOf(Type::makeU64().get()) == 8);
    assert(TypeSystem::sizeOf(Type::makePtr().get()) == 8);
    std::cout << "✓ Primitive type sizes correct\n";
}

void testPrimitiveAlignment() {
    assert(TypeSystem::alignOf(Type::makeU8().get()) == 1);
    assert(TypeSystem::alignOf(Type::makeU16().get()) == 2);
    assert(TypeSystem::alignOf(Type::makeU32().get()) == 4);
    assert(TypeSystem::alignOf(Type::makeU64().get()) == 8);
    assert(TypeSystem::alignOf(Type::makePtr().get()) == 8);
    std::cout << "✓ Primitive type alignments correct\n";
}

void testArraySize() {
    auto arrayType = std::make_unique<Type>(TypeKind::Array);
    arrayType->elementType = Type::makeU32();
    arrayType->arraySize = 10;
    
    assert(TypeSystem::sizeOf(arrayType.get()) == 40); // 10 * 4
    std::cout << "✓ Array size calculation correct\n";
}

void testStructSize() {
    // Create a simple struct: { u8, u32, u64 }
    std::vector<StructField> fields;
    fields.push_back(StructField("a", Type::makeU8()));
    fields.push_back(StructField("b", Type::makeU32()));
    fields.push_back(StructField("c", Type::makeU64()));
    
    auto structDef = std::make_unique<StructAST>("TestStruct", std::move(fields), false);
    
    std::unordered_map<std::string, StructAST*> registry;
    registry["TestStruct"] = structDef.get();
    
    auto structType = std::make_unique<Type>(TypeKind::Struct);
    structType->structName = "TestStruct";
    
    // Expected layout with alignment:
    // u8 a:     offset 0, size 1
    // padding:  3 bytes (align to u32)
    // u32 b:    offset 4, size 4
    // u64 c:    offset 8, size 8
    // Total:    16 bytes (aligned to 8)
    size_t size = TypeSystem::sizeOf(structType.get(), &registry);
    assert(size == 16);
    std::cout << "✓ Struct size calculation with alignment correct\n";
}

void testPackedStructSize() {
    // Create a packed struct: { u8, u32, u64 }
    std::vector<StructField> fields;
    fields.push_back(StructField("a", Type::makeU8()));
    fields.push_back(StructField("b", Type::makeU32()));
    fields.push_back(StructField("c", Type::makeU64()));
    
    auto structDef = std::make_unique<StructAST>("PackedStruct", std::move(fields), true);
    
    std::unordered_map<std::string, StructAST*> registry;
    registry["PackedStruct"] = structDef.get();
    
    auto structType = std::make_unique<Type>(TypeKind::Struct);
    structType->structName = "PackedStruct";
    
    // Packed layout (no padding):
    // u8 a:  offset 0, size 1
    // u32 b: offset 1, size 4
    // u64 c: offset 5, size 8
    // Total: 13 bytes
    size_t size = TypeSystem::sizeOf(structType.get(), &registry);
    assert(size == 13);
    std::cout << "✓ Packed struct size calculation correct\n";
}

void testStructAlignment() {
    // Create struct with u64 field
    std::vector<StructField> fields;
    fields.push_back(StructField("x", Type::makeU64()));
    
    auto structDef = std::make_unique<StructAST>("AlignedStruct", std::move(fields), false);
    
    std::unordered_map<std::string, StructAST*> registry;
    registry["AlignedStruct"] = structDef.get();
    
    auto structType = std::make_unique<Type>(TypeKind::Struct);
    structType->structName = "AlignedStruct";
    
    // Alignment should be 8 (max field alignment)
    size_t align = TypeSystem::alignOf(structType.get(), &registry);
    assert(align == 8);
    std::cout << "✓ Struct alignment calculation correct\n";
}

void testNestedStruct() {
    // Create inner struct: { u16, u16 }
    std::vector<StructField> innerFields;
    innerFields.push_back(StructField("a", Type::makeU16()));
    innerFields.push_back(StructField("b", Type::makeU16()));
    auto innerStruct = std::make_unique<StructAST>("Inner", std::move(innerFields), false);
    
    // Create outer struct: { u8, Inner, u8 }
    std::vector<StructField> outerFields;
    outerFields.push_back(StructField("x", Type::makeU8()));
    
    auto innerType = std::make_unique<Type>(TypeKind::Struct);
    innerType->structName = "Inner";
    outerFields.push_back(StructField("inner", std::move(innerType)));
    outerFields.push_back(StructField("y", Type::makeU8()));
    
    auto outerStruct = std::make_unique<StructAST>("Outer", std::move(outerFields), false);
    
    std::unordered_map<std::string, StructAST*> registry;
    registry["Inner"] = innerStruct.get();
    registry["Outer"] = outerStruct.get();
    
    auto outerType = std::make_unique<Type>(TypeKind::Struct);
    outerType->structName = "Outer";
    
    // Expected layout:
    // u8 x:      offset 0, size 1
    // padding:   1 byte (align to u16)
    // Inner:     offset 2, size 4 (two u16s)
    // u8 y:      offset 6, size 1
    // padding:   1 byte (align to 2)
    // Total:     8 bytes
    size_t size = TypeSystem::sizeOf(outerType.get(), &registry);
    assert(size == 8);
    std::cout << "✓ Nested struct size calculation correct\n";
}

int main() {
    std::cout << "Running TypeSystem tests...\n\n";
    
    testPrimitiveSizes();
    testPrimitiveAlignment();
    testArraySize();
    testStructSize();
    testPackedStructSize();
    testStructAlignment();
    testNestedStruct();
    
    std::cout << "\n✓ All TypeSystem tests passed!\n";
    return 0;
}
