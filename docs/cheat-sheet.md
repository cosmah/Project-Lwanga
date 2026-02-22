# Lwanga Cheat Sheet

**Quick reference for Lwanga syntax and common patterns**

## Basic Syntax

### Program Structure

```lwanga
fn main() -> u64 {
    return 0;
}
```

### Comments

```lwanga
// Single-line comment
/* Multi-line comment */
```

---

## Variables and Types

### Variable Declaration

```lwanga
let x: u64 = 42;           // Immutable
let mut y: u64 = 10;       // Mutable
y = 20;                     // OK
```

### Types

| Type | Description | Range |
|------|-------------|-------|
| `u8` | 8-bit unsigned | 0 to 255 |
| `u16` | 16-bit unsigned | 0 to 65,535 |
| `u32` | 32-bit unsigned | 0 to 4,294,967,295 |
| `u64` | 64-bit unsigned | 0 to 18,446,744,073,709,551,615 |
| `ptr` | Pointer | Memory address |
| `register` | Register | CPU register |

### Constants

```lwanga
const MAX_SIZE: u64 = 1024;
const BUFFER_LEN: u32 = 512;
```

---

## Functions

### Basic Function

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}
```

### Naked Function

```lwanga
naked fn shellcode() -> u64 {
    asm {
        // No prologue/epilogue
    }
}
```

---

## Control Flow

### If Statement

```lwanga
if x > 5 {
    return 1;
}
```

### If-Else

```lwanga
if x > 5 {
    return 1;
} else {
    return 0;
}
```

### While Loop

```lwanga
while counter < 10 {
    counter = counter + 1;
}
```

---

## Operators

### Arithmetic

```lwanga
a + b    // Addition
a - b    // Subtraction
a * b    // Multiplication
a / b    // Division
a % b    // Modulo
```

### Comparison

```lwanga
a == b   // Equal
a != b   // Not equal
a < b    // Less than
a > b    // Greater than
a <= b   // Less than or equal
a >= b   // Greater than or equal
```

### Logical

```lwanga
a && b   // AND
a || b   // OR
!a       // NOT
```

### Bitwise

```lwanga
a & b    // AND
a | b    // OR
a ^ b    // XOR
~a       // NOT
a << b   // Left shift
a >> b   // Right shift
```

---

## Pointers

### Basic Pointers

```lwanga
let x: u64 = 42;
let ptr: ptr = &x;         // Address of x
let value: u64 = *ptr;     // Dereference
```

### String Pointers

```lwanga
let msg: ptr = "Hello";
let second: ptr = msg + 1; // Pointer arithmetic
```

---

## Structs

### Definition

```lwanga
struct Point {
    x: u64,
    y: u64,
}
```

### Usage

```lwanga
let p: Point = Point { x: 10, y: 20 };
let x_val: u64 = p.x;
```

### Packed Struct

```lwanga
packed struct Header {
    magic: u32,
    version: u16,
}
```

---

## Unsafe Operations

### Unsafe Block

```lwanga
unsafe {
    let result: u64 = syscall(1, 1, msg, len);
}
```

---

## System Calls

### Common Syscalls (Linux x86_64)

| Syscall | Number | Arguments |
|---------|--------|-----------|
| read | 0 | fd, buf, count |
| write | 1 | fd, buf, count |
| open | 2 | filename, flags, mode |
| close | 3 | fd |
| stat | 4 | filename, statbuf |
| mmap | 9 | addr, length, prot, flags, fd, offset |
| exit | 60 | status |
| socket | 41 | domain, type, protocol |
| connect | 42 | sockfd, addr, addrlen |
| accept | 43 | sockfd, addr, addrlen |
| bind | 49 | sockfd, addr, addrlen |
| listen | 50 | sockfd, backlog |

### Write to stdout

```lwanga
let msg: ptr = "Hello!\n";
unsafe {
    syscall(1, 1, msg, 7);
}
```

### Exit Program

```lwanga
unsafe {
    syscall(60, 0, 0, 0);
}
```

---

## Inline Assembly

### Basic Assembly

```lwanga
unsafe {
    asm {
        mov rax, 60
        xor rdi, rdi
        syscall
    }
}
```

### With Variables

```lwanga
let result: u64 = 0;
unsafe {
    asm {
        mov rax, 39      // sys_getpid
        syscall
        mov result, rax
    }
}
```

---

## Advanced Features

### Encrypted Strings

```lwanga
let secret: ptr = enc "password123";
```

### Imports

```lwanga
import "std/io.lwanga";
import "lib/crypto.lwanga";
```

### Preprocessor

```lwanga
#if DEBUG
const LOG_LEVEL: u64 = 3;
#else
const LOG_LEVEL: u64 = 1;
#endif
```

---

## Common Patterns

### String Length

```lwanga
fn strlen(s: ptr) -> u64 {
    let mut len: u64 = 0;
    let mut p: ptr = s;
    while *p != 0 {
        len = len + 1;
        p = p + 1;
    }
    return len;
}
```

### Memory Copy

```lwanga
fn memcpy(dest: ptr, src: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    while i < n {
        *(dest + i) = *(src + i);
        i = i + 1;
    }
    return n;
}
```

### Error Handling

```lwanga
fn safe_divide(a: u64, b: u64) -> u64 {
    if b == 0 {
        return 0;  // Error
    }
    return a / b;
}
```

---

## Compiler Commands

### Basic Compilation

```bash
lwangac input.lwanga -o output
```

### Optimization Levels

```bash
lwangac input.lwanga -o output -O0  # No optimization
lwangac input.lwanga -o output -O1  # Basic
lwangac input.lwanga -o output -O2  # Moderate (recommended)
lwangac input.lwanga -o output -O3  # Aggressive
```

### Debug Build

```bash
lwangac input.lwanga -o output -g
```

### Position-Independent Code

```bash
lwangac input.lwanga -o output --pic
```

### Cross-Compilation

```bash
lwangac input.lwanga -o output --target aarch64-linux-gnu
```

---

## Formatter Commands

### Format File

```bash
lwangafmt --write myfile.lwanga
```

### Format Directory

```bash
lwangafmt --write --recursive src/
```

### Add Logo Header

```bash
lwangafmt --add-header --logo-style=minimal myfile.lwanga
```

### Check Formatting

```bash
lwangafmt --check src/**/*.lwanga
```

---

## Highlighter Commands

### Display File

```bash
lwangacat myfile.lwanga
```

### With Line Numbers

```bash
lwangacat --line-numbers myfile.lwanga
```

### Specific Theme

```bash
lwangacat --theme monokai myfile.lwanga
```

### Specific Lines

```bash
lwangacat --lines 10-20 myfile.lwanga
```

---

## Common Constants

### File Descriptors

```lwanga
const STDIN: u64 = 0;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;
```

### File Flags

```lwanga
const O_RDONLY: u32 = 0;
const O_WRONLY: u32 = 1;
const O_RDWR: u32 = 2;
const O_CREAT: u32 = 64;
const O_TRUNC: u32 = 512;
```

### Socket Types

```lwanga
const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;
const SOCK_DGRAM: u32 = 2;
```

---

## Example Programs

### Hello World

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 0;
}
```

