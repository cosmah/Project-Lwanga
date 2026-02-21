#ifndef LWANGA_AST_H
#define LWANGA_AST_H

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

namespace lwanga {

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
    U8, U16, U32, U64, Ptr, Array, Struct, FunctionPointer
};

struct Type {
    TypeKind kind;
    std::unique_ptr<Type> elementType;  // For pointers and arrays
    std::unique_ptr<Type> pointeeType;  // For pointers (what they point to)
    size_t arraySize;                    // For arrays
    std::string structName;              // For structs
    std::vector<std::unique_ptr<Type>> paramTypes;  // For function pointers
    std::unique_ptr<Type> returnType;    // For function pointers
    
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
    virtual ~ASTNode() = default;
};

// ===== Expression Nodes =====

class ExprAST : public ASTNode {
public:
    virtual ~ExprAST() = default;
};

// Integer literal
class IntLiteralExpr : public ExprAST {
public:
    uint64_t value;
    
    explicit IntLiteralExpr(uint64_t val) : value(val) {}
};

// String literal
class StringLiteralExpr : public ExprAST {
public:
    std::string value;
    
    explicit StringLiteralExpr(const std::string& val) : value(val) {}
};

// Identifier (variable reference)
class IdentifierExpr : public ExprAST {
public:
    std::string name;
    
    explicit IdentifierExpr(const std::string& n) : name(n) {}
};

// Binary expression
class BinaryExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> left;
    BinaryOp op;
    std::unique_ptr<ExprAST> right;
    
    BinaryExpr(std::unique_ptr<ExprAST> l, BinaryOp o, std::unique_ptr<ExprAST> r)
        : left(std::move(l)), op(o), right(std::move(r)) {}
};

// Unary expression
class UnaryExpr : public ExprAST {
public:
    UnaryOp op;
    std::unique_ptr<ExprAST> operand;
    
    UnaryExpr(UnaryOp o, std::unique_ptr<ExprAST> expr)
        : op(o), operand(std::move(expr)) {}
};

// Function call
class CallExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> callee;
    std::vector<std::unique_ptr<ExprAST>> args;
    
    CallExpr(std::unique_ptr<ExprAST> c, std::vector<std::unique_ptr<ExprAST>> a)
        : callee(std::move(c)), args(std::move(a)) {}
};

// Syscall expression
class SyscallExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> syscallNumber;  // Changed from uint64_t to support constants
    std::vector<std::unique_ptr<ExprAST>> args;
    
    SyscallExpr(std::unique_ptr<ExprAST> num, std::vector<std::unique_ptr<ExprAST>> a)
        : syscallNumber(std::move(num)), args(std::move(a)) {}
};

// Encrypted string block
class EncBlockExpr : public ExprAST {
public:
    std::string value;
    
    explicit EncBlockExpr(const std::string& val) : value(val) {}
};

// Type cast
class CastExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> expr;
    std::unique_ptr<Type> targetType;
    
    CastExpr(std::unique_ptr<ExprAST> e, std::unique_ptr<Type> t)
        : expr(std::move(e)), targetType(std::move(t)) {}
};

// Array indexing
class ArrayIndexExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> array;
    std::unique_ptr<ExprAST> index;
    
    ArrayIndexExpr(std::unique_ptr<ExprAST> arr, std::unique_ptr<ExprAST> idx)
        : array(std::move(arr)), index(std::move(idx)) {}
};

// Field access (struct.field)
class FieldAccessExpr : public ExprAST {
public:
    std::unique_ptr<ExprAST> object;
    std::string fieldName;
    
    FieldAccessExpr(std::unique_ptr<ExprAST> obj, const std::string& field)
        : object(std::move(obj)), fieldName(field) {}
};

// Struct initialization
class StructInitExpr : public ExprAST {
public:
    std::string structName;
    std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> fields;
    
    StructInitExpr(const std::string& name, 
                   std::vector<std::pair<std::string, std::unique_ptr<ExprAST>>> f)
        : structName(name), fields(std::move(f)) {}
};

// ===== Statement Nodes =====

class StmtAST : public ASTNode {
public:
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
                std::unique_ptr<ExprAST> init, bool mut)
        : name(n), type(std::move(t)), initializer(std::move(init)), isMutable(mut) {}
};

// Assignment
class AssignmentStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> target;
    std::unique_ptr<ExprAST> value;
    
    AssignmentStmt(std::unique_ptr<ExprAST> t, std::unique_ptr<ExprAST> v)
        : target(std::move(t)), value(std::move(v)) {}
};

// If statement
class IfStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> thenBlock;
    std::vector<std::unique_ptr<StmtAST>> elseBlock;
    
    IfStmt(std::unique_ptr<ExprAST> cond,
           std::vector<std::unique_ptr<StmtAST>> thenB,
           std::vector<std::unique_ptr<StmtAST>> elseB)
        : condition(std::move(cond)), thenBlock(std::move(thenB)), elseBlock(std::move(elseB)) {}
};

// While loop
class WhileStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> condition;
    std::vector<std::unique_ptr<StmtAST>> body;
    
    WhileStmt(std::unique_ptr<ExprAST> cond, std::vector<std::unique_ptr<StmtAST>> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

// Return statement
class ReturnStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> value;
    
    explicit ReturnStmt(std::unique_ptr<ExprAST> val) : value(std::move(val)) {}
};

// Expression statement
class ExprStmt : public StmtAST {
public:
    std::unique_ptr<ExprAST> expr;
    
    explicit ExprStmt(std::unique_ptr<ExprAST> e) : expr(std::move(e)) {}
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
    
    explicit AsmStmt(const std::string& code) : asmCode(code) {}
    
    AsmStmt(const std::string& code,
            std::vector<AsmOperand> out,
            std::vector<AsmOperand> in,
            std::vector<std::string> clob)
        : asmCode(code), outputs(std::move(out)), inputs(std::move(in)), clobbers(std::move(clob)) {}
};

// Unsafe block
class UnsafeBlockStmt : public StmtAST {
public:
    std::vector<std::unique_ptr<StmtAST>> body;
    
    explicit UnsafeBlockStmt(std::vector<std::unique_ptr<StmtAST>> b) : body(std::move(b)) {}
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
                bool naked = false)
        : name(n), params(std::move(p)), returnType(std::move(ret)), 
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
    
    StructAST(const std::string& n, std::vector<StructField> f, bool packed = false)
        : name(n), fields(std::move(f)), isPacked(packed) {}
};

// Constant definition
class ConstantAST : public ASTNode {
public:
    std::string name;
    std::unique_ptr<Type> type;
    std::unique_ptr<ExprAST> value;
    
    ConstantAST(const std::string& n, std::unique_ptr<Type> t, std::unique_ptr<ExprAST> v)
        : name(n), type(std::move(t)), value(std::move(v)) {}
};

// Import statement
class ImportAST : public ASTNode {
public:
    std::string path;
    
    explicit ImportAST(const std::string& p) : path(p) {}
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
