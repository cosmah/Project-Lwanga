# Lwanga Language Reference

Lwanga is a low-level systems programming language designed for security research, malware analysis, and systems programming. It compiles to native code via LLVM and provides direct access to system calls, inline assembly, and low-level memory operations.

## Table of Contents

1. [Basic Syntax](#basic-syntax)
2. [Types](#types)
3. [Functions](#functions)
4. [Control Flow](#control-flow)
5. [Operators](#operators)
6. [Pointers and Memory](#pointers-and-memory)
7. [Structs](#structs)
8. [Arrays](#arrays)
9. [Special Features](#special-features)
10. [Compiler Flags](#compiler-flags)

## Basic Syntax

### Comments

```lwanga
// Single-line comment

/* Multi-line
   comment */
```

### Variables

```lwanga
// Immutable variable
let x: u64 = 42;

// Mutable variable
let mut y: u64 = 10;
y = 20;  // OK

// Type inference (not yet implemented)
// let z = 42;  // Will be u64
```

### Constants

```lwanga
const PI: u64 = 314;
const MAX_SIZE: u64 = 1024 * 1024;
```

## Types

### Primitive Types

| Type | Description | Size |
|------|-------------|------|
| `u8` | Unsigned 8-bit integer | 1 byte |
| `u16` | Unsigned 16-bit integer | 2 bytes |
| `u32` | Unsigned 32-bit integer | 4 bytes |
| `u64` | Unsigned 64-bit integer | 8 bytes |
| `ptr` | Generic pointer | 8 bytes |

### Type Casts

```lwanga
let x: u64 = 42;
let p: ptr = x as ptr;
let y: u32 = x as u32;
```

### Function Pointer Types

```lwanga
// Function pointer type
let callback: fn(u64, u64) -> u64 = add;

// Indirect call
let result: u64 = callback(10, 20);
```

## Functions

### Function Definition

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

// No return value
fn print_hello() -> u64 {
    // ...
    return 0;
}
```

### Naked Functions

Naked functions have no prologue/epilogue, useful for shellcode:

```lwanga
naked fn shellcode() -> u64 {
    unsafe {
        asm {
            // Raw assembly code
            xor %rax, %rax
            ret
        }
    }
    return 0;
}
```

## Control Flow

### If Statements

```lwanga
if (x > 10) {
    // then block
} else {
    // else block
}

// Nested
if (x > 10) {
    if (y < 5) {
        // ...
    }
}
```

### While Loops

```lwanga
let mut i: u64 = 0;
while (i < 10) {
    // loop body
    i = i + 1;
}
```

### Return Statements

```lwanga
fn max(a: u64, b: u64) -> u64 {
    if (a > b) {
        return a;
    }
    return b;
}
```

## Operators

### Arithmetic Operators

```lwanga
let sum: u64 = a + b;      // Addition
let diff: u64 = a - b;     // Subtraction
let prod: u64 = a * b;     // Multiplication
let quot: u64 = a / b;     // Division
let rem: u64 = a % b;      // Modulo
```

### Comparison Operators

```lwanga
a == b  // Equal
a != b  // Not equal
a < b   // Less than
a > b   // Greater than
a <= b  // Less than or equal
a >= b  // Greater than or equal
```

### Bitwise Operators

```lwanga
a & b   // Bitwise AND
a | b   // Bitwise OR
a ^ b   // Bitwise XOR
a << b  // Left shift
a >> b  // Right shift
```

### Unary Operators

```lwanga
-x      // Negation
!x      // Bitwise NOT
*ptr    // Dereference
&var    // Address-of
```

## Pointers and Memory

### Pointer Operations

```lwanga
let x: u64 = 42;
let p: ptr = &x;           // Take address
let y: u64 = *p as u64;    // Dereference

// Pointer arithmetic
let next: ptr = (p as u64 + 8) as ptr;
```

### Unsafe Blocks

Operations that can cause undefined behavior must be in `unsafe` blocks:

```lwanga
unsafe {
    let value: u64 = *ptr as u64;  // Dereference
    *ptr = 42 as ptr;               // Write through pointer
}
```

## Structs

### Struct Definition

```lwanga
struct Point {
    x: u64,
    y: u64
}

// Packed struct (no padding)
packed struct Header {
    magic: u32,
    version: u16,
    flags: u16
}
```

### Struct Initialization

```lwanga
let p: Point = Point {
    x: 10,
    y: 20
};
```

### Field Access

```lwanga
let x_coord: u64 = p.x;
p.y = 30;
```

## Arrays

### Array Types

```lwanga
// Fixed-size array
let arr: [u64; 5] = [1, 2, 3, 4, 5];

// Array indexing
let first: u64 = arr[0];
arr[1] = 42;
```

## Special Features

### System Calls

Direct system call interface:

```lwanga
unsafe {
    let result: u64 = syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}
```

Example:

```lwanga
fn write(fd: u64, buf: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(1, fd, buf, count);
    }
}
```

### Inline Assembly

```lwanga
unsafe {
    asm {
        movq $42, %rax
        movq %rax, %rdi
        syscall
    }
}
```

Extended assembly with operands:

```lwanga
unsafe {
    let input: u64 = 42;
    let mut output: u64 = 0;
    
    asm {
        movq $1, $0
    } : "=r"(output) : "r"(input) : "rax";
}
```

### Encrypted Strings

Strings encrypted at compile time, decrypted at runtime:

```lwanga
unsafe {
    let secret: ptr = enc { "Hidden string" };
    write(1, secret, 13);
}
```

The string is XOR-encrypted with a random key at compile time and does not appear in plaintext in the binary.

### Register Variables

Direct access to CPU registers:

```lwanga
unsafe {
    let register rax: u64 = 42;
    let value: u64 = rax;
}
```

### Preprocessor

Conditional compilation:

```lwanga
#if ARCH_X86_64
fn syscall_exit(code: u64) -> u64 {
    unsafe { return syscall(60, code, 0, 0); }
}
#endif

#if ARCH_ARM64
fn syscall_exit(code: u64) -> u64 {
    unsafe { return syscall(93, code, 0, 0); }
}
#endif
```

Available defines:
- `PLATFORM_LINUX`
- `PLATFORM_WINDOWS`
- `ARCH_X86_64`
- `ARCH_ARM64`

### Module System

```lwanga
// Import another module
import "utils.lwanga";

// Use functions from imported module
fn main() -> u64 {
    return helper_function();
}
```

## Compiler Flags

### Basic Flags

```bash
lwangac input.lwanga -o output        # Compile to executable
lwangac input.lwanga -O2 -o output    # With optimization
```

### Optimization Levels

- `-O0`: No optimization (default)
- `-O1`: Basic optimizations
- `-O2`: Moderate optimizations
- `-O3`: Aggressive optimizations

### Code Generation

- `--jit`: Execute immediately with JIT compilation
- `--pic`: Generate position-independent code
- `--target <triple>`: Cross-compile for target (e.g., `x86_64-linux-gnu`, `aarch64-linux-gnu`)

### Debug and Analysis

- `-g`: Generate debug information (DWARF)
- `-v`: Verbose output with timing
- `--emit-llvm`: Display generated LLVM IR

### Security Features

- `--obfuscate`: Enable code obfuscation (control flow flattening, opaque predicates)

### Examples

```bash
# Basic compilation
lwangac hello.lwanga -o hello

# Optimized build
lwangac server.lwanga -O3 -o server

# Position-independent shellcode
lwangac shellcode.lwanga --pic -o shellcode

# Cross-compile for ARM64
lwangac program.lwanga --target aarch64-linux-gnu -o program-arm64

# JIT execution
lwangac script.lwanga --jit

# Debug build with verbose output
lwangac program.lwanga -g -v -o program-debug

# Obfuscated build
lwangac malware.lwanga --obfuscate -O3 -o malware
```

## Best Practices

### 1. Use Unsafe Blocks Appropriately

Only use `unsafe` for operations that truly need it:

```lwanga
// Good: Unsafe only where needed
fn read_byte(p: ptr) -> u64 {
    unsafe {
        return *p as u64;
    }
}

// Bad: Unnecessary unsafe
unsafe {
    let x: u64 = 42;  // This doesn't need unsafe
}
```

### 2. Check Syscall Return Values

```lwanga
let fd: u64 = open(filename, O_RDONLY, 0);
if (fd > 18446744073709551615 - 4096) {
    // Error handling
    return 1;
}
```

### 3. Use Constants for Magic Numbers

```lwanga
const BUFFER_SIZE: u64 = 4096;
const MAX_CONNECTIONS: u64 = 100;

let buffer: [u8; BUFFER_SIZE];
```

### 4. Prefer Structs for Related Data

```lwanga
struct Config {
    port: u16,
    max_clients: u64,
    timeout: u64
}

let cfg: Config = Config {
    port: 8080,
    max_clients: 100,
    timeout: 30
};
```

## Examples

See the `examples/` directory for complete programs:

- `hello_world.lwanga` - Basic syscall usage
- `tcp_server.lwanga` - Network programming
- `shellcode.lwanga` - Position-independent code
- `encrypted_strings.lwanga` - String encryption

## Resources

- [Syscall Reference](syscalls.md)
- [LLVM Documentation](https://llvm.org/docs/)
- [Linux System Call Table](https://filippo.io/linux-syscall-table/)
