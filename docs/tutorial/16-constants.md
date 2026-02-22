# Lesson 16: Constants

**In this lesson, you'll learn how to use constants to make your code more readable, maintainable, and efficient.**

---

## What are Constants?

Constants are named values that are determined at compile time and never change during program execution.

**Why use constants?**
- **Readability** - `MAX_BUFFER_SIZE` is clearer than `1024`
- **Maintainability** - Change value in one place, updates everywhere
- **Performance** - Compiler can optimize better (inlining, constant folding)
- **Type safety** - Constants have types, preventing mistakes
- **Self-documenting** - Names explain what values mean

---

## Declaring Constants

### Basic Syntax

```lwanga
const CONSTANT_NAME: type = value;
```

**Parts explained:**
- `const` - Keyword that declares a constant
- `CONSTANT_NAME` - Name in SCREAMING_SNAKE_CASE (convention)
- `: type` - The type of the constant
- `= value` - The compile-time value

### Simple Constants

```lwanga
const MAX_SIZE: u64 = 1024;
const PORT: u16 = 8080;
const BUFFER_LEN: u32 = 512;
const TIMEOUT: u8 = 30;

fn main() -> u64 {
    return MAX_SIZE;  // Returns 1024
}
```

**Naming convention:** Use SCREAMING_SNAKE_CASE for constants. This makes them easy to spot in code.

---

## Constants vs Variables

### Constants (Compile-Time)

```lwanga
const MAX: u64 = 100;

fn main() -> u64 {
    let x: u64 = MAX;  // MAX is replaced with 100 at compile time
    return x;
}
```

**Properties:**
- Value must be known at compile time
- Cannot be changed
- No memory allocated at runtime
- Inlined wherever used

### Variables (Runtime)

```lwanga
fn main() -> u64 {
    let x: u64 = 10;      // Allocated at runtime
    let y: u64 = x * 2;   // Calculated at runtime
    return y;
}
```

**Properties:**
- Value can be computed at runtime
- Can be changed (if `mut`)
- Memory allocated on stack
- Actual variable in memory

### Comparison

```lwanga
const COMPILE_TIME: u64 = 100;  // Known at compile time

fn main() -> u64 {
    let runtime: u64 = 50;  // Known at runtime
    
    // This is OK - constant used in expression
    let result: u64 = COMPILE_TIME + runtime;
    
    return result;  // Returns 150
}
```

**Design choice:** Constants must be compile-time values. This allows aggressive optimization and ensures they're truly constant.

---

## Using Constants

### File Descriptors

```lwanga
const STDIN: u64 = 0;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;

fn write_to_stdout(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, STDOUT, msg, len);
    }
}

fn write_to_stderr(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, STDERR, msg, len);
    }
}

fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    write_to_stdout(msg, 7);
    return 0;
}
```

**Why use constants here?**

Instead of magic numbers (0, 1, 2), we have meaningful names. Anyone reading the code knows what `STDOUT` means.

### Syscall Numbers

```lwanga
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_OPEN: u64 = 2;
const SYS_CLOSE: u64 = 3;
const SYS_EXIT: u64 = 60;

fn write_message(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}

fn exit_program(code: u64) -> u64 {
    unsafe {
        return syscall(SYS_EXIT, code, 0, 0);
    }
}

fn main() -> u64 {
    let msg: ptr = "Goodbye!\n";
    write_message(msg, 9);
    exit_program(0);
    return 0;
}
```

**Benefit:** If syscall numbers change (different OS), update constants in one place.

### Configuration Values

```lwanga
const MAX_CONNECTIONS: u32 = 100;
const TIMEOUT_SECONDS: u64 = 30;
const BUFFER_SIZE: u64 = 4096;
const MAX_RETRIES: u8 = 3;

fn allocate_buffer() -> u64 {
    // Use constant for buffer size
    return BUFFER_SIZE;
}

fn check_connections(count: u32) -> u64 {
    if count > MAX_CONNECTIONS {
        return 0;  // Too many connections
    }
    return 1;  // OK
}

fn main() -> u64 {
    let connections: u32 = 50;
    let ok: u64 = check_connections(connections);
    return ok;
}
```

