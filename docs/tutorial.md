# Lwanga Tutorial

**This tutorial has been reorganized into a comprehensive 40-lesson series!**

For the complete, detailed tutorial with 40 in-depth lessons covering everything from basics to advanced security tool development, please visit:

## 📚 [Complete Tutorial Series](tutorial/README.md)

The new tutorial includes:
- 40 detailed lessons
- Hands-on examples for every concept
- Real-world projects
- Exercises with solutions
- Design decision explanations
- Professional coding practices

---

## Quick Start (Legacy Content)

Below is a condensed version. For the full experience, use the [new tutorial series](tutorial/README.md).

## Table of Contents

1. [Getting Started](#getting-started)
2. [Basic Syntax](#basic-syntax)
3. [Variables and Types](#variables-and-types)
4. [Functions](#functions)
5. [Control Flow](#control-flow)
6. [Pointers and Memory](#pointers-and-memory)
7. [Structs](#structs)
8. [Unsafe Operations](#unsafe-operations)
9. [System Calls](#system-calls)
10. [Inline Assembly](#inline-assembly)
11. [Advanced Features](#advanced-features)
12. [Security Tools](#security-tools)

---

## Getting Started

### Prerequisites

- Lwanga compiler installed ([Installation Guide](../INSTALL.md))
- Basic understanding of systems programming
- Text editor or IDE with Lwanga support

### Your First Program

Create a file named `hello.lwanga`:

```lwanga
fn main() -> u64 {
    return 0;
}
```

Compile and run:

```bash
lwangac hello.lwanga -o hello
./hello
echo $?  # Should print 0
```

**Congratulations!** You've written your first Lwanga program.

---

## Basic Syntax

### Comments

```lwanga
// Single-line comment

/*
 * Multi-line comment
 * Useful for documentation
 */
```

### Program Structure

Every Lwanga program must have a `main` function:

```lwanga
fn main() -> u64 {
    // Your code here
    return 0;
}
```

The `main` function:
- Returns `u64` (unsigned 64-bit integer)
- Return value becomes the program's exit code
- `0` typically means success

---

## Variables and Types

### Declaring Variables

```lwanga
fn main() -> u64 {
    let x: u64 = 42;           // Immutable variable
    let mut y: u64 = 10;       // Mutable variable
    y = 20;                     // OK: y is mutable
    
    return 0;
}
```

### Integer Types

```lwanga
let a: u8 = 255;               // 8-bit unsigned (0-255)
let b: u16 = 65535;            // 16-bit unsigned (0-65535)
let c: u32 = 4294967295;       // 32-bit unsigned
let d: u64 = 18446744073709551615;  // 64-bit unsigned
```

### Pointer Type

```lwanga
let msg: ptr = "Hello";        // Pointer to string data
let num_ptr: ptr = &x;         // Pointer to variable
```

### Constants

```lwanga
const MAX_SIZE: u64 = 1024;
const BUFFER_LEN: u32 = 512;

fn main() -> u64 {
    let size: u64 = MAX_SIZE;
    return 0;
}
```

---

## Functions

### Function Definition

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let result: u64 = add(5, 3);
    return result;  // Returns 8
}
```

### Multiple Parameters

```lwanga
fn calculate(x: u64, y: u64, z: u64) -> u64 {
    return (x + y) * z;
}

fn main() -> u64 {
    let value: u64 = calculate(2, 3, 4);
    return value;  // Returns 20
}
```

### No Return Value

Functions that don't return a value still need `-> u64` but can return 0:

```lwanga
fn print_number(n: u64) -> u64 {
    // Print logic here
    return 0;
}
```

---

## Control Flow

### If Statements

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        return 1;
    }
    
    return 0;
}
```

### If-Else

```lwanga
fn main() -> u64 {
    let x: u64 = 3;
    
    if x > 5 {
        return 1;
    } else {
        return 0;
    }
}
```

### While Loops

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    while counter < 10 {
        counter = counter + 1;
    }
    
    return counter;  // Returns 10
}
```

### Comparison Operators

```lwanga
x == y    // Equal
x != y    // Not equal
x < y     // Less than
x > y     // Greater than
x <= y    // Less than or equal
x >= y    // Greater than or equal
```

### Logical Operators

```lwanga
x && y    // Logical AND
x || y    // Logical OR
!x        // Logical NOT
```

---

## Pointers and Memory

### Pointer Basics

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;         // Get address of x
    
    return 0;
}
```

### String Pointers

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    // msg points to the string data
    
    return 0;
}
```

### Pointer Arithmetic

```lwanga
fn main() -> u64 {
    let msg: ptr = "ABC";
    let second: ptr = msg + 1;  // Points to 'B'
    
    return 0;
}
```

---

## Structs

### Defining Structs

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn main() -> u64 {
    let p: Point = Point { x: 10, y: 20 };
    return p.x;  // Returns 10
}
```

### Packed Structs

For precise memory layout control:

```lwanga
packed struct Header {
    magic: u32,
    version: u16,
    flags: u16,
}

fn main() -> u64 {
    let h: Header = Header {
        magic: 0x12345678,
        version: 1,
        flags: 0,
    };
    return 0;
}
```

---

## Unsafe Operations

### Unsafe Blocks

Operations that bypass safety checks must be in `unsafe` blocks:

```lwanga
fn main() -> u64 {
    unsafe {
        // Unsafe operations here
        let result: u64 = syscall(1, 1, 0, 0);
    }
    return 0;
}
```

### When to Use Unsafe

- System calls
- Raw pointer dereferencing
- Inline assembly
- Direct memory access

---

## System Calls

### Basic Syscall

```lwanga
fn main() -> u64 {
    unsafe {
        // syscall(number, arg1, arg2, arg3)
        let result: u64 = syscall(60, 0, 0, 0);  // exit(0)
    }
    return 0;
}
```

### Write to stdout

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    let len: u64 = 7;
    
    unsafe {
        // write(fd=1, buf=msg, count=len)
        syscall(1, 1, msg, len);
    }
    
    return 0;
}
```

### Common Syscalls (Linux x86_64)

| Syscall | Number | Arguments |
|---------|--------|-----------|
| read | 0 | fd, buf, count |
| write | 1 | fd, buf, count |
| open | 2 | filename, flags, mode |
| close | 3 | fd |
| exit | 60 | status |

**Full reference**: [syscalls.md](syscalls.md)

---

## Inline Assembly

### Basic Assembly

```lwanga
fn get_pid() -> u64 {
    let pid: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 39      // sys_getpid
            syscall
            mov pid, rax     // Store result
        }
    }
    
    return pid;
}
```

### Naked Functions

Functions without prologue/epilogue:

```lwanga
naked fn shellcode() -> u64 {
    asm {
        mov rax, 60      // sys_exit
        xor rdi, rdi     // status = 0
        syscall
    }
}
```

### Intel vs AT&T Syntax

Lwanga supports both:

```lwanga
// Intel syntax (default)
asm {
    mov rax, 60
    syscall
}

// AT&T syntax
asm {
    movq $60, %rax
    syscall
}
```

---

## Advanced Features

### Encrypted Strings

Compile-time string encryption:

```lwanga
fn main() -> u64 {
    let secret: ptr = enc "password123";
    // String is encrypted in the binary
    
    return 0;
}
```

### Position-Independent Code

```bash
# Compile with PIC
lwangac shellcode.lwanga -o shellcode --pic
```

### Preprocessor Directives

```lwanga
#if DEBUG
const LOG_LEVEL: u64 = 3;
#else
const LOG_LEVEL: u64 = 1;
#endif

fn main() -> u64 {
    return LOG_LEVEL;
}
```

### Imports

```lwanga
import "std/io.lwanga";

fn main() -> u64 {
    write_string("Hello from module!\n");
    return 0;
}
```

---

## Security Tools

### Example: Port Scanner

```lwanga
fn scan_port(ip: ptr, port: u16) -> u64 {
    // Socket creation
    let sock: u64 = 0;
    unsafe {
        sock = syscall(41, 2, 1, 0);  // socket(AF_INET, SOCK_STREAM, 0)
    }
    
    // Connection attempt
    // ... (simplified for tutorial)
    
    return sock;
}

fn main() -> u64 {
    let target: ptr = "127.0.0.1";
    let result: u64 = scan_port(target, 80);
    return result;
}
```

### Example: Memory Dumper

```lwanga
fn dump_memory(addr: ptr, size: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < size {
        let byte_ptr: ptr = addr + i;
        // Read and print byte
        i = i + 1;
    }
    
    return 0;
}
```

### Example: Simple Shellcode

```lwanga
naked fn execve_shellcode() -> u64 {
    asm {
        // execve("/bin/sh", NULL, NULL)
        xor rax, rax
        push rax
        mov rdi, 0x68732f6e69622f  // "/bin/sh"
        push rdi
        mov rdi, rsp
        xor rsi, rsi
        xor rdx, rdx
        mov rax, 59  // sys_execve
        syscall
    }
}
```

---

## Best Practices

### 1. Use Unsafe Sparingly

```lwanga
// Good: Minimal unsafe block
fn write_msg(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, 1, msg, len);
    }
}

// Bad: Unnecessary unsafe
unsafe fn add(a: u64, b: u64) -> u64 {
    return a + b;  // No need for unsafe
}
```

### 2. Check Return Values

```lwanga
fn open_file(path: ptr) -> u64 {
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(2, path, 0, 0);  // open()
    }
    
    if fd == -1 {
        return 0;  // Error
    }
    
    return fd;
}
```

### 3. Use Constants

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}
```

### 4. Document Your Code

```lwanga
// Writes a message to stdout
// Returns: number of bytes written, or -1 on error
fn write_string(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, 1, msg, len);
    }
}
```

---

## Common Patterns

### Error Handling

```lwanga
fn safe_divide(a: u64, b: u64) -> u64 {
    if b == 0 {
        return 0;  // Error: division by zero
    }
    return a / b;
}
```

### Resource Management

```lwanga
fn process_file(path: ptr) -> u64 {
    let fd: u64 = 0;
    
    // Open file
    unsafe {
        fd = syscall(2, path, 0, 0);
    }
    
    if fd == -1 {
        return 1;  // Error
    }
    
    // Process file...
    
    // Close file
    unsafe {
        syscall(3, fd, 0, 0);
    }
    
    return 0;
}
```

### Buffer Management

```lwanga
const BUFFER_SIZE: u64 = 1024;

fn read_data(fd: u64) -> u64 {
    let buffer: [u8; BUFFER_SIZE] = [0; BUFFER_SIZE];
    let bytes_read: u64 = 0;
    
    unsafe {
        bytes_read = syscall(0, fd, &buffer, BUFFER_SIZE);
    }
    
    return bytes_read;
}
```

---

## Debugging Tips

### 1. Use Return Codes

```lwanga
fn main() -> u64 {
    let result: u64 = do_something();
    return result;  // Check with: echo $?
}
```

### 2. Compile with Debug Info

```bash
lwangac -g program.lwanga -o program
gdb ./program
```

### 3. Use Verbose Mode

```bash
lwangac -v program.lwanga -o program
```

### 4. Check Generated Assembly

```bash
lwangac --emit-llvm program.lwanga
```

---

## Next Steps

### Intermediate Topics
- [Language Reference](language-reference.md) - Complete language specification
- [Syscall Reference](syscalls.md) - All system calls
- [API Documentation](api/) - Standard library

### Advanced Topics
- [Compiler Internals](compiler.md) - How the compiler works
- [Optimization Guide](optimization.md) - Performance tuning
- [Security Patterns](security-patterns.md) - Advanced security techniques

### Tools
- [Formatter Guide](formatter.md) - Using lwangafmt
- [Highlighter Guide](highlighter.md) - Using lwangacat
- [IDE Setup](ide-setup.md) - Editor configuration

---

## Practice Exercises

### Exercise 1: Calculator
Write a program that adds two numbers and returns the result.

<details>
<summary>Solution</summary>

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let result: u64 = add(10, 20);
    return result;  // Returns 30
}
```
</details>

### Exercise 2: Fibonacci
Calculate the nth Fibonacci number.

<details>
<summary>Solution</summary>

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
</details>

### Exercise 3: String Length
Calculate the length of a null-terminated string.

<details>
<summary>Solution</summary>

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

fn main() -> u64 {
    let msg: ptr = "Hello";
    return strlen(msg);  // Returns 5
}
```
</details>

---

## Getting Help

- **Documentation**: [docs/](../)
- **Examples**: [examples/](../../examples/)
- **Issues**: [GitHub Issues](https://github.com/cosmah/Project-Lwanga/issues)
- **Email**: cosmahke4@gmail.com

---

**Next**: [Language Reference](language-reference.md) | [Back to README](../README.md)
