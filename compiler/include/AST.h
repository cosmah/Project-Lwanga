#ifndef LWANGA_AST_H
#define LWANGA_AST_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace lwanga {

// Source location for AST nodes
struct SourceLocation {
    uint32_t line;
    uint32_t column;
    
    SourceLocation(uint32_t l = 0, uint32_t c = 0) : line(l), column(c) {}
};

// Forward declarations for visitor pattern
class ASTVisitor;
class ExprAST;
class StmtAST;

// Binary operators
enum class BinaryOp {
    Add, Sub, Mul, Div, Mod,
    BitAnd, BitOr, BitXor,
    LeftShift, RightShift,
    Equal, NotEqual, Less, Greater, LessEqual, GreaterEqual
};

// Unary operators
enum class UnaryOp {
    Negate, Not, Deref, AddressOf
};

// Type representation
enum class TypeKind {
    U8, U16, U32, U64, Ptr, Array, Struct, FunctionPointer, Register
};

struct Type {
    TypeKind kind;
    std::unique_ptr<Type> elementType;  // For pointers and arrays
    std::unique_ptr<Type> pointeeType;  // For pointers (what they point to)
    size_t arraySize;                    // For arrays
    std::string structName;              // For structs
    std::vector<std::unique_ptr<Type>> paramTypes;  // For function pointers
    std::unique_ptr<Type> returnType;    // For function pointers
    std::string registerName;            // For register types (e.g., "rax", "rbx")
    
    Type(TypeKind k) : kind(k), arraySize(0) {}
    
    static std::unique_ptr<Type> makeU8() { return std::make_unique<Type>(TypeKind::U8); }
    static std::unique_ptr<Type> makeU16() { return std::make_unique<Type>(TypeKind::U16); }
    static std::unique_ptr<Type> makeU32() { return std::make_unique<Type>(TypeKind::U32); }
    static std::unique_ptr<Type> makeU64() { return std::make_unique<Type>(TypeKind::U64); }
    static std::unique_ptr<Type> makePtr() { return std::make_unique<Type>(TypeKind::Ptr); }
    static std::unique_ptr<Type> makePtrTo(std::unique_ptr<Type> pointee) {
        auto ptr = std::make_unique<Type>(TypeKind::Ptr);
        ptr->pointeeType = std::move(pointee);
        return ptr;
    }
};

// Base AST node
class ASTNode {
public:
    SourceLocation loc;
    
    ASTNode() : loc(0, 0) {}
    explicit ASTNode(SourceLocation l) : loc(l) {}
    virtual ~ASTNode() = default;
};

// ===== Expression Nodes =====

class ExprAST : public ASTNode {
public:
    ExprAST() : ASTNode() {}
    explicit ExprAST(SourceLocation l) : ASTNode(l) {}
    virtual ~ExprAST() = default;
};

// Integer literal
class IntLiteralExpr : public ExprAST {
public:
    uint64_t value;
    
    IntLiteralExpr(uint64_t val, SourceLocation l = SourceLocation()) 
        : ExprAST(l), value(val) {}
};

// String literal
class StringLiteralExpr : public ExprAST {
public:
    std::string value;
    
    StringLiteralExpr(const std::string& val, SourceLocation l = SourceLocation()) 
        : ExprAST(l), value(val) {}
};

// Array literal
class ArrayLiteralExpr : public ExprAST {
public:
    std::vector<std::unique_ptr<ExprAST>> elements;
    
    ArrayLiteralExpr(std::vector<std::unique_ptr<ExprAST>> elems, SourceLocation l = SourceLocation())
        : ExprAST(l), elements(std::move(elems)) {}
};

// Identifier (variable reference)
class IdentifierExpr : public ExprAST {
public:
    std::string name;
    
    IdentifierExpr(const std::string& n, SourceLocation l = SourceLocation()) 
        : ExprAST(l), name(n) {}
};

// Binary expression
class BinaryExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> left;
    BinaryOp op;
    std::unique_ptr<ExprAST> right;
    
    BinaryExpr(std::unique_ptr<ExprAST> l, BinaryOp o, std::unique_ptr<ExprAST> r, SourceLocation loc = SourceLocation())
        : ExprAST(loc), left(std::move(l)), op(o), right(std::move(r)) {}
};

// Unary expression
class UnaryExpr : public ExprAST {
public:
    UnaryOp op;
    std::unique_ptr<ExprAST> operand;
    