### Network Protocols

```lwanga
const TCP_PROTOCOL: u8 = 6;
const UDP_PROTOCOL: u8 = 17;
const ICMP_PROTOCOL: u8 = 1;

const HTTP_PORT: u16 = 80;
const HTTPS_PORT: u16 = 443;
const SSH_PORT: u16 = 22;
const DNS_PORT: u16 = 53;

fn create_tcp_socket() -> u64 {
    // Use protocol constant
    let protocol: u8 = TCP_PROTOCOL;
    return protocol as u64;
}

fn connect_to_web_server(use_ssl: u64) -> u16 {
    if use_ssl == 1 {
        return HTTPS_PORT;
    }
    return HTTP_PORT;
}

fn main() -> u64 {
    let port: u16 = connect_to_web_server(1);
    return port as u64;  // Returns 443
}
```

---

## Constant Expressions

### Arithmetic in Constants

```lwanga
const KILOBYTE: u64 = 1024;
const MEGABYTE: u64 = KILOBYTE * 1024;
const GIGABYTE: u64 = MEGABYTE * 1024;

fn main() -> u64 {
    return MEGABYTE;  // Returns 1048576
}
```

**Note:** Constant expressions are evaluated at compile time!

### Bit Operations in Constants

```lwanga
const FLAG_READ: u32 = 0x01;
const FLAG_WRITE: u32 = 0x02;
const FLAG_EXECUTE: u32 = 0x04;
const FLAG_ALL: u32 = FLAG_READ | FLAG_WRITE | FLAG_EXECUTE;

fn main() -> u64 {
    return FLAG_ALL as u64;  // Returns 7 (0x01 | 0x02 | 0x04)
}
```

### Type Conversions in Constants

```lwanga
const BYTE_VALUE: u8 = 255;
const WORD_VALUE: u16 = BYTE_VALUE as u16;
const DWORD_VALUE: u32 = WORD_VALUE as u32;

fn main() -> u64 {
    return DWORD_VALUE as u64;  // Returns 255
}
```

---

## Practical Examples

### Example 1: Error Codes

```lwanga
const ERROR_SUCCESS: u64 = 0;
const ERROR_INVALID_PARAM: u64 = 1;
const ERROR_OUT_OF_MEMORY: u64 = 2;
const ERROR_FILE_NOT_FOUND: u64 = 3;
const ERROR_PERMISSION_DENIED: u64 = 4;

fn open_file(path: ptr) -> u64 {
    // Simulate file opening
    if path as u64 == 0 {
        return ERROR_INVALID_PARAM;
    }
    
    // Success
    return ERROR_SUCCESS;
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    let result: u64 = open_file(path);
    
    if result == ERROR_SUCCESS {
        return 0;
    }
    
    return result;
}
```

### Example 2: Memory Sizes

```lwanga
const PAGE_SIZE: u64 = 4096;
const CACHE_LINE_SIZE: u64 = 64;
const STACK_SIZE: u64 = PAGE_SIZE * 16;  // 64KB

fn align_to_page(size: u64) -> u64 {
    // Round up to next page boundary
    let remainder: u64 = size % PAGE_SIZE;
    
    if remainder == 0 {
        return size;
    }
    
    return size + (PAGE_SIZE - remainder);
}

fn main() -> u64 {
    let size: u64 = 5000;
    let aligned: u64 = align_to_page(size);
    
    return aligned;  // Returns 8192 (2 pages)
}
```

### Example 3: ASCII Values

