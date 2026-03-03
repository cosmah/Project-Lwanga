# Lesson 1: Introduction to Lwanga

**Welcome to Lwanga! In this lesson, you'll learn what Lwanga is, why it exists, and what makes it unique.**

---

## What is Lwanga?

Lwanga is a low-level, compiled systems programming language designed specifically for:
- **Security research and penetration testing**
- **Shellcode development**
- **Exploit creation**
- **Systems programming with minimal footprint**

Unlike general-purpose languages, Lwanga is built from the ground up for security professionals, pentesters, and systems programmers who need direct hardware access and minimal binary sizes.

---

## Why Was Lwanga Created?

### The Problem

Security researchers and systems programmers often face these challenges:

1. **C is too unsafe**: Easy to make mistakes, no modern safety features
2. **Rust is too complex**: Steep learning curve, large binaries, complex syntax
3. **Python is too slow**: Interpreted, can't create standalone binaries easily
4. **Assembly is too low-level**: Hard to maintain, platform-specific

### The Solution: Lwanga

Lwanga bridges the gap by providing:

- **Low-level control** like C and Assembly
- **Modern safety features** (explicit unsafe blocks)
- **Minimal binaries** (Hello World under 10KB)
- **Simple syntax** (easier than Rust, safer than C)
- **Direct syscalls** (no libc dependency)
- **Built-in security features** (encrypted strings, obfuscation)

---

## Language Philosophy

### 1. Explicit Over Implicit

In Lwanga, everything is explicit. You always know what your code is doing:

```lwanga
// You must explicitly mark unsafe operations
unsafe {
    syscall(1, 1, msg, len);  // Clear that this is a system call
}
```

**Why?** Security code requires precision. Implicit behavior can hide bugs or vulnerabilities.

### 2. Minimal Runtime

Lwanga programs have zero runtime overhead:

- No garbage collector
- No standard library by default
- No hidden allocations
- Direct syscalls without libc

**Why?** Smaller binaries are harder to detect, faster to execute, and easier to analyze.

### 3. Safety Where It Matters

Lwanga provides safety features but doesn't force them everywhere:

```lwanga
// Safe by default
let x: u64 = 42;
let y: u64 = x + 10;

// Unsafe when needed
unsafe {
    let result: u64 = syscall(60, 0, 0, 0);
}
```

**Why?** Systems programming requires unsafe operations, but they should be clearly marked.

### 4. Security-First Design

Built-in features for security work:

- Compile-time string encryption
- Position-independent code generation
- Code obfuscation
- Naked functions for shellcode

**Why?** Security tools have unique requirements that general-purpose languages don't address.

---

## Comparison with Other Languages

### Lwanga vs C

| Feature | C | Lwanga |
|---------|---|--------|
| Memory safety | No | Explicit unsafe blocks |
| Binary size | Small | Smaller (no libc) |
| Learning curve | Moderate | Easy |
| Type safety | Weak | Strong |
| Modern features | No | Yes (encrypted strings, etc.) |

### Lwanga vs Rust

| Feature | Rust | Lwanga |
|---------|------|--------|
| Memory safety | Borrow checker | Explicit unsafe |
| Binary size | Large | Minimal |
| Learning curve | Steep | Gentle |
| Syntax complexity | High | Low |
| Security features | General | Specialized |

### Lwanga vs Python

| Feature | Python | Lwanga |
|---------|--------|--------|
| Execution | Interpreted | Compiled |
| Binary size | N/A (needs runtime) | Minimal |
| Performance | Slow | Fast (native code) |
| Low-level access | Limited | Full |
| Deployment | Needs Python installed | Standalone binary |

---

## What Can You Build with Lwanga?

### 1. Security Tools

```lwanga
// Port scanner
fn scan_port(ip: ptr, port: u16) -> u64 {
    // Create socket, attempt connection
    // Return 1 if open, 0 if closed
}
```

### 2. Shellcode

```lwanga
// Position-independent shellcode
naked fn shellcode() -> u64 {
    asm {
        mov rax, 59      // execve
        lea rdi, [rip + binsh]
        xor rsi, rsi
        xor rdx, rdx
        syscall
    binsh:
        .ascii "/bin/sh\0"
    }
}
```

### 3. System Utilities

```lwanga
// File encryption tool
fn encrypt_file(input: ptr, output: ptr, key: ptr) -> u64 {
    // Read file, encrypt, write output
}
```

### 4. Network Tools

```lwanga
// Simple TCP server
fn tcp_server(port: u16) -> u64 {
    // Create socket, bind, listen, accept connections
}
```

---

## Key Features

### 1. Direct System Calls

No libc dependency - talk directly to the kernel:

```lwanga
unsafe {
    syscall(1, 1, msg, len);  // write(stdout, msg, len)
}
```

### 2. Inline Assembly