    UnaryExpr(UnaryOp o, std::unique_ptr<ExprAST> expr, SourceLocation loc = SourceLocation())
        : ExprAST(loc), op(o), operand(std::move(expr)) {}
};

// Function call
class CallExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> callee;
    std::vector<std::unique_ptr<ExprAST>> args;
    
    CallExpr(std::unique_ptr<ExprAST> c, std::vector<std::unique_ptr<ExprAST>> a, SourceLocation loc = SourceLocation())
        : ExprAST(loc), callee(std::move(c)), args(std::move(a)) {}
};

// Syscall expression
class SyscallExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> syscallNumber;  // Changed from uint64_t to support constants
    std::vector<std::unique_ptr<ExprAST>> args;
    
    SyscallExpr(std::unique_ptr<ExprAST> num, std::vector<std::unique_ptr<ExprAST>> a, SourceLocation loc = SourceLocation())
        : ExprAST(loc), syscallNumber(std::move(num)), args(std::move(a)) {}
};

// Encrypted string block
class EncBlockExpr : public ExprAST {
public:
    std::string value;
    
    EncBlockExpr(const std::string& val, SourceLocation loc = SourceLocation()) 
        : ExprAST(loc), value(val) {}
};

// Type cast
class CastExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    std::unique_ptr<Type> targetType;
    
    CastExpr(std::unique_ptr<ExprAST> e, std::unique_ptr<Type> t, SourceLocation loc = SourceLocation())
        : ExprAST(loc), expr(std::move(e)), targetType(std::move(t)) {}
};

// Array indexing
class ArrayIndexExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> array;
    std::unique_ptr<ExprAST> index;
    
    ArrayIndexExpr(std::unique_ptr<ExprAST> arr, std::unique_ptr<ExprAST> idx, SourceLocation loc = SourceLocation())
        : ExprAST(loc), array(std::move(arr)), index(std::move(idx)) {}
};

// Field access (struct.field)
class FieldAccessExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> object;
    std::string fieldName;
    
    FieldAccessExpr(std::unique_ptr<ExprAST> obj, const std::string& field, SourceLocation loc = SourceLocation())
        : ExprAST(loc), object(std::move(obj)), fieldName(field) {}
};

// Struct initialization
class StructInitExpr : public ExprAST {
public:
    std::string structName;
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> fields;
    
    StructInitExpr(const std::string& name, 
                   std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> f,
                   SourceLocation loc = SourceLocation())
        : ExprAST(loc), structName(name), fields(std::move(f)) {}
};

// ===== Statement Nodes =====

class StmtAST : public ASTNode {
public:
    StmtAST() : ASTNode() {}
    explicit StmtAST(SourceLocation l) : ASTNode(l) {}
    virtual ~StmtAST() = default;
};

// Variable declaration
class VarDeclStmt : public StmtAST {
public:
    std::string name;
    std::unique_ptr<Type> type;
    std::unique_ptr<ExprAST> initializer;
    bool isMutable;
    
    VarDeclStmt(const std::string& n, std::unique_ptr<Type> t, 
                std::unique_ptr<ExprAST> init, bool mut, SourceLocation loc = SourceLocation())
        : StmtAST(loc), name(n), type(std::move(t)), initializer(std::move(init)), isMutable(mut) {}
};

// Assignment
class AssignmentStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> target;
    std::unique_ptr<ExprAST> value;
    
    AssignmentStmt(std::unique_ptr<ExprAST> t, std::unique_ptr<ExprAST> v, SourceLocation loc = SourceLocation())
        : StmtAST(loc), target(std::move(t)), value(std::move(v)) {}
};

// If statement
class IfStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> thenBlock;
    std::vector<std::unique_ptr<StmtAST>> elseBlock;
    
    IfStmt(std::unique_ptr<ExprAST> cond,
           std::vector<std::unique_ptr<StmtAST>> thenB,
           std::vector<std::unique_ptr<StmtAST>> elseB,
           SourceLocation loc = SourceLocation())
        : StmtAST(loc), condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}
};

// While loop
class WhileStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> body;
    
    WhileStmt(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<StmtAST>> b, SourceLocation loc = SourceLocation())
        : StmtAST(loc), condition(std::move(cond)), body(std::move(b)) {}
};

