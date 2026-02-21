# Lwanga Compiler

This directory contains the Lwanga compiler implementation written in C++ using LLVM.

## Structure

- `src/` - Source code for the compiler
  - `lexer/` - Lexical analysis (tokenization)
  - `parser/` - Syntax analysis (AST construction)
  - `ast/` - Abstract Syntax Tree node definitions
  - `types/` - Type system and type checker
  - `codegen/` - LLVM IR generation
  - `backend/` - LLVM backend integration and optimization
  - `driver/` - Compiler driver and CLI
- `include/` - Public header files
- `tests/` - Unit tests and integration tests
- `CMakeLists.txt` - CMake build configuration

## Building

```bash
mkdir build && cd build
cmake ..
make
```

## Usage

```bash
./lwangac input.lwanga -o output
```
