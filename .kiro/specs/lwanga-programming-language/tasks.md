# Implementation Plan: Lwanga Programming Language Compiler

## Overview

This implementation plan breaks down the development of the Lwanga compiler into discrete, incremental tasks. The compiler will be implemented in C++ using LLVM as the backend. The plan follows a bottom-up approach, building foundational components first (lexer, parser) and progressively adding more complex features. Each task builds on previous work, ensuring the compiler remains functional at each stage.

The implementation prioritizes core functionality first, with security-specific features (obfuscation, encrypted strings) added later. Testing tasks are included as optional sub-tasks to validate correctness without blocking progress.

## Tasks

- [x] 1. Set up project structure and LLVM integration
  - Create CMake build system with LLVM dependencies
  - Set up directory structure: src/, include/, tests/, examples/
  - Create main.cpp with basic command-line argument parsing
  - Link against LLVM libraries (Core, Support, Target, X86)
  - Verify LLVM integration with a "Hello LLVM" test program
  - _Requirements: 12.1, 12.7, 12.8_

- [x] 2. Implement lexer for tokenization
  - [x] 2.1 Create Token and TokenType classes
    - Define Token struct with type, lexeme, line, column fields
    - Define TokenType enum covering all keywords, operators, delimiters
    - Implement token-to-string conversion for debugging
    - _Requirements: 1.1, 1.2_
  
  - [x] 2.2 Implement Lexer class with state machine
    - Create Lexer class with source text input
    - Implement next_token() method with state machine logic
    - Add whitespace and comment skipping (// and /* */)
    - Implement keyword recognition with lookup table
    - _Requirements: 1.1, 1.9_
  
  - [x] 2.3 Add number literal parsing
    - Parse hexadecimal literals (0x prefix)
    - Parse decimal integer literals
    - Store numeric values in Token
    - _Requirements: 1.3, 1.4_
  
  - [x] 2.4 Add identifier and operator parsing
    - Parse identifiers matching [a-zA-Z_][a-zA-Z0-9_]*
    - Parse single-character operators
    - Parse multi-character operators (<<, >>, ==, !=, <=, >=) with lookahead
    - _Requirements: 1.5, 1.6_
  
  - [x] 2.5 Add string literal parsing
    - Parse string literals in double quotes
    - Handle escape sequences (\n, \r, \t, \\, \", \xHH)
    - _Requirements: 1.10, 19.1_
  
  - [x] 2.6 Add delimiter parsing and error handling
    - Parse all delimiters: (), {}, [], ;, ,, :, .
    - Implement error reporting with line/column numbers
    - Handle invalid characters gracefully
    - _Requirements: 1.7, 1.8, 10.1_
  
  - [x]* 2.7 Write unit tests for lexer
    - Test keyword recognition
    - Test number parsing (hex and decimal)
    - Test operator parsing (single and multi-character)
    - Test string literals with escape sequences
    - Test error cases (invalid characters)
    - _Requirements: 1.1-1.10_

- [x] 3. Implement parser for AST construction
  - [x] 3.1 Define AST node classes
    - Create base ASTNode class with visitor pattern support
    - Define expression nodes: IntLiteral, StringLiteral, Identifier, Binary, Unary
    - Define statement nodes: VarDecl, Assignment, If, While, Return, ExprStmt
    - Define top-level nodes: Function, Struct, Constant, Import
    - _Requirements: 2.1_
  
  - [x] 3.2 Implement Parser class with recursive descent
    - Create Parser class consuming Lexer
    - Implement parse() entry point returning Program AST
    - Add error recovery and synchronization
    - _Requirements: 2.1, 2.10_
  
  - [x] 3.3 Parse function definitions
    - Implement parse_function() for fn name(params) -> type { body }
    - Parse parameter lists with types
    - Parse return types
    - Parse function bodies as statement blocks
    - Handle naked function modifier
    - _Requirements: 2.2, 2.9_
  
  - [x] 3.4 Parse variable declarations and assignments
    - Implement parse_var_decl() for let [mut] name: type = expr;
    - Implement parse_assignment() for lvalue = expr;
    - Track mutability in AST
    - _Requirements: 2.3, 15.1, 15.2_
  
  - [x] 3.5 Parse control flow statements
    - Implement parse_if() for if (cond) { block } [else { block }]
    - Implement parse_while() for while (cond) { block }
    - Implement parse_return() for return [expr];
    - _Requirements: 2.4, 2.5, 2.15, 14.1, 14.2, 14.3_
  
  - [x] 3.6 Parse expressions with operator precedence
    - Implement parse_expression() with precedence climbing
    - Define operator precedence table
    - Parse binary operators: +, -, *, /, %, &, |, ^, <<, >>, ==, !=, <, >, <=, >=
    - Parse unary operators: -, !, *, &
    - _Requirements: 2.11, 2.12, 17.1-17.7_
  
  - [x] 3.7 Parse function calls and array indexing
    - Implement parse_call() for function(args)
    - Implement parse_array_index() for array[index]
    - Handle chained operations (e.g., array[i].field)
    - _Requirements: 2.14, 2.13_
  
  - [x] 3.8 Parse special constructs
    - Implement parse_asm_block() for asm { code }
    - Implement parse_syscall() for syscall(num, args...)
    - Implement parse_enc_block() for enc { "string" }
    - Implement parse_unsafe_block() for unsafe { code }
    - _Requirements: 2.6, 2.7, 2.8, 28.1_
  
  - [x]* 3.9 Write unit tests for parser
    - Test function parsing
    - Test expression parsing with correct precedence
    - Test control flow parsing
    - Test error recovery
    - _Requirements: 2.1-2.15_

- [x] 4. Implement type system and type checker
  - [x] 4.1 Define Type classes
    - Create Type base class with subclasses for primitives, pointers, arrays, structs, functions
    - Implement type equality and compatibility checking
    - Implement size_of() and align_of() for each type
    - _Requirements: 3.1, 3.2_
  
  - [x] 4.2 Implement symbol table
    - Create SymbolTable class with scope management
    - Implement enter_scope(), exit_scope(), define(), lookup()
    - Store variable mutability information
    - _Requirements: 3.3, 15.3_
  
  - [x] 4.3 Implement TypeChecker class
    - Create TypeChecker with AST visitor pattern
    - Build symbol table during first pass
    - Type check expressions during second pass
    - Collect type errors with source locations
    - _Requirements: 3.3, 3.6, 3.7, 10.3_
  
  - [x] 4.4 Add type checking for expressions
    - Check binary operations for type compatibility
    - Check unary operations
    - Check function call argument types
    - Check array indexing types
    - _Requirements: 3.3, 3.4, 3.6_
  
  - [x] 4.5 Add type checking for statements
    - Check variable declarations and initializers
    - Check assignments to mutable variables only
    - Check return statement types match function signature
    - Verify all code paths return values
    - _Requirements: 3.7, 15.3, 15.4, 14.5_
  
  - [x] 4.6 Add pointer and cast type checking
    - Check pointer arithmetic operations
    - Check dereference and address-of operations
    - Validate explicit casts
    - _Requirements: 3.4, 3.5, 8.1-8.4_
  
  - [ ]* 4.7 Write property test for type checker
    - **Property 1: Type preservation**
    - **Validates: Requirements 3.3**
    - For any well-typed expression, type checking should produce a consistent type
  
  - [ ]* 4.8 Write unit tests for type checker
    - Test type compatibility rules
    - Test mutability checking
    - Test pointer type checking
    - Test error reporting
    - _Requirements: 3.1-3.7_

- [x] 5. Checkpoint - Ensure lexer, parser, and type checker work together
  - Ensure all tests pass, ask the user if questions arise.

- [x] 6. Implement LLVM IR generation for basic constructs
  - [x] 6.1 Create IRGenerator class
    - Initialize LLVM context, module, and builder
    - Create symbol table mapping AST symbols to LLVM values
    - Implement visitor pattern for AST traversal
    - _Requirements: 9.1_
  
  - [x] 6.2 Generate LLVM types from Lwanga types
    - Map u8, u16, u32, u64 to LLVM integer types
    - Map ptr to LLVM pointer type
    - Map arrays to LLVM array types
    - Map structs to LLVM struct types
    - _Requirements: 3.1, 3.2_
  
  - [x] 6.3 Generate function declarations and definitions
    - Create LLVM function prototypes from AST functions
    - Generate function bodies with entry basic blocks
    - Allocate stack space for local variables
    - _Requirements: 9.1_
  
  - [x] 6.4 Generate IR for expressions
    - Generate IR for integer literals (constants)
    - Generate IR for binary operations (add, sub, mul, div, etc.)
    - Generate IR for unary operations (neg, not)
    - Generate IR for variable loads and stores
    - _Requirements: 9.1_
  
  - [x] 6.5 Generate IR for control flow
    - Generate IR for if statements (conditional branches)
    - Generate IR for while loops (loop basic blocks)
    - Generate IR for return statements
    - Ensure proper basic block termination
    - _Requirements: 14.4_
  
  - [x] 6.6 Generate IR for function calls
    - Generate IR for direct function calls
    - Pass arguments according to calling convention
    - Handle return values
    - _Requirements: 9.1_
  
  - [x]* 6.7 Write unit tests for IR generation
    - Test function generation
    - Test expression generation
    - Test control flow generation
    - Verify generated IR is valid
    - _Requirements: 9.1_

- [x] 7. Implement pointer operations and memory management
  - [x] 7.1 Generate IR for pointer operations
    - Generate IR for dereference (*ptr)
    - Generate IR for address-of (&var)
    - Generate IR for pointer arithmetic (ptr + offset)
    - _Requirements: 8.1, 8.2, 8.3, 8.5_
  
  - [x] 7.2 Generate IR for type casts
    - Implement pointer-to-integer casts
    - Implement integer-to-pointer casts
    - Implement pointer-to-pointer casts
    - _Requirements: 8.4_
  
  - [ ]* 7.3 Write property test for pointer operations
    - **Property 2: Pointer round-trip**
    - **Validates: Requirements 8.1, 8.2**
    - For any variable, taking its address and dereferencing should yield the original value

- [x] 8. Implement arrays and structs
  - [x] 8.1 Parse struct definitions
    - Implement parse_struct() for struct Name { fields }
    - Parse field declarations with types
    - Handle packed struct attribute
    - _Requirements: 20.1, 20.7_
  
  - [x] 8.2 Parse struct initialization and field access
    - Implement parse_struct_init() for Name { field: value, ... }
    - Implement parse_field_access() for struct.field
    - _Requirements: 20.2, 20.3_
  
  - [x] 8.3 Generate IR for struct types
    - Create LLVM struct types with correct field layout
    - Calculate field offsets
    - Handle packed structs
    - _Requirements: 20.4, 20.6_
  
  - [x] 8.4 Generate IR for struct operations
    - Generate IR for struct initialization
    - Generate IR for field access (GEP instructions)
    - Handle nested structs
    - _Requirements: 20.4, 20.5_
  
  - [x] 8.5 Parse array types and literals
    - Implement parse_array_type() for [type; size]
    - Implement parse_array_literal() for [val1, val2, ...]
    - _Requirements: 16.1, 16.2_
  
  - [x] 8.6 Generate IR for array operations
    - Allocate contiguous memory for arrays
    - Generate IR for array indexing (GEP instructions)
    - Handle array-to-pointer decay
    - _Requirements: 16.4, 16.5, 16.6_
  
  - [ ]* 8.7 Write unit tests for structs and arrays
    - Test struct layout and field access
    - Test array allocation and indexing
    - Test nested structures
    - _Requirements: 16.1-16.7, 20.1-20.7_

- [-] 9. Implement system call interface
  - [x] 9.1 Generate IR for syscall expressions
    - Parse syscall number and arguments
    - Generate platform-specific inline assembly for syscalls
    - Map arguments to correct registers (x86_64: rdi, rsi, rdx, r10, r8, r9)
    - Return syscall result as u64
    - _Requirements: 4.1, 4.2, 4.3, 4.5, 4.6_
  
  - [x] 9.2 Add platform-specific syscall implementations
    - Implement x86_64 Linux syscall (syscall instruction)
    - Implement ARM64 Linux syscall (svc #0 instruction)
    - Use LLVM target triple to select correct implementation
    - _Requirements: 4.4, 9.7_
  
  - [ ]* 9.3 Write unit tests for syscall generation
    - Test syscall IR generation for x86_64
    - Test syscall IR generation for ARM64
    - Verify register allocation
    - _Requirements: 4.1-4.6, 18.1-18.4_

- [x] 10. Implement inline assembly support
  - [x] 10.1 Generate IR for asm blocks
    - Extract assembly code string from AST
    - Generate LLVM inline assembly IR
    - Preserve assembly instructions exactly
    - _Requirements: 5.1, 5.2, 5.5_
  
  - [x] 10.2 Add register constraints for inline assembly
    - Support register inputs and outputs
    - Handle register clobbers
    - Support AT&T syntax
    - _Requirements: 5.3, 5.4_
  
  - [x]* 10.3 Write unit tests for inline assembly
    - Test asm block parsing
    - Test IR generation for inline assembly
    - Verify assembly preservation
    - _Requirements: 5.1-5.5_

- [x] 11. Implement naked functions
  - [x] 11.1 Add naked function attribute
    - Mark naked functions in AST during parsing
    - Apply LLVM "naked" attribute to function definitions
    - Disable prologue/epilogue generation
    - _Requirements: 6.1, 6.2, 6.3, 6.4_
  
  - [x]* 11.2 Write unit tests for naked functions
    - Test naked function parsing
    - Verify no prologue/epilogue in generated code
    - _Requirements: 6.1-6.4_

- [x] 12. Checkpoint - Ensure core compilation pipeline works
  - Ensure all tests pass, ask the user if questions arise.

- [x] 13. Implement encrypted string literals
  - [x] 13.1 Generate encryption for enc blocks
    - Generate random XOR key at compile time
    - Encrypt string bytes: encrypted[i] = plaintext[i] ^ key[i % key_len]
    - Store encrypted bytes as global array
    - _Requirements: 7.1, 7.2, 7.4_
  
  - [x] 13.2 Generate runtime decryption code
    - Generate decryption function in IR
    - Implement XOR decryption loop
    - Return pointer to decrypted buffer
    - _Requirements: 7.3, 7.5_
  
  - [x]* 13.3 Write property test for encrypted strings
    - **Property 3: Encryption round-trip**
    - **Validates: Requirements 7.1-7.5**
    - For any string, encrypting then decrypting should produce the original string

- [x] 14. Implement constants and compile-time evaluation
  - [x] 14.1 Parse constant declarations
    - Implement parse_const() for const NAME: type = value;
    - Store constants in symbol table
    - _Requirements: 22.1_
  
  - [x] 14.2 Implement constant expression evaluator
    - Evaluate arithmetic operations at compile time
    - Evaluate bitwise operations at compile time
    - Substitute constant values during IR generation
    - _Requirements: 22.2, 22.3, 22.4, 22.5_
  
  - [x]* 14.3 Write unit tests for constant evaluation
    - Test arithmetic constant expressions
    - Test bitwise constant expressions
    - Test constant substitution
    - _Requirements: 22.1-22.5_

- [x] 15. Implement function pointers
  - [x] 15.1 Parse function pointer types
    - Implement parse_function_pointer_type() for fn(params) -> return
    - Handle function pointer assignments
    - Parse indirect function calls
    - _Requirements: 21.1, 21.2, 21.3_
  
  - [x] 15.2 Generate IR for function pointers
    - Create LLVM function pointer types
    - Generate IR for function pointer assignments
    - Generate IR for indirect calls
    - Handle function pointer casts
    - _Requirements: 21.4, 21.5_
  
  - [x]* 15.3 Write unit tests for function pointers
    - Test function pointer type parsing
    - Test indirect calls
    - Test function pointer casts
    - _Requirements: 21.1-21.5_

- [x] 16. Implement register access
  - [x] 16.1 Parse register variable declarations
    - Recognize register keyword in variable declarations
    - Parse platform-specific register names
    - Store register information in symbol table
    - _Requirements: 11.1, 11.2_
  
  - [x] 16.2 Generate IR for register variables
    - Use LLVM register constraints
    - Generate IR for register reads
    - Generate IR for register writes
    - _Requirements: 11.3, 11.4_
  
  - [x]* 16.3 Write unit tests for register access
    - Test register variable parsing
    - Test register read/write IR generation
    - _Requirements: 11.1-11.4_

- [x] 17. Implement unsafe blocks
  - [x] 17.1 Parse unsafe blocks
    - Implement parse_unsafe_block() for unsafe { code }
    - Track unsafe context during type checking
    - _Requirements: 28.1_
  
  - [x] 17.2 Enforce unsafe operation restrictions
    - Allow unchecked pointer operations only in unsafe blocks
    - Allow inline assembly only in unsafe blocks or naked functions
    - Allow direct memory access only in unsafe blocks
    - Report errors for unsafe operations outside unsafe context
    - _Requirements: 28.2, 28.3, 28.4, 28.5_
  
  - [x]* 17.3 Write unit tests for unsafe blocks
    - Test unsafe block parsing
    - Test unsafe operation enforcement
    - Test error reporting for unsafe violations
    - _Requirements: 28.1-28.5_

- [x] 18. Implement module system
  - [x] 18.1 Parse import statements
    - Implement parse_import() for import "path";
    - Resolve module paths relative to source file
    - _Requirements: 23.1, 23.2_
  
  - [x] 18.2 Implement module loading and linking
    - Load and parse imported modules
    - Merge symbol tables from multiple modules
    - Detect circular imports
    - Handle module exports
    - _Requirements: 23.3, 23.4, 23.5_
  
  - [x]* 18.3 Write unit tests for module system
    - Test import parsing
    - Test module resolution
    - Test circular import detection
    - _Requirements: 23.1-23.5_

- [x] 19. Implement preprocessor
  - [x] 19.1 Add preprocessor directives to lexer
    - Recognize #if, #else, #endif, #define
    - Parse preprocessor conditions
    - _Requirements: 27.1_
  
  - [x] 19.2 Implement preprocessor evaluation
    - Evaluate preprocessor conditions before parsing
    - Support platform conditionals (PLATFORM_LINUX, PLATFORM_WINDOWS)
    - Support architecture conditionals (ARCH_X86_64, ARCH_ARM64)
    - Exclude code blocks when conditions are false
    - _Requirements: 27.2, 27.3, 27.4, 27.5_
  
  - [x]* 19.3 Write unit tests for preprocessor
    - Test conditional compilation
    - Test platform-specific code
    - Test architecture-specific code
    - _Requirements: 27.1-27.5_

- [x] 20. Implement LLVM backend and optimization
  - [x] 20.1 Create Backend class
    - Initialize LLVM target machine
    - Set up optimization pass manager
    - Configure code generation options
    - _Requirements: 9.2, 9.3_
  
  - [x] 20.2 Implement optimization passes
    - Configure -O0: no optimization
    - Configure -O1: basic optimizations (constant folding, DCE)
    - Configure -O2: moderate optimizations (inlining, loop opts)
    - Configure -O3: aggressive optimizations (vectorization)
    - _Requirements: 9.2, 12.3_
  
  - [x] 20.3 Implement static executable generation
    - Generate object code from LLVM IR
    - Link object code into freestanding executable
    - Ensure no libc or runtime dependencies
    - Minimize binary size
    - _Requirements: 9.3, 9.6, 13.1, 13.2, 13.3, 13.4_
  
  - [x] 20.4 Implement JIT compilation
    - Set up LLVM JIT engine
    - Compile IR to memory
    - Execute main function immediately
    - _Requirements: 9.4, 12.4_
  
  - [x] 20.5 Add position-independent code generation
    - Configure LLVM for PIC generation
    - Support --pic flag
    - _Requirements: 9.5_
  
  - [x] 20.6 Add multi-architecture support
    - Support x86_64 target
    - Support ARM64 target
    - Use target triple for platform selection
    - _Requirements: 9.7, 12.6, 26.2_
  
  - [x]* 20.7 Write unit tests for backend
    - Test optimization levels
    - Test static executable generation
    - Test JIT compilation
    - Test multi-architecture support
    - _Requirements: 9.1-9.7_

- [x] 21. Implement obfuscation passes
  - [x] 21.1 Add control flow flattening
    - Convert structured control flow to switch-based dispatch
    - Apply when --obfuscate flag is set
    - _Requirements: 24.1, 24.2_
  
  - [x] 21.2 Add opaque predicates
    - Insert always-true/false conditions
    - Make conditions hard to analyze statically
    - _Requirements: 24.3_
  
  - [x] 21.3 Add instruction substitution and reordering
    - Replace simple operations with complex equivalents
    - Randomize instruction ordering where possible
    - Preserve program semantics
    - _Requirements: 24.4, 24.5_
  
  - [x]* 21.4 Write unit tests for obfuscation
    - Test control flow flattening
    - Test opaque predicates
    - Verify semantic preservation
    - _Requirements: 24.1-24.5_

- [x] 22. Implement compiler driver and CLI
  - [x] 22.1 Implement command-line argument parsing
    - Parse input file paths
    - Parse -o output flag
    - Parse -O optimization level flags
    - Parse --jit, --target, -v, -g, --obfuscate, --pic flags
    - _Requirements: 12.1, 12.2, 12.3, 12.4, 12.5, 12.6_
  
  - [x] 22.2 Orchestrate compilation pipeline
    - Read source files
    - Run lexer, parser, type checker for each file
    - Merge ASTs from multiple files
    - Generate LLVM IR
    - Apply optimizations and obfuscation
    - Generate output (executable, object, or JIT)
    - _Requirements: 12.1-12.8_
  
  - [x] 22.3 Implement error reporting
    - Display errors in human-readable format
    - Show source location with line/column numbers
    - Display source context and error messages
    - Distinguish errors from warnings
    - Report all errors found during compilation
    - _Requirements: 10.1, 10.2, 10.3, 10.4, 10.5, 10.6_
  
  - [x] 22.4 Add verbose mode
    - Display compilation stages when -v is set
    - Show timing information for each stage
    - Display LLVM IR when requested
    - _Requirements: 12.5_
  
  - [x] 22.5 Add debugging information support
    - Generate DWARF debug info when -g is set
    - Map generated code to source line numbers
    - Strip debug symbols when -g is not set
    - _Requirements: 25.1, 25.2, 25.3, 25.4_
  
  - [x]* 22.6 Write integration tests for compiler driver
    - Test end-to-end compilation
    - Test various flag combinations
    - Test error reporting
    - _Requirements: 12.1-12.8_

- [x] 23. Implement cross-compilation support
  - [x] 23.1 Add target triple validation
    - Parse --target flag
    - Validate supported targets
    - Report errors for unsupported targets
    - _Requirements: 26.1, 26.5_
  
  - [x] 23.2 Implement platform-specific code generation
    - Generate platform-specific syscall interfaces
    - Use platform-specific calling conventions
    - Handle platform differences in IR generation
    - _Requirements: 26.2, 26.3, 26.4_
  
  - [x]* 23.3 Write unit tests for cross-compilation
    - Test target triple parsing
    - Test platform-specific code generation
    - _Requirements: 26.1-26.5_

- [x] 24. Checkpoint - Ensure complete compiler works end-to-end
  - Ensure all tests pass, ask the user if questions arise.

- [x] 25. Create example programs and documentation
  - [x] 25.1 Write example programs
    - Create hello_world.lwanga (syscall-based output)
    - Create tcp_server.lwanga (network socket example)
    - Create shellcode.lwanga (position-independent code example)
    - Create encrypted_strings.lwanga (enc block example)
    - _Requirements: 4.1, 7.1, 18.1, 18.2_
  
  - [x] 25.2 Document syscall numbers
    - Create syscall reference for Linux x86_64
    - Create syscall reference for Linux ARM64
    - Document common syscalls (read, write, open, close, socket, etc.)
    - _Requirements: 18.5_
  
  - [x] 25.3 Write language reference documentation
    - Document syntax and semantics
    - Document type system
    - Document special features (syscall, asm, enc, naked)
    - Document compiler flags and options
    - _Requirements: All requirements_

- [x] 26. Final integration and polish
  - [x] 26.1 Optimize compiler performance
    - Profile compilation stages
    - Optimize hot paths in lexer and parser
    - Reduce memory allocations
    - _Requirements: 9.1, 9.2_
  
  - [x] 26.2 Add comprehensive error messages
    - Improve error message clarity
    - Add suggestions for common mistakes
    - Add "did you mean?" suggestions for typos
    - _Requirements: 10.1-10.6_
  
  - [x] 26.3 Verify minimal binary size
    - Test that minimal programs produce <10KB executables
    - Optimize code generation for size
    - Strip unnecessary sections
    - _Requirements: 13.4_
  
  - [x]* 26.4 Write property test for compilation correctness
    - **Property 4: Compilation determinism**
    - **Validates: Requirements 9.1**
    - For any valid program, compiling twice should produce identical output (with fixed seeds)
  
  - [x]* 26.5 Write end-to-end integration tests
    - Test complete compilation pipeline
    - Test generated executables run correctly
    - Test cross-compilation
    - Test all language features together
    - _Requirements: All requirements_

- [x] 27. Final checkpoint - Complete compiler validation
  - Ensure all tests pass, ask the user if questions arise.

## Notes

- Tasks marked with `*` are optional testing tasks that can be skipped for faster MVP
- Each task references specific requirements for traceability
- The implementation follows a bottom-up approach: lexer → parser → type checker → IR generation → backend
- LLVM integration is established early to enable incremental testing of code generation
- Security-specific features (obfuscation, encrypted strings) are implemented after core functionality
- Checkpoints ensure the compiler remains functional at major milestones
- Property tests validate universal correctness properties across many inputs
- Unit tests validate specific examples, edge cases, and error conditions
