# Requirements Document: Lwanga Programming Language

## Introduction

Lwanga is a low-level, compiled programming language specifically designed for security researchers and hacking tool builders. The language prioritizes minimal binary size, direct system access, and features that enable the creation of security tools, exploits, and position-independent code. The compiler uses LLVM as its backend to provide cross-platform support and optimization while maintaining the low-level control required by security professionals.

## Glossary

- **Lwanga_Compiler**: The complete compilation system that transforms Lwanga source code into executable binaries
- **Lexer**: The tokenization component that converts source text into a stream of tokens
- **Parser**: The component that analyzes token streams and builds an Abstract Syntax Tree
- **AST**: Abstract Syntax Tree - the intermediate representation of program structure
- **IR_Generator**: The component that converts AST into LLVM Intermediate Representation
- **Backend**: The LLVM-based code generation system that produces machine code
- **Syscall_Interface**: The built-in mechanism for making direct system calls without libc
- **Enc_Block**: Encrypted string literal blocks that are decrypted at runtime
- **Naked_Function**: A function without compiler-generated prologue/epilogue code
- **PIC**: Position-Independent Code - code that can execute at any memory address
- **Fixed_Width_Type**: Integer types with explicit bit widths (u8, u16, u32, u64, ptr)

## Requirements

### Requirement 1: Lexical Analysis

**User Story:** As a compiler developer, I want to tokenize Lwanga source code, so that the parser can analyze program structure.

#### Acceptance Criteria