Mix high-level and low-level code seamlessly:

```lwanga
fn get_pid() -> u64 {
    let pid: u64 = 0;
    unsafe {
        asm {
            mov rax, 39      // sys_getpid
            syscall
            mov pid, rax
        }
    }
    return pid;
}
```

### 3. Encrypted Strings

Strings are encrypted at compile-time:

```lwanga
let password: ptr = enc "secret123";
// "secret123" never appears in plaintext in the binary
```

### 4. Type Safety

Strong type system prevents common bugs:

```lwanga
let x: u64 = 42;
let y: u32 = x;  // ERROR: type mismatch
```

### 5. Zero-Cost Abstractions

High-level features compile to efficient machine code:

```lwanga
struct Point {
    x: u64,
    y: u64,
}

let p: Point = Point { x: 10, y: 20 };
// Compiles to direct memory access, no overhead
```

---

## Who Should Use Lwanga?

### Perfect For:

- **Penetration testers** creating custom tools
- **Security researchers** developing exploits
- **Malware analysts** understanding low-level code
- **Systems programmers** needing minimal binaries
- **CTF players** solving binary exploitation challenges
- **Embedded developers** working with constrained resources

### Not Ideal For:

- Web development (use Go, Rust, or Node.js)
- GUI applications (use C++, Rust, or Python)
- Data science (use Python or R)
- General application development (use Rust, Go, or C++)

---

## Design Decisions Explained

### Why No Standard Library?

**Decision**: Lwanga has no default standard library.

**Reason**: 
- Keeps binaries minimal
- Forces explicit dependencies
- Prevents bloat
- Gives you full control

**Impact**: You write more code, but you know exactly what's in your binary.

### Why Explicit Unsafe Blocks?

**Decision**: Unsafe operations must be in `unsafe {}` blocks.

**Reason**:
- Makes dangerous code visible
- Easier to audit security-critical code
- Helps prevent accidents
- Clear separation of safe/unsafe

**Impact**: Slightly more verbose, but much safer.

### Why Direct Syscalls?

**Decision**: Use syscalls instead of libc functions.

**Reason**:
- Smaller binaries (no libc linking)
- More control over behavior
- Harder to detect (no libc imports)
- Better for shellcode

**Impact**: More low-level, but more powerful.

### Why Strong Types?

**Decision**: Strict type checking at compile time.

**Reason**:
- Catches bugs early
- Prevents common vulnerabilities
- Makes code more maintainable
- Self-documenting

**Impact**: Slightly more typing, but fewer bugs.

---

## Setting Up Your Environment

### 1. Install Lwanga

Follow the [Installation Guide](../../INSTALL.md) for your platform:

- **Linux**: `.deb` or `.rpm` package
- **macOS**: Homebrew or build from source
- **Windows**: Installer or Chocolatey

### 2. Verify Installation

```bash
lwangac --version
```

You should see:
```
Lwanga Compiler v1.0.3
```

### 3. Choose an Editor

Lwanga has extensions for:
- **VS Code**: Full IDE support with syntax highlighting
- **Vim/Neovim**: Syntax highlighting and commands
- **Any text editor**: Works with any editor

### 4. Create a Workspace

```bash
mkdir lwanga-tutorial
cd lwanga-tutorial
```

---

## Your First Look at Lwanga Code

Here's a complete Lwanga program (don't worry if you don't understand it yet):

```lwanga
// This is a comment

// Every program needs a main function
fn main() -> u64 {
    // Declare a variable
    let message: ptr = "Hello, Lwanga!\n";
    let length: u64 = 15;
    
    // Make a system call to write to stdout
    unsafe {
        syscall(1, 1, message, length);
    }
    
    // Return 0 to indicate success
    return 0;
}
```

**What's happening here?**

1. `fn main() -> u64` - Define the main function that returns a 64-bit number
2. `let message: ptr` - Create a variable that points to a string
3. `unsafe { ... }` - Mark the syscall as an unsafe operation
4. `syscall(1, 1, message, length)` - Call the write syscall
5. `return 0` - Exit with success code

Don't worry if this seems complex - we'll break it all down in the next lessons!

---

## What You'll Learn Next

In the next lesson, you'll:
- Write your first Hello World program
- Understand every line of code
- Compile and run it
- Learn about common mistakes

---

## Summary

In this lesson, you learned:

- ✅ What Lwanga is and why it exists
- ✅ The language philosophy and design goals
- ✅ How Lwanga compares to other languages
- ✅ What you can build with Lwanga
- ✅ Key features and design decisions
- ✅ How to set up your environment

---

## Next Lesson

Ready to write code? Continue to [Lesson 2: Your First Program](02-hello-world.md)

---

**Navigation**: [Tutorial Home](README.md) | [Next: Hello World →](02-hello-world.md)