// Return statement
class ReturnStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    
    ReturnStmt(std::unique_ptr<ExprAST> val, SourceLocation loc = SourceLocation()) 
        : StmtAST(loc), value(std::move(val)) {}
};

// Expression statement
class ExprStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    
    ExprStmt(std::unique_ptr<ExprAST> e, SourceLocation loc = SourceLocation()) 
        : StmtAST(loc), expr(std::move(e)) {}
};

// Inline assembly operand (for inputs/outputs)
struct AsmOperand {
    std::string constraint;  // e.g., "=r", "r", "m"
    std::unique_ptr<ExprAST> expr;  // variable reference
    
    AsmOperand(const std::string& c, std::unique_ptr<ExprAST> e)
        : constraint(c), expr(std::move(e)) {}
};

// Inline assembly block
class AsmStmt : public StmtAST {
public:
    std::string asmCode;
    std::vector<AsmOperand> outputs;   // output operands
    std::vector<AsmOperand> inputs;    // input operands
    std::vector<std::string> clobbers; // clobbered registers
    
    AsmStmt(const std::string& code, SourceLocation loc = SourceLocation()) 
        : StmtAST(loc), asmCode(code) {}
    
    AsmStmt(const std::string& code,
            std::vector<AsmOperand> out,
            std::vector<AsmOperand> in,
            std::vector<std::string> clob,
            SourceLocation loc = SourceLocation())
        : StmtAST(loc), asmCode(code), outputs(std::move(out)), inputs(std::move(in)), clobbers(std::move(clob)) {}
};

// Unsafe block
class UnsafeBlockStmt : public StmtAST {
public:
    std::vector<std::unique_ptr<StmtAST>> body;
    
    UnsafeBlockStmt(std::vector<std::unique_ptr<StmtAST>> b, SourceLocation loc = SourceLocation()) 
        : StmtAST(loc), body(std::move(b)) {}
};

// ===== Top-Level Nodes =====

// Function parameter
struct Parameter {
    std::string name;
    std::unique_ptr<Type> type;
    
    Parameter(const std::string& n, std::unique_ptr<Type> t)
        : name(n), type(std::move(t)) {}
};

// Function definition
class FunctionAST : public ASTNode {
public:
    std::string name;
    std::vector<Parameter> params;
    std::unique_ptr<Type> returnType;
    std::vector<std::unique_ptr<StmtAST>> body;
    bool isNaked;
    
    FunctionAST(const std::string& n, std::vector<Parameter> p,
                std::unique_ptr<Type> ret, std::vector<std::unique_ptr<StmtAST>> b,
                bool naked = false, SourceLocation loc = SourceLocation())
        : ASTNode(loc), name(n), params(std::move(p)), returnType(std::move(ret)), 
          body(std::move(b)), isNaked(naked) {}
};

// Struct field
struct StructField {
    std::string name;
    std::unique_ptr<Type> type;
    
    StructField(const std::string& n, std::unique_ptr<Type> t)
        : name(n), type(std::move(t)) {}
};

// Struct definition
class StructAST : public ASTNode {
public:
    std::string name;
    std::vector<StructField> fields;
    bool isPacked;
    
    StructAST(const std::string& n, std::vector<StructField> f, bool packed = false, SourceLocation loc = SourceLocation())
        : ASTNode(loc), name(n), fields(std::move(f)), isPacked(packed) {}
};

// Constant definition
class ConstantAST : public ASTNode {
public:
    std::string name;
    std::unique_ptr<Type> type;
    std::unique_ptr<ExprAST> value;
    
    ConstantAST(const std::string& n, std::unique_ptr<Type> t, std::unique_ptr<ExprAST> v, SourceLocation loc = SourceLocation())
        : ASTNode(loc), name(n), type(std::move(t)), value(std::move(v)) {}
};

// Import statement
class ImportAST : public ASTNode {
public:
    std::string path;
    
    ImportAST(const std::string& p, SourceLocation loc = SourceLocation()) 
        : ASTNode(loc), path(p) {}
};

// Program (top-level container)
class ProgramAST : public ASTNode {
public:
    std::vector<std::unique_ptr<FunctionAST>> functions;
    std::vector<std::unique_ptr<StructAST>> structs;
    std::vector<std::unique_ptr<ConstantAST>> constants;
    std::vector<std::unique_ptr<ImportAST>> imports;
};

} // namespace lwanga

#endif // LWANGA_AST_H
