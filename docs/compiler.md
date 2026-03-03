# Lwanga Compiler Guide

**Complete reference for the `lwangac` compiler**

## Table of Contents

1. [Overview](#overview)
2. [Command Line Options](#command-line-options)
3. [Compilation Process](#compilation-process)
4. [Optimization Levels](#optimization-levels)
5. [Cross-Compilation](#cross-compilation)
6. [Advanced Features](#advanced-features)
7. [Troubleshooting](#troubleshooting)

---

## Overview

`lwangac` is the Lwanga compiler, built on LLVM 18. It compiles Lwanga source code to native machine code for x86_64 and ARM64 architectures.

### Basic Usage

```bash
lwangac input.lwanga -o output
```

### Quick Examples

```bash
# Compile with default settings
lwangac hello.lwanga -o hello

# Compile with optimization
lwangac hello.lwanga -o hello -O2

# Compile with debug information
lwangac hello.lwanga -o hello -g

# Verbose output
lwangac hello.lwanga -o hello -v
```

---

## Command Line Options

### Input/Output

| Option | Description | Example |
|--------|-------------|---------|
| `<input>` | Input source file (required) | `hello.lwanga` |
| `-o <file>` | Output executable name | `-o hello` |

### Optimization

| Option | Description | Default |
|--------|-------------|---------|
| `-O0` | No optimization | ✓ |
| `-O1` | Basic optimization | |
| `-O2` | Moderate optimization | |
| `-O3` | Aggressive optimization | |

### Debug and Information

| Option | Description |
|--------|-------------|
| `-g` | Generate debug information |
| `-v` | Verbose output |
| `--emit-llvm` | Display generated LLVM IR |

### Code Generation

| Option | Description |
|--------|-------------|
| `--pic` | Generate position-independent code |
| `--obfuscate` | Enable code obfuscation |
| `--target <triple>` | Cross-compilation target |

### Execution

| Option | Description |
|--------|-------------|
| `--jit` | Execute code immediately using JIT |

---

## Compilation Process

### Stages

1. **Preprocessing** - Handle `#if`, `#define` directives
2. **Lexical Analysis** - Convert source to tokens
3. **Parsing** - Build Abstract Syntax Tree (AST)
4. **Type Checking** - Verify type correctness
5. **IR Generation** - Generate LLVM Intermediate Representation
6. **Optimization** - Apply optimization passes
7. **Code Generation** - Produce native machine code

### Example with Verbose Output

```bash
lwangac -v hello.lwanga -o hello
```

Output:
```
Lwanga Compiler v1.0.3
Input file: hello.lwanga
Output file: hello
Optimization level: -O0

[1/6] Loading modules...
  Loaded in 0ms

[2/6] Type checking...
  Type checked in 0ms

[3/6] Generating LLVM IR...
  Generated IR in 0ms

[4/6] Setting up backend...
  Backend configured

[5/6] Optimizing...
  Completed in 0ms

[6/6] Generating executable...
  Generated in 73ms

Compilation successful!
Output: hello
Total time: 75ms
```

---

## Optimization Levels

### -O0 (No Optimization)

**Use for**: Development, debugging

```bash
lwangac program.lwanga -o program -O0
```

**Characteristics**:
- Fastest compilation
- Largest binary size
- Slowest execution
- Best for debugging

### -O1 (Basic Optimization)

**Use for**: Quick builds with some optimization

```bash
lwangac program.lwanga -o program -O1
```

**Characteristics**:
- Fast compilation
- Moderate binary size
- Moderate execution speed
- Good balance

### -O2 (Moderate Optimization)

**Use for**: Production builds (recommended)

```bash
lwangac program.lwanga -o program -O2
```

**Characteristics**:
- Slower compilation
- Smaller binary size
- Fast execution
- **Recommended for release**

### -O3 (Aggressive Optimization)

**Use for**: Performance-critical code

```bash
lwangac program.lwanga -o program -O3
```

**Characteristics**:
- Slowest compilation
- Smallest binary size
- Fastest execution
- May increase code size in some cases

### Optimization Comparison

| Level | Compile Time | Binary Size | Execution Speed |
|-------|--------------|-------------|-----------------|
| -O0 | Fastest | Largest | Slowest |
| -O1 | Fast | Large | Moderate |
| -O2 | Moderate | Small | Fast |
| -O3 | Slow | Smallest | Fastest |

---

## Cross-Compilation

### Supported Targets

- `x86_64-linux-gnu` - Linux x86_64
- `aarch64-linux-gnu` - Linux ARM64
- `x86_64-apple-darwin` - macOS x86_64
- `aarch64-apple-darwin` - macOS ARM64 (Apple Silicon)

### Cross-Compile Example

```bash
# Compile for ARM64 on x86_64 machine
lwangac program.lwanga -o program-arm64 --target aarch64-linux-gnu

# Compile for x86_64 on ARM64 machine
lwangac program.lwanga -o program-x64 --target x86_64-linux-gnu
```

### Verify Target

```bash
file program-arm64
# Output: program-arm64: ELF 64-bit LSB executable, ARM aarch64, ...
```

---

## Advanced Features

### Position-Independent Code (PIC)

Generate shellcode or shared libraries:

```bash
lwangac shellcode.lwanga -o shellcode --pic
```

**Use cases**:
- Shellcode development
- Shared libraries
- Security research

### Code Obfuscation

Obfuscate control flow and constants:

```bash
lwangac program.lwanga -o program --obfuscate
```

**Features**:
- Control flow flattening
- Constant encryption
- Dead code insertion
- String obfuscation

### JIT Execution

Execute code immediately without creating a file:

```bash
lwangac program.lwanga --jit
```

**Use cases**:
- Quick testing
- Scripting
- Interactive development

### Debug Information

Generate debug symbols for GDB/LLDB:

```bash
lwangac program.lwanga -o program -g
```

Then debug:
```bash
gdb ./program
# or
lldb ./program
```

### LLVM IR Output

View generated LLVM Intermediate Representation:

```bash
lwangac program.lwanga --emit-llvm
```

**Use cases**:
- Understanding compiler behavior
- Optimization analysis
- Learning LLVM

---

## Compilation Examples

### Example 1: Simple Program

```lwanga
// hello.lwanga
fn main() -> u64 {
    return 0;
}
```

```bash
lwangac hello.lwanga -o hello
./hello
echo $?  # Prints: 0
```

### Example 2: Optimized Release Build

```bash
lwangac program.lwanga -o program -O2
strip program  # Remove debug symbols
```

### Example 3: Debug Build

```bash
lwangac program.lwanga -o program -g -O0
gdb ./program
```

### Example 4: Shellcode

```bash
lwangac shellcode.lwanga -o shellcode --pic -O2
objcopy -O binary -j .text shellcode shellcode.bin
```

### Example 5: Cross-Compile for ARM

```bash
lwangac program.lwanga -o program-arm --target aarch64-linux-gnu -O2
```

---

## Error Messages

### Syntax Errors

```
error[E0001]: unexpected token
  --> program.lwanga:5:10
   |
5  |     let x = ;
   |             ^ expected expression
```

### Type Errors

```
error[E0002]: type mismatch
  --> program.lwanga:8:15
   |
8  |     let x: u32 = "string";
   |                  ^^^^^^^^ expected u32, found ptr
```

### Undefined Variable

```
error[E0003]: undefined identifier 'x'
  --> program.lwanga:10:15
   |
10 |     let y = x + 5;
   |             ^ undefined variable
   |
   = help: did you mean 'z'?
```

---

## Performance Tips

### 1. Use Appropriate Optimization Level

```bash
# Development
lwangac program.lwanga -o program -O0

# Production
lwangac program.lwanga -o program -O2
```

### 2. Profile Before Optimizing

```bash
# Compile with profiling
lwangac program.lwanga -o program -O2 -g

# Run with profiler
perf record ./program
perf report
```

### 3. Use Constants

```lwanga
// Good: Compiler can optimize
const BUFFER_SIZE: u64 = 1024;

// Bad: Runtime value
let buffer_size: u64 = 1024;
```

### 4. Minimize Unsafe Blocks

```lwanga
// Good: Small unsafe block
fn write(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, 1, msg, len);
    }
}

// Bad: Large unsafe block
unsafe fn process() -> u64 {
    // 100 lines of code...
}
```

---

## Troubleshooting

### Compilation Fails

**Problem**: `error: LLVM not found`

**Solution**:
```bash
# Ubuntu/Debian
sudo apt install llvm-18-dev

# Set LLVM path
export LLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm
```

### Binary Too Large

**Problem**: Executable is larger than expected

**Solutions**:
```bash
# Use optimization
lwangac program.lwanga -o program -O2

# Strip debug symbols
strip program

# Use PIC for smaller code
lwangac program.lwanga -o program --pic -O2
```

### Slow Compilation

**Problem**: Compilation takes too long

**Solutions**:
```bash
# Use lower optimization level
lwangac program.lwanga -o program -O1

# Disable obfuscation
lwangac program.lwanga -o program  # Don't use --obfuscate
```

### Runtime Errors

**Problem**: Program crashes or behaves unexpectedly

**Solutions**:
```bash
# Compile with debug info
lwangac program.lwanga -o program -g -O0

# Debug with GDB
gdb ./program
(gdb) run
(gdb) backtrace
```

---

## Compiler Internals

### Architecture

```
Source Code (.lwanga)
    ↓
Preprocessor (#if, #define)
    ↓
Lexer (Tokens)
    ↓
Parser (AST)
    ↓
Type Checker
    ↓
IR Generator (LLVM IR)
    ↓
Optimizer (LLVM Passes)
    ↓
Backend (Machine Code)
    ↓
Executable
```

### LLVM Integration

Lwanga uses LLVM 18 for:
- Intermediate representation
- Optimization passes
- Code generation
- Cross-compilation support

---

## Environment Variables

| Variable | Description | Example |
|----------|-------------|---------|
| `LLVM_DIR` | LLVM installation path | `/usr/lib/llvm-18` |
| `LWANGA_PATH` | Additional include paths | `/usr/local/lib/lwanga` |

---

## Exit Codes

| Code | Meaning |
|------|---------|
| 0 | Success |
| 1 | Compilation error |
| 2 | Invalid arguments |
| 3 | File not found |
| 4 | LLVM error |

---

## See Also

- [Tutorial](tutorial.md) - Learn Lwanga
- [Language Reference](language-reference.md) - Complete specification
- [Formatter Guide](formatter.md) - Code formatting
- [Examples](../examples/) - Sample programs

---

**Back to**: [Documentation Index](README.md) | [Main README](../README.md)