1. WHEN the Lexer receives source code, THE Lexer SHALL tokenize it into a stream of tokens with line number tracking
2. THE Lexer SHALL recognize keywords: u8, u16, u32, u64, ptr, fn, return, if, else, while, asm, syscall, enc, naked, let, mut
3. WHEN the Lexer encounters hexadecimal literals (0x prefix), THE Lexer SHALL parse them as integer tokens
4. WHEN the Lexer encounters decimal literals, THE Lexer SHALL parse them as integer tokens
5. THE Lexer SHALL recognize identifiers matching the pattern [a-zA-Z_][a-zA-Z0-9_]*
6. THE Lexer SHALL recognize operators: +, -, *, /, %, &, |, ^, <<, >>, ==, !=, <, >, <=, >=, =, !
7. THE Lexer SHALL recognize delimiters: (, ), {, }, [, ], ;, ,, :
8. WHEN the Lexer encounters invalid characters, THE Lexer SHALL return an error with line number information
9. THE Lexer SHALL skip whitespace and comments (// single-line, /* */ multi-line)
10. WHEN the Lexer encounters string literals in double quotes, THE Lexer SHALL parse them as string tokens

### Requirement 2: Syntax Analysis

**User Story:** As a compiler developer, I want to parse token streams into an AST, so that I can analyze and generate code from program structure.

#### Acceptance Criteria
1. WHEN the Parser receives a token stream, THE Parser SHALL build an Abstract Syntax Tree representing program structure
2. THE Parser SHALL parse function definitions with syntax: fn name(params) -> return_type { body }
3. THE Parser SHALL parse variable declarations with syntax: let [mut] name: type = expression;
4. THE Parser SHALL parse if-else statements with syntax: if (condition) { block } [else { block }]
5. THE Parser SHALL parse while loops with syntax: while (condition) { block }
6. THE Parser SHALL parse inline assembly blocks with syntax: asm { assembly_code }
7. THE Parser SHALL parse syscall expressions with syntax: syscall(number, arg1, arg2, ...)
8. THE Parser SHALL parse encrypted string blocks with syntax: enc { "string" }
9. THE Parser SHALL parse naked function declarations with syntax: naked fn name() { body }
10. WHEN the Parser encounters syntax errors, THE Parser SHALL return descriptive error messages with line numbers
11. THE Parser SHALL parse binary expressions with correct operator precedence
12. THE Parser SHALL parse pointer operations: dereference (*ptr), address-of (&var), pointer arithmetic
13. THE Parser SHALL parse array indexing with syntax: array[index]
14. THE Parser SHALL parse function calls with syntax: function(arg1, arg2, ...)
15. THE Parser SHALL parse return statements with syntax: return [expression];

### Requirement 3: Type System

**User Story:** As a security researcher, I want explicit fixed-width integer types, so that I can write precise low-level code with predictable memory layout.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support fixed-width unsigned integer types: u8, u16, u32, u64
2. THE Lwanga_Compiler SHALL support a pointer type: ptr
3. WHEN type checking expressions, THE Lwanga_Compiler SHALL verify type compatibility
4. WHEN type checking pointer arithmetic, THE Lwanga_Compiler SHALL allow arithmetic operations on ptr types
5. THE Lwanga_Compiler SHALL support explicit type casting with syntax: expression as type
6. WHEN type checking function calls, THE Lwanga_Compiler SHALL verify argument types match parameter types
7. WHEN type checking assignments, THE Lwanga_Compiler SHALL verify the right-hand side type matches the left-hand side type

### Requirement 4: Direct System Call Interface

**User Story:** As a security tool builder, I want to make direct system calls without libc, so that I can create minimal binaries and avoid library dependencies.

#### Acceptance Criteria

1. WHEN the IR_Generator encounters a syscall expression, THE IR_Generator SHALL generate LLVM IR for direct system call invocation
2. THE Syscall_Interface SHALL accept a system call number as the first argument
3. THE Syscall_Interface SHALL accept up to 6 additional arguments for system call parameters
4. THE Syscall_Interface SHALL generate platform-specific system call instructions (syscall on x86_64, svc on ARM)
5. THE Syscall_Interface SHALL return the system call result as a u64 value
6. WHEN generating syscall IR, THE IR_Generator SHALL preserve argument order according to platform ABI

### Requirement 5: Inline Assembly Support

**User Story:** As a security researcher, I want to write inline assembly, so that I can implement low-level operations and shellcode directly in my programs.

#### Acceptance Criteria

1. WHEN the Parser encounters an asm block, THE Parser SHALL capture the assembly code as a string
2. WHEN the IR_Generator encounters an asm block, THE IR_Generator SHALL generate LLVM inline assembly IR
3. THE Lwanga_Compiler SHALL support AT&T syntax assembly code within asm blocks
4. THE Lwanga_Compiler SHALL allow register access within asm blocks
5. WHEN generating inline assembly, THE IR_Generator SHALL preserve the exact assembly instructions provided

### Requirement 6: Naked Functions

**User Story:** As a security researcher, I want to create naked functions without compiler-generated prologue/epilogue, so that I can write position-independent code and shellcode.

#### Acceptance Criteria

1. WHEN the Parser encounters a naked function declaration, THE Parser SHALL mark the function as naked in the AST
2. WHEN the IR_Generator encounters a naked function, THE IR_Generator SHALL generate LLVM IR without function prologue or epilogue
3. THE Lwanga_Compiler SHALL apply the "naked" attribute to naked functions in LLVM IR
4. WHEN generating code for naked functions, THE Backend SHALL not insert stack frame setup or teardown instructions

### Requirement 7: Encrypted String Literals

**User Story:** As a security tool builder, I want to encrypt string literals at compile time, so that sensitive strings are not visible in the binary until runtime.

#### Acceptance Criteria

1. WHEN the Parser encounters an enc block, THE Parser SHALL extract the string literal for encryption
2. WHEN the IR_Generator encounters an enc block, THE IR_Generator SHALL generate encrypted byte array data
3. THE IR_Generator SHALL generate runtime decryption code for enc blocks
4. THE IR_Generator SHALL use XOR encryption with a compile-time generated key
5. WHEN the program executes an enc block, THE decryption code SHALL produce the original string at runtime

### Requirement 8: Memory Management

**User Story:** As a security researcher, I want manual memory control with raw pointer arithmetic, so that I can implement precise memory operations for exploits and tools.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support pointer dereference operations with syntax: *ptr
2. THE Lwanga_Compiler SHALL support address-of operations with syntax: &variable
3. THE Lwanga_Compiler SHALL support pointer arithmetic: ptr + offset, ptr - offset
4. THE Lwanga_Compiler SHALL allow casting between pointer types and integer types
5. WHEN generating IR for pointer operations, THE IR_Generator SHALL use LLVM pointer instructions
6. THE Lwanga_Compiler SHALL not perform automatic memory management or garbage collection

### Requirement 9: Code Generation

**User Story:** As a compiler developer, I want to generate optimized machine code via LLVM, so that Lwanga programs execute efficiently across multiple platforms.

#### Acceptance Criteria

1. WHEN the IR_Generator completes AST traversal, THE IR_Generator SHALL produce valid LLVM IR
2. THE Backend SHALL use LLVM to optimize the generated IR
3. THE Backend SHALL support static compilation to native executables
4. THE Backend SHALL support JIT compilation for immediate execution
5. THE Backend SHALL generate position-independent code when requested
6. WHEN generating static binaries, THE Backend SHALL produce minimal executables without standard library dependencies
7. THE Backend SHALL support target architectures: x86_64, ARM64

### Requirement 10: Error Handling and Diagnostics

**User Story:** As a Lwanga programmer, I want clear error messages with source locations, so that I can quickly identify and fix issues in my code.

#### Acceptance Criteria

1. WHEN the Lexer encounters an error, THE Lexer SHALL report the error with line and column numbers
2. WHEN the Parser encounters an error, THE Parser SHALL report the error with line numbers and context
3. WHEN the IR_Generator encounters a type error, THE IR_Generator SHALL report the error with source location
4. THE Lwanga_Compiler SHALL display error messages in a human-readable format
5. WHEN multiple errors exist, THE Lwanga_Compiler SHALL report all errors found during compilation
6. THE Lwanga_Compiler SHALL distinguish between errors (compilation fails) and warnings (compilation continues)

### Requirement 11: Register Access

**User Story:** As a security researcher, I want to access CPU registers directly as variables, so that I can write low-level code that manipulates processor state.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support register variables with syntax: let reg_name: register;
2. THE Lwanga_Compiler SHALL recognize platform-specific register names (rax, rbx, rcx, rdx, rsi, rdi, rsp, rbp for x86_64)
3. WHEN the IR_Generator encounters register variables, THE IR_Generator SHALL generate LLVM IR using register constraints
4. THE Lwanga_Compiler SHALL allow reading from and writing to register variables

### Requirement 12: Compiler Driver

**User Story:** As a Lwanga programmer, I want a command-line compiler interface, so that I can compile my programs with various options.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL accept source file paths as command-line arguments
2. THE Lwanga_Compiler SHALL support an output flag (-o) to specify the output file name
3. THE Lwanga_Compiler SHALL support an optimization level flag (-O0, -O1, -O2, -O3)
4. THE Lwanga_Compiler SHALL support a JIT execution flag (--jit) to compile and run immediately
5. THE Lwanga_Compiler SHALL support a verbose flag (-v) to display compilation stages
6. THE Lwanga_Compiler SHALL support a target architecture flag (--target) to specify the compilation target
7. WHEN compilation succeeds, THE Lwanga_Compiler SHALL exit with status code 0
8. WHEN compilation fails, THE Lwanga_Compiler SHALL exit with a non-zero status code

### Requirement 13: Standard Library Minimalism

**User Story:** As a security tool builder, I want zero standard library dependencies, so that I can create tiny, portable binaries.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL not link against libc by default
2. THE Lwanga_Compiler SHALL not include runtime library dependencies in generated binaries
3. THE Backend SHALL generate freestanding executables that do not require dynamic linking
4. WHEN generating static binaries, THE Backend SHALL produce executables under 10KB for minimal programs

### Requirement 14: Control Flow

**User Story:** As a Lwanga programmer, I want standard control flow constructs, so that I can write structured programs.

#### Acceptance Criteria

1. THE Parser SHALL parse if statements with optional else clauses
2. THE Parser SHALL parse while loops
3. THE Parser SHALL parse return statements with optional return values
4. WHEN the IR_Generator encounters control flow statements, THE IR_Generator SHALL generate appropriate LLVM branch instructions
5. THE Lwanga_Compiler SHALL verify that all code paths in non-void functions return a value

### Requirement 15: Variable Mutability

**User Story:** As a Lwanga programmer, I want explicit mutability control, so that I can prevent accidental modifications to variables.

#### Acceptance Criteria

1. THE Parser SHALL parse immutable variable declarations with syntax: let name: type = value;
2. THE Parser SHALL parse mutable variable declarations with syntax: let mut name: type = value;
3. WHEN type checking assignments, THE Lwanga_Compiler SHALL verify that the target variable is mutable
4. WHEN a program attempts to assign to an immutable variable, THE Lwanga_Compiler SHALL report a compilation error

### Requirement 16: Arrays and Buffers

**User Story:** As a security researcher, I want fixed-size arrays and buffer manipulation, so that I can handle payloads, shellcode, and network data.

#### Acceptance Criteria

1. THE Parser SHALL parse array type declarations with syntax: [type; size]
2. THE Parser SHALL parse array literals with syntax: [value1, value2, value3]
3. THE Parser SHALL parse array indexing with syntax: array[index]
4. WHEN the IR_Generator encounters array declarations, THE IR_Generator SHALL allocate contiguous memory
5. THE Lwanga_Compiler SHALL support pointer-to-array conversions for buffer manipulation
6. THE Lwanga_Compiler SHALL allow arrays to be passed to functions as pointers
7. WHEN accessing array elements, THE Lwanga_Compiler SHALL generate bounds-checked or unchecked access based on context

### Requirement 17: Bitwise Operations

**User Story:** As a security tool builder, I want comprehensive bitwise operations, so that I can manipulate binary data for encoding, encryption, and protocol implementation.

#### Acceptance Criteria

1. THE Parser SHALL parse bitwise AND operations with syntax: a & b
2. THE Parser SHALL parse bitwise OR operations with syntax: a | b
3. THE Parser SHALL parse bitwise XOR operations with syntax: a ^ b
4. THE Parser SHALL parse bitwise NOT operations with syntax: ~a
5. THE Parser SHALL parse left shift operations with syntax: a << b
6. THE Parser SHALL parse right shift operations with syntax: a >> b
7. WHEN the IR_Generator encounters bitwise operations, THE IR_Generator SHALL generate LLVM bitwise instructions

### Requirement 18: Network and I/O Primitives

**User Story:** As a security tool builder, I want low-level I/O operations via syscalls, so that I can implement network tools, file operations, and process manipulation.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL provide syscall access for socket operations (socket, bind, listen, accept, connect, send, recv)
2. THE Lwanga_Compiler SHALL provide syscall access for file operations (open, read, write, close, lseek)
3. THE Lwanga_Compiler SHALL provide syscall access for process operations (fork, execve, wait, kill)
4. THE Lwanga_Compiler SHALL provide syscall access for memory operations (mmap, munmap, mprotect)
5. THE Lwanga_Compiler SHALL document syscall numbers for supported platforms in compiler documentation

### Requirement 19: String Operations

**User Story:** As a Lwanga programmer, I want basic string handling capabilities, so that I can work with text data in security tools.

#### Acceptance Criteria

1. THE Parser SHALL parse string literals with escape sequences (\n, \r, \t, \\, \", \x00)
2. THE Lwanga_Compiler SHALL store string literals as null-terminated byte arrays
3. THE Lwanga_Compiler SHALL support string length calculation at compile time
4. THE Lwanga_Compiler SHALL allow pointer arithmetic on string literals
5. WHEN the IR_Generator encounters string literals, THE IR_Generator SHALL generate global constant data

### Requirement 20: Struct Types

**User Story:** As a security researcher, I want to define structured data types, so that I can represent network packets, file formats, and complex data structures.

#### Acceptance Criteria

1. THE Parser SHALL parse struct definitions with syntax: struct Name { field1: type1, field2: type2 }
2. THE Parser SHALL parse struct initialization with syntax: Name { field1: value1, field2: value2 }
3. THE Parser SHALL parse field access with syntax: struct_instance.field_name
4. WHEN the IR_Generator encounters struct types, THE IR_Generator SHALL generate LLVM struct types with correct memory layout
5. THE Lwanga_Compiler SHALL support nested struct definitions
6. THE Lwanga_Compiler SHALL calculate struct sizes and field offsets at compile time
7. THE Lwanga_Compiler SHALL support packed structs with explicit memory layout control

### Requirement 21: Function Pointers

**User Story:** As a security researcher, I want function pointers, so that I can implement callbacks, dynamic dispatch, and hook mechanisms.

#### Acceptance Criteria

1. THE Parser SHALL parse function pointer types with syntax: fn(param_types) -> return_type
2. THE Parser SHALL parse function pointer assignments from function names
3. THE Parser SHALL parse indirect function calls with syntax: function_pointer(args)
4. WHEN the IR_Generator encounters function pointers, THE IR_Generator SHALL generate LLVM function pointer types
5. THE Lwanga_Compiler SHALL support casting between function pointer types and ptr types

### Requirement 22: Constant Expressions

**User Story:** As a Lwanga programmer, I want compile-time constant evaluation, so that I can define constants and perform compile-time calculations.

#### Acceptance Criteria

1. THE Parser SHALL parse constant declarations with syntax: const NAME: type = value;
2. THE Lwanga_Compiler SHALL evaluate constant expressions at compile time
3. THE Lwanga_Compiler SHALL support arithmetic operations in constant expressions
4. THE Lwanga_Compiler SHALL support bitwise operations in constant expressions
5. WHEN a constant is used, THE Lwanga_Compiler SHALL substitute its compile-time value

### Requirement 23: Module System

**User Story:** As a Lwanga programmer, I want to organize code into modules, so that I can build larger projects with multiple source files.

#### Acceptance Criteria

1. THE Parser SHALL parse import statements with syntax: import "path/to/module";
2. THE Lwanga_Compiler SHALL resolve imported modules relative to the source file
3. THE Lwanga_Compiler SHALL parse each imported module and include its definitions
4. THE Lwanga_Compiler SHALL detect circular imports and report errors
5. THE Lwanga_Compiler SHALL support exporting functions and types from modules

### Requirement 24: Obfuscation Support

**User Story:** As a security tool builder, I want built-in code obfuscation capabilities, so that I can make reverse engineering more difficult.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support a compilation flag (--obfuscate) to enable obfuscation
2. WHEN obfuscation is enabled, THE Backend SHALL apply control flow flattening transformations
3. WHEN obfuscation is enabled, THE Backend SHALL insert opaque predicates
4. WHEN obfuscation is enabled, THE Backend SHALL randomize instruction ordering where semantically equivalent
5. THE Lwanga_Compiler SHALL preserve program semantics when obfuscation is applied

### Requirement 25: Debugging Information

**User Story:** As a Lwanga programmer, I want optional debugging information, so that I can debug my programs when needed.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support a debug flag (-g) to include debugging information
2. WHEN debug mode is enabled, THE Backend SHALL generate DWARF debugging information
3. WHEN debug mode is disabled, THE Backend SHALL strip all debugging symbols
4. THE Lwanga_Compiler SHALL map generated code back to source line numbers in debug mode

### Requirement 26: Cross-Compilation

**User Story:** As a security researcher, I want to cross-compile for different platforms, so that I can build tools for target systems from my development machine.

#### Acceptance Criteria

1. THE Lwanga_Compiler SHALL support target triple specification with --target flag
2. THE Lwanga_Compiler SHALL support cross-compilation for x86_64-linux, x86_64-windows, aarch64-linux, aarch64-android
3. WHEN cross-compiling, THE Backend SHALL generate platform-specific syscall interfaces
4. WHEN cross-compiling, THE Backend SHALL use platform-specific calling conventions
5. THE Lwanga_Compiler SHALL validate that target platform is supported before compilation

### Requirement 27: Preprocessor

**User Story:** As a Lwanga programmer, I want conditional compilation, so that I can write platform-specific code and feature flags.

#### Acceptance Criteria

1. THE Lexer SHALL recognize preprocessor directives: #if, #else, #endif, #define
2. THE Lwanga_Compiler SHALL evaluate preprocessor conditions before parsing
3. THE Lwanga_Compiler SHALL support platform-based conditionals: #if PLATFORM_LINUX, #if PLATFORM_WINDOWS
4. THE Lwanga_Compiler SHALL support architecture-based conditionals: #if ARCH_X86_64, #if ARCH_ARM64
5. WHEN preprocessor conditions are false, THE Lwanga_Compiler SHALL exclude code blocks from compilation

### Requirement 28: Unsafe Blocks

**User Story:** As a security researcher, I want explicit unsafe blocks for dangerous operations, so that I can clearly mark code that bypasses safety checks.

#### Acceptance Criteria

1. THE Parser SHALL parse unsafe blocks with syntax: unsafe { code }
2. THE Lwanga_Compiler SHALL allow unchecked pointer operations only within unsafe blocks
3. THE Lwanga_Compiler SHALL allow inline assembly only within unsafe blocks or naked functions
4. THE Lwanga_Compiler SHALL allow direct memory access only within unsafe blocks
5. WHEN code outside unsafe blocks attempts unsafe operations, THE Lwanga_Compiler SHALL report a compilation error
