# Design Document: Lwanga Programming Language Compiler

## Overview

The Lwanga compiler is a complete compilation system that transforms Lwanga source code into optimized native executables. The compiler follows a traditional multi-stage pipeline architecture: Lexer → Parser → AST → IR Generator → LLVM Backend. This design leverages LLVM's mature optimization and code generation infrastructure while providing the low-level control and hacking-specific features that security researchers require.

The compiler is designed to produce minimal, freestanding binaries without standard library dependencies, making it ideal for creating security tools, exploits, and position-independent code. Key design principles include:

- **Zero-cost abstractions**: High-level features compile to efficient machine code
- **Explicit control**: Programmers have direct access to memory, registers, and system calls
- **Minimal footprint**: Generated binaries are tiny and self-contained
- **Cross-platform**: LLVM backend enables targeting multiple architectures
- **Security-focused**: Built-in features for obfuscation, encryption, and low-level manipulation

## Architecture

### Compilation Pipeline

```
Source Code (.lwanga)
    ↓
┌─────────────────┐
│     Lexer       │  Tokenization with state machine
└─────────────────┘
    ↓
Token Stream
    ↓
┌─────────────────┐
│     Parser      │  Recursive descent parser
└─────────────────┘
    ↓
Abstract Syntax Tree (AST)
    ↓
┌─────────────────┐
│  Type Checker   │  Semantic analysis
└─────────────────┘
    ↓
Typed AST
    ↓
┌─────────────────┐
│  IR Generator   │  LLVM IR generation
└─────────────────┘
    ↓
LLVM IR
    ↓
┌─────────────────┐
│  LLVM Backend   │  Optimization + Code generation
└─────────────────┘
    ↓
Native Executable or JIT
```

### Component Interaction

The compiler is organized into distinct phases with clear interfaces:

1. **Lexer**: Consumes raw source text, produces token stream
2. **Parser**: Consumes token stream, produces AST
3. **Type Checker**: Consumes AST, produces typed AST with error reporting
4. **IR Generator**: Consumes typed AST, produces LLVM IR
5. **Backend**: Consumes LLVM IR, produces machine code

Each phase is independent and testable, with well-defined input/output contracts.

## Components and Interfaces

### 1. Lexer

**Purpose**: Convert source text into a stream of tokens with position tracking.

**Data Structures**:

```
Token {
    type: TokenType,
    lexeme: String,
    line: u32,
    column: u32
}

TokenType = 
    | Keyword(KeywordType)
    | Identifier(String)
    | IntLiteral(u64)
    | StringLiteral(String)
    | Operator(OperatorType)
    | Delimiter(DelimiterType)
    | EOF

KeywordType = U8 | U16 | U32 | U64 | Ptr | Fn | Return | If | Else | While 
            | Asm | Syscall | Enc | Naked | Let | Mut | Struct | Const 
            | Import | Unsafe

OperatorType = Plus | Minus | Star | Slash | Percent | Ampersand | Pipe 
             | Caret | Tilde | LeftShift | RightShift | Equal | NotEqual 
             | Less | Greater | LessEqual | GreaterEqual | Assign | Not

DelimiterType = LeftParen | RightParen | LeftBrace | RightBrace 
              | LeftBracket | RightBracket | Semicolon | Comma | Colon | Dot
```

**Algorithm**:

The lexer uses a state machine approach with lookahead:

1. Skip whitespace and comments
2. Check for keywords by matching against keyword table
3. Parse numbers (hex with 0x prefix, decimal otherwise)
4. Parse identifiers (start with letter/underscore, continue with alphanumeric/underscore)
5. Parse operators (with lookahead for multi-character operators like <<, ==)
6. Parse string literals (handle escape sequences)
7. Track line and column for error reporting

**Interface**:

```
Lexer {
    fn new(source: String) -> Lexer
    fn next_token() -> Result<Token, LexError>
    fn peek_token() -> Result<Token, LexError>
}
```

### 2. Parser

**Purpose**: Build an Abstract Syntax Tree from token stream using recursive descent parsing.

**Data Structures**:

