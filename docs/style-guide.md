# Lwanga Style Guide

**Coding conventions and best practices for Lwanga**

## Table of Contents

1. [Overview](#overview)
2. [Naming Conventions](#naming-conventions)
3. [Formatting](#formatting)
4. [Code Organization](#code-organization)
5. [Best Practices](#best-practices)
6. [Security Guidelines](#security-guidelines)
7. [Examples](#examples)

---

## Overview

This style guide provides conventions for writing clean, maintainable Lwanga code. Following these guidelines ensures consistency across the codebase and makes collaboration easier.

### Philosophy

- **Clarity over cleverness**: Write code that's easy to understand
- **Explicit over implicit**: Make intentions clear
- **Safety first**: Use `unsafe` sparingly and deliberately
- **Minimal footprint**: Keep binaries small and efficient

---

## Naming Conventions

### Variables

Use `snake_case` for variable names:

```lwanga
// Good
let buffer_size: u64 = 1024;
let file_descriptor: u64 = 3;
let user_input: ptr = "data";

// Bad
let BufferSize: u64 = 1024;
let fileDescriptor: u64 = 3;
let USERINPUT: ptr = "data";
```

### Functions

Use `snake_case` for function names:

```lwanga
// Good
fn calculate_checksum(data: ptr, len: u64) -> u64 {
    return 0;
}

fn open_file(path: ptr) -> u64 {
    return 0;
}

// Bad
fn CalculateChecksum(data: ptr, len: u64) -> u64 {
    return 0;
}

fn OpenFile(path: ptr) -> u64 {
    return 0;
}
```

### Constants

Use `SCREAMING_SNAKE_CASE` for constants:

```lwanga
// Good
const MAX_BUFFER_SIZE: u64 = 4096;
const DEFAULT_TIMEOUT: u32 = 30;
const SYSCALL_WRITE: u64 = 1;

// Bad
const maxBufferSize: u64 = 4096;
const default_timeout: u32 = 30;
```

### Structs

Use `PascalCase` for struct names:

```lwanga
// Good
struct FileDescriptor {
    fd: u64,
    flags: u32,
}

struct NetworkSocket {
    addr: u32,
    port: u16,
}

// Bad
struct file_descriptor {
    fd: u64,
    flags: u32,
}
```

### Type Names

Use `snake_case` for type aliases:

```lwanga
// Good
type file_handle = u64;
type buffer_ptr = ptr;

// Bad
type FileHandle = u64;
type BufferPtr = ptr;
```

---

## Formatting

### Indentation

Use 4 spaces (no tabs):

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    if x > 5 {
        return 1;
    }
    return 0;
}
```

### Line Length

Keep lines under 100 characters:

```lwanga
// Good
let result: u64 = calculate_complex_value(
    first_parameter,
    second_parameter,
    third_parameter
);

// Bad
let result: u64 = calculate_complex_value(first_parameter, second_parameter, third_parameter, fourth_parameter);
```

### Braces

Use K&R style (opening brace on same line):

```lwanga
// Good
fn main() -> u64 {
    if condition {
        do_something();
    } else {
        do_something_else();
    }
    return 0;
}

// Bad
fn main() -> u64
{
    if condition
    {
        do_something();
    }
    else
    {
        do_something_else();
    }
    return 0;
}
```

### Spacing

Space around operators:

```lwanga
// Good
let x: u64 = a + b * c;
let y: u64 = (a + b) / (c - d);

// Bad
let x: u64=a+b*c;
let y: u64=(a+b)/(c-d);
```

Space after commas:

```lwanga
// Good
fn add(a: u64, b: u64, c: u64) -> u64 {
    return a + b + c;
}

// Bad
fn add(a: u64,b: u64,c: u64) -> u64 {
    return a+b+c;
}
```

### Blank Lines

Use blank lines to separate logical sections:

```lwanga
fn process_data(data: ptr, len: u64) -> u64 {
    // Validate input
    if len == 0 {
        return 0;
    }

    // Process data
    let mut i: u64 = 0;
    while i < len {
        i = i + 1;
    }

    // Return result
    return i;
}
```

---

## Code Organization

### File Structure

Organize files logically:

```
project/
├── main.lwanga           # Entry point
├── lib/
│   ├── io.lwanga        # I/O operations
│   ├── net.lwanga       # Network operations
│   └── crypto.lwanga    # Cryptography
└── utils/
    ├── string.lwanga    # String utilities
    └── math.lwanga      # Math utilities
```

### Import Order

Sort imports alphabetically:

```lwanga
import "lib/crypto.lwanga";
import "lib/io.lwanga";
import "lib/net.lwanga";
import "utils/math.lwanga";
import "utils/string.lwanga";
```

### Function Order

Organize functions logically:

1. Public functions first
2. Helper functions after
3. Related functions together

```lwanga
// Public API
fn open_file(path: ptr) -> u64 {
    return open_file_internal(path, 0);
}

fn close_file(fd: u64) -> u64 {
    return close_file_internal(fd);
}

// Internal helpers
fn open_file_internal(path: ptr, flags: u32) -> u64 {
    return 0;
}

fn close_file_internal(fd: u64) -> u64 {
    return 0;
}
```

---

## Best Practices

### Use Descriptive Names

```lwanga
// Good
fn calculate_file_checksum(path: ptr) -> u64 {
    let file_size: u64 = get_file_size(path);
    let buffer: ptr = allocate_buffer(file_size);
    return compute_checksum(buffer, file_size);
}

// Bad
fn calc(p: ptr) -> u64 {
    let s: u64 = get_sz(p);
    let b: ptr = alloc(s);
    return chk(b, s);
}
```

### Minimize Unsafe Blocks

```lwanga
// Good: Minimal unsafe scope
fn write_message(msg: ptr, len: u64) -> u64 {
    // Safe code here
    let result: u64 = 0;
    
    unsafe {
        result = syscall(1, 1, msg, len);
    }
    
    // More safe code
    return result;
}

// Bad: Unnecessary unsafe scope
unsafe fn write_message(msg: ptr, len: u64) -> u64 {
    let x: u64 = 10;  // Doesn't need unsafe
    let y: u64 = 20;  // Doesn't need unsafe
    return syscall(1, 1, msg, len);
}
```

### Check Return Values

```lwanga
// Good
fn open_and_read(path: ptr) -> u64 {
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(2, path, 0, 0);  // open
    }
    
    if fd == -1 {
        return 0;  // Error
    }
    
    // Continue with valid fd
    return fd;
}

// Bad
fn open_and_read(path: ptr) -> u64 {
    let fd: u64 = 0;
    unsafe {
        fd = syscall(2, path, 0, 0);
    }
    // No error checking!
    return fd;
}
```

### Use Constants

```lwanga
// Good
const STDOUT: u64 = 1;
const STDERR: u64 = 2;
const SYS_WRITE: u64 = 1;

fn print_error(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDERR, msg, len);
    }
}

// Bad
fn print_error(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, 2, msg, len);  // Magic numbers
    }
}
```

### Document Complex Logic

```lwanga
// Good
fn calculate_checksum(data: ptr, len: u64) -> u64 {
    // Use XOR-based checksum algorithm
    // Each byte is XORed with the running total
    let mut checksum: u64 = 0;
    let mut i: u64 = 0;
    
    while i < len {
        let byte: u8 = *(data + i);
        checksum = checksum ^ byte;
        i = i + 1;
    }
    
    return checksum;
}

// Bad
fn calculate_checksum(data: ptr, len: u64) -> u64 {
    let mut c: u64 = 0;
    let mut i: u64 = 0;
    while i < len {
        c = c ^ *(data + i);
        i = i + 1;
    }
    return c;
}
```

---

## Security Guidelines

### Validate Input

```lwanga
fn process_buffer(buf: ptr, len: u64) -> u64 {
    // Validate length
    if len == 0 || len > MAX_BUFFER_SIZE {
        return 0;
    }
    
    // Validate pointer
    if buf == 0 {
        return 0;
    }
    
    // Process buffer
    return 1;
}
```

### Use Encrypted Strings

```lwanga
// Good: Sensitive data encrypted
fn authenticate() -> u64 {
    let password: ptr = enc "secret_password";
    let api_key: ptr = enc "api_key_12345";
    return verify_credentials(password, api_key);
}

// Bad: Plaintext sensitive data
fn authenticate() -> u64 {
    let password: ptr = "secret_password";
    let api_key: ptr = "api_key_12345";
    return verify_credentials(password, api_key);
}
```

### Clear Sensitive Data

```lwanga
fn process_password(pwd: ptr, len: u64) -> u64 {
    // Use password
    let result: u64 = hash_password(pwd, len);
    
    // Clear from memory
    let mut i: u64 = 0;
    while i < len {
        *(pwd + i) = 0;
        i = i + 1;
    }
    
    return result;
}
```

### Avoid Buffer Overflows

```lwanga
fn copy_string(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    // Ensure we don't overflow
    while i < max_len - 1 {
        let c: u8 = *(src + i);
        if c == 0 {
            break;
        }
        *(dest + i) = c;
        i = i + 1;
    }
    
    // Null terminate
    *(dest + i) = 0;
    return i;
}
```

---

## Examples

### Example 1: Well-Structured Function

```lwanga
const MAX_PATH_LEN: u64 = 4096;
const SYS_OPEN: u64 = 2;
const O_RDONLY: u32 = 0;

// Opens a file and returns the file descriptor
// Returns -1 on error
fn open_file(path: ptr) -> u64 {
    // Validate path
    let path_len: u64 = string_length(path);
    if path_len == 0 || path_len >= MAX_PATH_LEN {
        return -1;
    }
    
    // Open file
    let fd: u64 = 0;
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    return fd;
}
```

### Example 2: Clean Error Handling

```lwanga
const ERROR_INVALID_INPUT: u64 = 1;
const ERROR_SYSCALL_FAILED: u64 = 2;
const SUCCESS: u64 = 0;

fn write_to_file(fd: u64, data: ptr, len: u64) -> u64 {
    // Validate inputs
    if fd == 0 || data == 0 || len == 0 {
        return ERROR_INVALID_INPUT;
    }
    
    // Perform write
    let bytes_written: u64 = 0;
    unsafe {
        bytes_written = syscall(1, fd, data, len);
    }
    
    // Check result
    if bytes_written != len {
        return ERROR_SYSCALL_FAILED;
    }
    
    return SUCCESS;
}
```

### Example 3: Proper Struct Usage

```lwanga
struct Connection {
    socket_fd: u64,
    remote_addr: u32,
    remote_port: u16,
    state: u8,
}

const STATE_CLOSED: u8 = 0;
const STATE_OPEN: u8 = 1;

fn create_connection(addr: u32, port: u16) -> Connection {
    let conn: Connection = Connection {
        socket_fd: 0,
        remote_addr: addr,
        remote_port: port,
        state: STATE_CLOSED,
    };
    return conn;
}

fn open_connection(conn: ptr) -> u64 {
    // Implementation
    return 0;
}
```

---

## Automated Formatting

Use `lwangafmt` to automatically format your code:

```bash
# Format single file
lwangafmt --write myfile.lwanga

# Format entire project
lwangafmt --write --recursive src/

# Check formatting (CI/CD)
lwangafmt --check src/**/*.lwanga
```

Configuration in `.lwangafmt.toml`:

```toml
[formatting]
indent_width = 4
max_line_width = 100
brace_style = "kr"
space_around_operators = true
```

---

## See Also

- [Tutorial](tutorial.md) - Learn Lwanga
- [Formatter Guide](formatter.md) - Using lwangafmt
- [Contributing](../CONTRIBUTING.md) - Contribution guidelines
- [Language Reference](language-reference.md) - Complete specification

---

**Back to**: [Documentation Index](README.md) | [Main README](../README.md)