### Fibonacci

```lwanga
fn fibonacci(n: u64) -> u64 {
    if n <= 1 {
        return n;
    }
    let mut a: u64 = 0;
    let mut b: u64 = 1;
    let mut i: u64 = 2;
    while i <= n {
        let temp: u64 = a + b;
        a = b;
        b = temp;
        i = i + 1;
    }
    return b;
}

fn main() -> u64 {
    return fibonacci(10);  // Returns 55
}
```

### Simple Shellcode

```lwanga
naked fn shellcode() -> u64 {
    asm {
        mov rax, 60      // sys_exit
        xor rdi, rdi     // status = 0
        syscall
    }
}

fn main() -> u64 {
    shellcode();
    return 0;
}
```

---

## Quick Tips

1. **Use constants** for magic numbers
2. **Minimize unsafe blocks** - keep them small
3. **Check return values** from syscalls
4. **Use descriptive names** for variables and functions
5. **Format code** with `lwangafmt`
6. **Test thoroughly** before deploying
7. **Document complex logic** with comments
8. **Validate input** before processing
9. **Clear sensitive data** after use
10. **Use encrypted strings** for secrets

---

## Resources

- [Tutorial](tutorial.md) - Complete learning guide
- [Language Reference](language-reference.md) - Full specification
- [Compiler Guide](compiler.md) - Compiler options
- [Syscall Reference](syscalls.md) - System calls
- [Examples](../examples/) - Sample programs

---

**Back to**: [Documentation Index](README.md) | [Main README](../README.md)