```lwanga
const ASCII_ZERO: u8 = 48;   // '0'
const ASCII_NINE: u8 = 57;   // '9'
const ASCII_A: u8 = 65;      // 'A'
const ASCII_Z: u8 = 90;      // 'Z'
const ASCII_a: u8 = 97;      // 'a'
const ASCII_z: u8 = 122;     // 'z'
const ASCII_SPACE: u8 = 32;  // ' '
const ASCII_NEWLINE: u8 = 10; // '\n'

fn is_digit(c: u8) -> u64 {
    if c >= ASCII_ZERO && c <= ASCII_NINE {
        return 1;
    }
    return 0;
}

fn is_uppercase(c: u8) -> u64 {
    if c >= ASCII_A && c <= ASCII_Z {
        return 1;
    }
    return 0;
}

fn to_lowercase(c: u8) -> u8 {
    if is_uppercase(c) == 1 {
        return c + 32;  // 'A' + 32 = 'a'
    }
    return c;
}

fn main() -> u64 {
    let ch: u8 = ASCII_A;
    let lower: u8 = to_lowercase(ch);
    
    return lower as u64;  // Returns 97 ('a')
}
```

### Example 4: Bit Masks

```lwanga
const MASK_LOWER_BYTE: u64 = 0xFF;
const MASK_LOWER_WORD: u64 = 0xFFFF;
const MASK_LOWER_DWORD: u64 = 0xFFFFFFFF;

fn extract_byte(value: u64, byte_index: u64) -> u8 {
    let shift: u64 = byte_index * 8;
    let shifted: u64 = value >> shift;
    let masked: u64 = shifted & MASK_LOWER_BYTE;
    
    return masked as u8;
}

fn main() -> u64 {
    let value: u64 = 0x12345678;
    let byte0: u8 = extract_byte(value, 0);  // 0x78
    let byte1: u8 = extract_byte(value, 1);  // 0x56
    
    return byte0 as u64;
}
```

---

## Scope and Visibility

### Global Constants

```lwanga
const GLOBAL_CONST: u64 = 100;

fn function1() -> u64 {
    return GLOBAL_CONST;  // Accessible
}

fn function2() -> u64 {
    return GLOBAL_CONST * 2;  // Accessible
}

fn main() -> u64 {
    return GLOBAL_CONST;  // Accessible
}
```

**Constants are globally visible** - they can be used anywhere in your program.

---

## Common Mistakes

### Mistake 1: Trying to Change a Constant

```lwanga
const MAX: u64 = 100;

fn main() -> u64 {
    MAX = 200;  // ERROR: cannot assign to constant
    return MAX;
}
```

**Fix:** Use a variable instead:
```lwanga
fn main() -> u64 {
    let mut max: u64 = 100;
    max = 200;  // OK
    return max;
}
```

### Mistake 2: Using Runtime Value

```lwanga
fn get_value() -> u64 {
    return 42;
}

const VALUE: u64 = get_value();  // ERROR: not a compile-time value
```

**Fix:** Use a variable:
```lwanga
fn get_value() -> u64 {
    return 42;
}

fn main() -> u64 {
    let value: u64 = get_value();  // OK
    return value;
}
```

### Mistake 3: Wrong Naming Convention

```lwanga
const maxSize: u64 = 1024;  // Wrong: should be SCREAMING_SNAKE_CASE
```

**Fix:**
```lwanga
const MAX_SIZE: u64 = 1024;  // Correct
```

---

## Exercises

### Exercise 1: Define Color Constants

Define constants for RGB color values (red, green, blue, white, black).

<details>
<summary>Solution</summary>

```lwanga
const COLOR_RED: u32 = 0xFF0000;
const COLOR_GREEN: u32 = 0x00FF00;
const COLOR_BLUE: u32 = 0x0000FF;
const COLOR_WHITE: u32 = 0xFFFFFF;
const COLOR_BLACK: u32 = 0x000000;

fn main() -> u64 {
    return COLOR_RED as u64;
}
```
</details>

### Exercise 2: Time Constants

Define constants for time conversions (seconds in minute, hour, day).

<details>
<summary>Solution</summary>