```
AST = Program(Vec<TopLevel>)

TopLevel = 
    | FunctionDef(Function)
    | StructDef(Struct)
    | ConstDef(Constant)
    | Import(String)

Function {
    name: String,
    params: Vec<Parameter>,
    return_type: Type,
    body: Block,
    is_naked: bool
}

Parameter {
    name: String,
    type: Type
}

Type = 
    | U8 | U16 | U32 | U64 | Ptr
    | Array(Box<Type>, usize)
    | Struct(String)
    | FunctionPointer(Vec<Type>, Box<Type>)

Statement = 
    | VarDecl(String, Type, Expression, bool) // name, type, init, is_mutable
    | Assignment(LValue, Expression)
    | If(Expression, Block, Option<Block>)
    | While(Expression, Block)
    | Return(Option<Expression>)
    | Expression(Expression)
    | AsmBlock(String)
    | UnsafeBlock(Block)

Expression = 
    | IntLiteral(u64)
    | StringLiteral(String)
    | Identifier(String)
    | Binary(Box<Expression>, BinaryOp, Box<Expression>)
    | Unary(UnaryOp, Box<Expression>)
    | Call(Box<Expression>, Vec<Expression>)
    | Syscall(u64, Vec<Expression>)
    | EncBlock(String)
    | Cast(Box<Expression>, Type)
    | ArrayIndex(Box<Expression>, Box<Expression>)
    | FieldAccess(Box<Expression>, String)
    | StructInit(String, Vec<(String, Expression)>)

BinaryOp = Add | Sub | Mul | Div | Mod | BitAnd | BitOr | BitXor 
         | LeftShift | RightShift | Equal | NotEqual | Less | Greater 
         | LessEqual | GreaterEqual

UnaryOp = Negate | Not | Deref | AddressOf

LValue = 
    | Variable(String)
    | Deref(Box<Expression>)
    | ArrayIndex(Box<Expression>, Box<Expression>)
    | FieldAccess(Box<Expression>, String)

Block = Vec<Statement>

Struct {
    name: String,
    fields: Vec<(String, Type)>,
    is_packed: bool
}

Constant {
    name: String,
    type: Type,
    value: Expression
}
```

**Algorithm**:

Recursive descent parser with operator precedence climbing for expressions:

1. **parse_program()**: Parse top-level declarations (functions, structs, imports)
2. **parse_function()**: Parse function signature and body
3. **parse_statement()**: Dispatch to specific statement parsers
4. **parse_expression()**: Use precedence climbing for binary operators
5. **parse_primary()**: Parse literals, identifiers, parenthesized expressions

Operator precedence (highest to lowest):
1. Unary: -, !, *, &
2. Multiplicative: *, /, %
3. Additive: +, -
4. Shift: <<, >>
5. Relational: <, >, <=, >=
6. Equality: ==, !=
7. Bitwise AND: &
8. Bitwise XOR: ^
9. Bitwise OR: |

**Interface**:

```
Parser {
    fn new(lexer: Lexer) -> Parser
    fn parse() -> Result<AST, ParseError>
}
```

### 3. Type Checker

**Purpose**: Perform semantic analysis and type checking on the AST.

**Data Structures**:

```
TypeChecker {
    symbol_table: SymbolTable,
    current_function: Option<String>,
    errors: Vec<TypeError>
}

SymbolTable {
    scopes: Vec<Scope>
}

Scope = HashMap<String, Symbol>

Symbol = 
    | Variable(Type, bool) // type, is_mutable
    | Function(Vec<Type>, Type) // param types, return type
    | Struct(Vec<(String, Type)>)
    | Constant(Type, Value)

TypeError {
    message: String,
    line: u32,
    column: u32
}
```

**Algorithm**:

1. Build symbol table by traversing AST
2. Check each expression for type correctness
3. Verify function calls match signatures
4. Verify assignments to mutable variables only
5. Verify all code paths return appropriate values
6. Verify pointer operations are in unsafe blocks
7. Collect all errors for reporting

**Type Rules**:

- Binary arithmetic operations require matching numeric types
- Pointer arithmetic: ptr + u64 → ptr, ptr - ptr → u64
- Comparisons require matching types, produce boolean result
- Function calls require exact parameter type matches
- Assignments require matching types or valid implicit conversions
- Casts allow explicit type conversions

**Interface**:

```
TypeChecker {
    fn new() -> TypeChecker
    fn check(ast: AST) -> Result<TypedAST, Vec<TypeError>>
}
```

### 4. IR Generator

**Purpose**: Convert typed AST into LLVM Intermediate Representation.

**Data Structures**:

```
IRGenerator {
    context: LLVMContext,
    module: LLVMModule,
    builder: LLVMBuilder,
    symbol_table: HashMap<String, LLVMValue>,
    current_function: Option<LLVMFunction>
}
```

**Algorithm**:

1. **Initialize LLVM context and module**
2. **Generate function declarations**: Create LLVM function prototypes
3. **Generate struct types**: Define LLVM struct types with correct layout
4. **Generate function bodies**:
   - Create entry basic block
   - Allocate stack space for local variables
   - Generate IR for each statement
   - Ensure proper termination (return or branch)
5. **Handle special constructs**:
   - **Syscall**: Generate inline assembly for syscall instruction
   - **Asm blocks**: Generate LLVM inline assembly
   - **Enc blocks**: Generate encrypted data + decryption code
   - **Naked functions**: Apply naked attribute, no prologue/epilogue

**Syscall Implementation**:

Platform-specific syscall generation:

**x86_64 Linux**:
```
; syscall number in rax
; args in rdi, rsi, rdx, r10, r8, r9
; result in rax
syscall
```

**ARM64 Linux**:
```
; syscall number in x8
; args in x0-x5
; result in x0
svc #0
```

**Encrypted String Implementation**:

1. Generate random XOR key at compile time
2. Encrypt string bytes: encrypted[i] = plaintext[i] ^ key[i % key_len]
3. Store encrypted bytes as global array
4. Generate decryption function:
   ```
   for i in 0..len:
       decrypted[i] = encrypted[i] ^ key[i % key_len]
   ```
5. Return pointer to decrypted buffer

**Interface**:

```
IRGenerator {
    fn new(target_triple: String) -> IRGenerator
    fn generate(ast: TypedAST) -> Result<LLVMModule, IRError>
}
```

### 5. Backend

**Purpose**: Optimize LLVM IR and generate native machine code.

**Components**:

- **Optimizer**: Apply LLVM optimization passes
- **Code Generator**: Generate native assembly/object code
- **Linker**: Create final executable (static linking)

**Optimization Levels**:

- **-O0**: No optimization, fast compilation
- **-O1**: Basic optimizations (constant folding, dead code elimination)
- **-O2**: Moderate optimizations (inlining, loop optimizations)
- **-O3**: Aggressive optimizations (vectorization, aggressive inlining)

**Obfuscation Passes** (when --obfuscate flag is set):

1. **Control Flow Flattening**: Convert structured control flow to switch-based dispatch
2. **Opaque Predicates**: Insert always-true/false conditions that are hard to analyze
3. **Instruction Substitution**: Replace simple operations with complex equivalents
4. **Bogus Control Flow**: Add fake branches that are never taken

**Output Modes**:

- **Static Executable**: Freestanding binary with no dynamic dependencies
- **JIT**: Compile and execute in memory immediately
- **Object File**: Relocatable object for linking with other code

**Interface**:

```
Backend {
    fn new(module: LLVMModule, target: String) -> Backend
    fn optimize(level: OptLevel) -> Result<(), BackendError>
    fn obfuscate() -> Result<(), BackendError>
    fn emit_object() -> Result<Vec<u8>, BackendError>
    fn emit_executable(output_path: String) -> Result<(), BackendError>
    fn jit_execute() -> Result<i32, BackendError>
}
```

### 6. Compiler Driver

**Purpose**: Orchestrate the compilation pipeline and handle command-line interface.

**Data Structures**:

```
CompilerOptions {
    input_files: Vec<String>,
    output_file: Option<String>,
    optimization_level: OptLevel,
    target_triple: String,
    jit_mode: bool,
    obfuscate: bool,
    debug_info: bool,
    verbose: bool
}

OptLevel = O0 | O1 | O2 | O3
```

**Algorithm**:

1. Parse command-line arguments
2. Read source files
3. For each source file:
   - Run lexer
   - Run parser
   - Run type checker
4. Merge ASTs from multiple files
5. Generate LLVM IR
6. Apply optimizations
7. Generate output (executable, object, or JIT)
8. Report errors or success

**Interface**:

```
Compiler {
    fn new(options: CompilerOptions) -> Compiler
    fn compile() -> Result<(), CompilerError>
}
```

## Data Models

### Symbol Table

The symbol table maintains scoping information and symbol definitions:

```
SymbolTable {
    scopes: Vec<Scope>,
    current_scope: usize
}

Scope {
    parent: Option<usize>,
    symbols: HashMap<String, Symbol>
}

Symbol {
    name: String,
    kind: SymbolKind,
    type: Type,
    is_mutable: bool,
    defined_at: SourceLocation
}

SymbolKind = Variable | Function | Struct | Constant | Parameter
```

**Operations**:
- `enter_scope()`: Push new scope
- `exit_scope()`: Pop current scope
- `define(name, symbol)`: Add symbol to current scope
- `lookup(name)`: Search current scope and parents
- `lookup_current_scope(name)`: Search only current scope

### Type System

Types are represented as an algebraic data type:

```
Type = 
    | Primitive(PrimitiveType)
    | Pointer(Box<Type>)
    | Array(Box<Type>, usize)
    | Struct(String, Vec<Field>)
    | Function(Vec<Type>, Box<Type>)

PrimitiveType = U8 | U16 | U32 | U64

Field {
    name: String,
    type: Type,
    offset: usize
}
```

**Type Operations**:
- `size_of(type)`: Calculate type size in bytes
- `align_of(type)`: Calculate type alignment
- `is_compatible(t1, t2)`: Check type compatibility
- `can_cast(from, to)`: Check if cast is valid

### Error Reporting

Errors are collected during compilation with source location information:

```
CompilerError {
    kind: ErrorKind,
    message: String,
    location: SourceLocation,
    suggestion: Option<String>
}

ErrorKind = LexError | ParseError | TypeError | IRError | BackendError

SourceLocation {
    file: String,
    line: u32,
    column: u32,
    length: u32
}
```

**Error Display Format**:
```
error[E0001]: type mismatch
  --> example.lwanga:15:10
   |
15 |     let x: u32 = "hello";
   |                  ^^^^^^^ expected u32, found string
   |
help: consider using a numeric literal
```