```lwanga
const SECONDS_PER_MINUTE: u64 = 60;
const SECONDS_PER_HOUR: u64 = SECONDS_PER_MINUTE * 60;
const SECONDS_PER_DAY: u64 = SECONDS_PER_HOUR * 24;

fn hours_to_seconds(hours: u64) -> u64 {
    return hours * SECONDS_PER_HOUR;
}

fn main() -> u64 {
    let seconds: u64 = hours_to_seconds(2);
    return seconds;  // Returns 7200
}
```
</details>

### Exercise 3: HTTP Status Codes

Define constants for common HTTP status codes.

<details>
<summary>Solution</summary>

```lwanga
const HTTP_OK: u16 = 200;
const HTTP_CREATED: u16 = 201;
const HTTP_BAD_REQUEST: u16 = 400;
const HTTP_UNAUTHORIZED: u16 = 401;
const HTTP_FORBIDDEN: u16 = 403;
const HTTP_NOT_FOUND: u16 = 404;
const HTTP_INTERNAL_ERROR: u16 = 500;

fn is_success(status: u16) -> u64 {
    if status >= 200 && status < 300 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    return is_success(HTTP_OK);  // Returns 1
}
```
</details>

### Exercise 4: File Permissions

Define constants for Unix file permissions.

<details>
<summary>Solution</summary>

```lwanga
const PERM_READ: u16 = 0o4;
const PERM_WRITE: u16 = 0o2;
const PERM_EXECUTE: u16 = 0o1;

const PERM_USER_READ: u16 = PERM_READ << 6;
const PERM_USER_WRITE: u16 = PERM_WRITE << 6;
const PERM_USER_EXECUTE: u16 = PERM_EXECUTE << 6;

const PERM_GROUP_READ: u16 = PERM_READ << 3;
const PERM_GROUP_WRITE: u16 = PERM_WRITE << 3;
const PERM_GROUP_EXECUTE: u16 = PERM_EXECUTE << 3;

const PERM_OTHER_READ: u16 = PERM_READ;
const PERM_OTHER_WRITE: u16 = PERM_WRITE;
const PERM_OTHER_EXECUTE: u16 = PERM_EXECUTE;

fn main() -> u64 {
    let perms: u16 = PERM_USER_READ | PERM_USER_WRITE | PERM_GROUP_READ;
    return perms as u64;  // Returns 0o644
}
```
</details>

### Exercise 5: Buffer Sizes

Define constants for various buffer sizes used in networking.

<details>
<summary>Solution</summary>

```lwanga
const SMALL_BUFFER: u64 = 256;
const MEDIUM_BUFFER: u64 = 1024;
const LARGE_BUFFER: u64 = 4096;
const HUGE_BUFFER: u64 = 65536;

const MTU_ETHERNET: u64 = 1500;
const MTU_LOOPBACK: u64 = 65536;

fn choose_buffer_size(data_size: u64) -> u64 {
    if data_size <= SMALL_BUFFER {
        return SMALL_BUFFER;
    }
    
    if data_size <= MEDIUM_BUFFER {
        return MEDIUM_BUFFER;
    }
    
    if data_size <= LARGE_BUFFER {
        return LARGE_BUFFER;
    }
    
    return HUGE_BUFFER;
}

fn main() -> u64 {
    let size: u64 = choose_buffer_size(2000);
    return size;  // Returns 4096
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What constants are and why they're useful
- ✅ How to declare constants with `const` keyword
- ✅ Naming convention: SCREAMING_SNAKE_CASE
- ✅ Constants vs variables (compile-time vs runtime)
- ✅ Common use cases: file descriptors, syscalls, configuration, protocols
- ✅ Constant expressions and compile-time evaluation
- ✅ Practical examples: error codes, memory sizes, ASCII values, bit masks
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand constants, let's dive into Lwanga's memory model in [Lesson 17: Memory Model](17-memory-model.md).

---

**Navigation**: [← Previous: Type System](15-type-system.md) | [Tutorial Home](README.md) | [Next: Memory Model →](17-memory-model.md)
