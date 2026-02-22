# Lesson 33: Preprocessor

**In this lesson, you'll learn how to use the preprocessor for conditional compilation, allowing you to write code that adapts to different platforms, configurations, and build modes.**

---

## What is the Preprocessor?

The preprocessor is a tool that processes your code before compilation. It allows you to:

- **Conditional compilation** - Include/exclude code based on conditions
- **Platform-specific code** - Write code that works on different systems
- **Feature flags** - Enable/disable features at compile time
- **Debug vs Release** - Different behavior for development and production
- **Configuration** - Customize builds without changing code

**Design choice:** Lwanga's preprocessor uses simple `#if` directives similar to C, making it familiar and easy to learn.

---

## Conditional Compilation

### Basic #if Directive

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

**How it works:**
- If `DEBUG` is defined, `LOG_LEVEL` is 3
- Otherwise, `LOG_LEVEL` is 1
- Only one version exists in the compiled binary

**Compile with DEBUG:**
```bash
lwangac -D DEBUG program.lwanga -o program
```

**Compile without DEBUG:**
```bash
lwangac program.lwanga -o program
```

### #elif for Multiple Conditions

```lwanga
#if OPTIMIZATION_LEVEL == 3
const BUFFER_SIZE: u64 = 8192;
#elif OPTIMIZATION_LEVEL == 2
const BUFFER_SIZE: u64 = 4096;
#elif OPTIMIZATION_LEVEL == 1
const BUFFER_SIZE: u64 = 2048;
#else
const BUFFER_SIZE: u64 = 1024;
#endif

fn main() -> u64 {
    return BUFFER_SIZE;
}
```

**Compile with specific optimization:**
```bash
lwangac -D OPTIMIZATION_LEVEL=3 program.lwanga -o program
```

### Nested Conditionals

```lwanga
#if PLATFORM_LINUX
    #if DEBUG
    const LOG_FILE: ptr = "/tmp/debug.log";
    #else
    const LOG_FILE: ptr = "/var/log/app.log";
    #endif
#elif PLATFORM_WINDOWS
    #if DEBUG
    const LOG_FILE: ptr = "C:\\temp\\debug.log";
    #else
    const LOG_FILE: ptr = "C:\\logs\\app.log";
    #endif
#endif

fn main() -> u64 {
    return 0;
}
```

---

## Platform-Specific Code

### Detecting Platforms

```lwanga
#if LINUX
const NEWLINE: ptr = "\n";
const PATH_SEP: ptr = "/";
const NULL_DEVICE: ptr = "/dev/null";
#elif WINDOWS
const NEWLINE: ptr = "\r\n";
const PATH_SEP: ptr = "\\";
const NULL_DEVICE: ptr = "NUL";
#elif MACOS
const NEWLINE: ptr = "\n";
const PATH_SEP: ptr = "/";
const NULL_DEVICE: ptr = "/dev/null";
#else
const NEWLINE: ptr = "\n";
const PATH_SEP: ptr = "/";
const NULL_DEVICE: ptr = "/dev/null";
#endif

fn main() -> u64 {
    return 0;
}
```

### Platform-Specific Functions

```lwanga
#if LINUX
fn get_pid() -> u64 {
    const SYS_GETPID: u64 = 39;
    let pid: u64 = 0;
    unsafe {
        pid = syscall(SYS_GETPID, 0, 0, 0);
    }
    return pid;
}
#elif WINDOWS
fn get_pid() -> u64 {
    // Windows-specific implementation
    return 0;  // Placeholder
}
#else
fn get_pid() -> u64 {
    return 0;  // Fallback
}
#endif

fn main() -> u64 {
    let pid: u64 = get_pid();
    return pid;
}
```

### Architecture-Specific Code

```lwanga
#if ARCH_X86_64
const POINTER_SIZE: u64 = 8;
const MAX_ADDRESS: u64 = 0x7FFFFFFFFFFF;
#elif ARCH_X86
const POINTER_SIZE: u64 = 4;
const MAX_ADDRESS: u64 = 0xFFFFFFFF;
#elif ARCH_ARM64
const POINTER_SIZE: u64 = 8;
const MAX_ADDRESS: u64 = 0xFFFFFFFFFFFF;
#else
const POINTER_SIZE: u64 = 8;
const MAX_ADDRESS: u64 = 0x7FFFFFFFFFFF;
#endif

fn main() -> u64 {
    return POINTER_SIZE;
}
```

---

## Feature Flags

### Enabling Optional Features

```lwanga
#if FEATURE_NETWORKING
const SYS_SOCKET: u64 = 41;
const SYS_CONNECT: u64 = 42;

fn network_init() -> u64 {
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, 2, 1, 0);
    }
    return sockfd;
}
#endif

fn main() -> u64 {
    #if FEATURE_NETWORKING
    let socket: u64 = network_init();
    return socket;
    #else
    return 0;
    #endif
}
```

**Compile with networking:**
```bash
lwangac -D FEATURE_NETWORKING program.lwanga -o program
```

**Compile without networking:**
```bash
lwangac program.lwanga -o program
```

### Multiple Feature Flags

```lwanga
#if FEATURE_LOGGING
fn log_message(msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    unsafe {
        syscall(SYS_WRITE, 1, msg, len);
    }
    return 0;
}
#endif

#if FEATURE_ENCRYPTION
fn encrypt_data(data: ptr, len: u64) -> u64 {
    // Encryption logic
    return 0;
}
#endif

fn process_data(data: ptr, len: u64) -> u64 {
    #if FEATURE_LOGGING
    log_message("Processing data\n", 16);
    #endif
    
    #if FEATURE_ENCRYPTION
    encrypt_data(data, len);
    #endif
    
    return 0;
}

fn main() -> u64 {
    let data: ptr = "Hello";
    process_data(data, 5);
    return 0;
}
```

**Compile with both features:**
```bash
lwangac -D FEATURE_LOGGING -D FEATURE_ENCRYPTION program.lwanga -o program
```

### Combining Feature Flags

```lwanga
#if FEATURE_NETWORKING && FEATURE_ENCRYPTION
fn secure_send(socket: u64, data: ptr, len: u64) -> u64 {
    // Encrypt then send
    let encrypted: ptr = encrypt_data(data, len);
    return network_send(socket, encrypted, len);
}
#endif

#if FEATURE_LOGGING || FEATURE_DEBUG
fn debug_print(msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    unsafe {
        syscall(SYS_WRITE, 1, msg, len);
    }
    return 0;
}
#endif

fn main() -> u64 {
    return 0;
}
```

---

## Debug vs Release Builds

### Debug-Only Code

```lwanga
#if DEBUG
fn assert(condition: u64, msg: ptr) -> u64 {
    if condition == 0 {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, msg, 50);  // Write to stderr
            syscall(60, 1, 0, 0);  // Exit with error
        }
    }
    return 0;
}
#else
fn assert(condition: u64, msg: ptr) -> u64 {
    // No-op in release builds
    return 0;
}
#endif

fn divide(a: u64, b: u64) -> u64 {
    assert(b != 0, "Division by zero!\n");
    return a / b;
}

fn main() -> u64 {
    let result: u64 = divide(10, 2);
    return result;
}
```

### Performance Counters

```lwanga
#if DEBUG
let mut operation_count: u64 = 0;

fn count_operation() -> u64 {
    operation_count = operation_count + 1;
    return 0;
}

fn print_stats() -> u64 {
    const SYS_WRITE: u64 = 1;
    let msg: ptr = "Operations: ";
    unsafe {
        syscall(SYS_WRITE, 1, msg, 12);
    }
    return 0;
}
#else
fn count_operation() -> u64 {
    return 0;  // No-op in release
}

fn print_stats() -> u64 {
    return 0;  // No-op in release
}
#endif

fn process() -> u64 {
    count_operation();
    // Do work
    return 0;
}

fn main() -> u64 {
    process();
    process();
    process();
    print_stats();
    return 0;
}
```

---

## Compile-Time Configuration

### Defining Macros

```bash
# Single definition
lwangac -D DEBUG program.lwanga -o program

# Multiple definitions
lwangac -D DEBUG -D FEATURE_NETWORKING program.lwanga -o program

# Definition with value
lwangac -D MAX_CONNECTIONS=100 program.lwanga -o program

# Multiple values
lwangac -D DEBUG -D LOG_LEVEL=3 -D BUFFER_SIZE=8192 program.lwanga -o program
```

### Using Defined Values

```lwanga
#if MAX_CONNECTIONS
const CONNECTION_LIMIT: u64 = MAX_CONNECTIONS;
#else
const CONNECTION_LIMIT: u64 = 10;  // Default
#endif

#if BUFFER_SIZE
const BUF_SIZE: u64 = BUFFER_SIZE;
#else
const BUF_SIZE: u64 = 1024;  // Default
#endif

fn main() -> u64 {
    return CONNECTION_LIMIT;
}
```

### Configuration Profiles

```bash
# Development profile
lwangac -D DEBUG -D LOG_LEVEL=3 -D FEATURE_ALL program.lwanga -o program-dev

# Production profile
lwangac -D RELEASE -D LOG_LEVEL=1 -D OPTIMIZE program.lwanga -o program-prod

# Testing profile
lwangac -D TEST -D LOG_LEVEL=2 -D MOCK_NETWORK program.lwanga -o program-test
```

---

## Practical Examples

### Example 1: Cross-Platform File Operations

```lwanga
#if LINUX
const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;
const O_RDONLY: u32 = 0;
const O_WRONLY: u32 = 1;
const O_CREAT: u32 = 64;
#elif WINDOWS
// Windows syscall numbers would be different
const SYS_OPEN: u64 = 100;  // Placeholder
const SYS_READ: u64 = 101;
const SYS_WRITE: u64 = 102;
const SYS_CLOSE: u64 = 103;
const O_RDONLY: u32 = 0;
const O_WRONLY: u32 = 1;
const O_CREAT: u32 = 64;
#endif

fn open_file(path: ptr, flags: u32) -> u64 {
    let fd: u64 = 0;
    unsafe {
        fd = syscall(SYS_OPEN, path, flags, 0644);
    }
    return fd;
}

fn main() -> u64 {
    #if LINUX
    let file: ptr = "/tmp/test.txt";
    #elif WINDOWS
    let file: ptr = "C:\\temp\\test.txt";
    #endif
    
    let fd: u64 = open_file(file, O_RDONLY);
    return fd;
}
```

### Example 2: Feature-Complete Application

```lwanga
#if FEATURE_LOGGING
fn log(level: u64, msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    
    #if LOG_LEVEL >= level
    unsafe {
        syscall(SYS_WRITE, 1, msg, len);
    }
    #endif
    
    return 0;
}
#else
fn log(level: u64, msg: ptr, len: u64) -> u64 {
    return 0;  // No-op
}
#endif

fn process_request(data: ptr, len: u64) -> u64 {
    log(1, "Processing request\n", 18);
    
    #if FEATURE_VALIDATION
    if validate_input(data, len) == 0 {
        log(2, "Invalid input\n", 14);
        return 1;
    }
    #endif
    
    #if FEATURE_ENCRYPTION
    let encrypted: ptr = encrypt(data, len);
    data = encrypted;
    #endif
    
    log(1, "Request processed\n", 18);
    return 0;
}

fn main() -> u64 {
    let request: ptr = "Hello";
    process_request(request, 5);
    return 0;
}
```

### Example 3: Build Variants

```lwanga
#if BUILD_VARIANT == 1
// Minimal build
const FEATURES: ptr = "minimal";
const BUFFER_SIZE: u64 = 512;
const MAX_CONNECTIONS: u64 = 10;
#elif BUILD_VARIANT == 2
// Standard build
const FEATURES: ptr = "standard";
const BUFFER_SIZE: u64 = 2048;
const MAX_CONNECTIONS: u64 = 100;
#elif BUILD_VARIANT == 3
// Premium build
const FEATURES: ptr = "premium";
const BUFFER_SIZE: u64 = 8192;
const MAX_CONNECTIONS: u64 = 1000;
#else
// Default build
const FEATURES: ptr = "default";
const BUFFER_SIZE: u64 = 1024;
const MAX_CONNECTIONS: u64 = 50;
#endif

fn main() -> u64 {
    return BUFFER_SIZE;
}
```

**Compile different variants:**
```bash
lwangac -D BUILD_VARIANT=1 program.lwanga -o program-minimal
lwangac -D BUILD_VARIANT=2 program.lwanga -o program-standard
lwangac -D BUILD_VARIANT=3 program.lwanga -o program-premium
```

### Example 4: Testing Infrastructure

```lwanga
#if TEST
// Mock functions for testing
fn network_send(socket: u64, data: ptr, len: u64) -> u64 {
    // Mock: just return success
    return len;
}

fn network_receive(socket: u64, buffer: ptr, len: u64) -> u64 {
    // Mock: return fake data
    return 10;
}
#else
// Real implementations
fn network_send(socket: u64, data: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_WRITE, socket, data, len);
    }
    return result;
}

fn network_receive(socket: u64, buffer: ptr, len: u64) -> u64 {
    const SYS_READ: u64 = 0;
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_READ, socket, buffer, len);
    }
    return result;
}
#endif

fn main() -> u64 {
    let socket: u64 = 3;
    let data: ptr = "test";
    let sent: u64 = network_send(socket, data, 4);
    return sent;
}
```

### Example 5: Version-Specific Code

```lwanga
#if VERSION >= 2
// New API (version 2.0+)
fn process_data_v2(data: ptr, len: u64, options: u64) -> u64 {
    // New implementation with options
    return 0;
}

fn process_data(data: ptr, len: u64) -> u64 {
    return process_data_v2(data, len, 0);
}
#else
// Old API (version 1.x)
fn process_data(data: ptr, len: u64) -> u64 {
    // Old implementation
    return 0;
}
#endif

fn main() -> u64 {
    let data: ptr = "test";
    process_data(data, 4);
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting #endif

```lwanga
#if DEBUG
const LOG_LEVEL: u64 = 3;
// ERROR: Missing #endif

fn main() -> u64 {
    return 0;
}
```

**Fix:** Always close #if with #endif.

### Mistake 2: Undefined Macro

```lwanga
#if SOME_UNDEFINED_MACRO
const VALUE: u64 = 1;
#endif

fn main() -> u64 {
    return VALUE;  // ERROR: VALUE might not exist!
}
```

**Fix:** Provide default values:
```lwanga
#if SOME_MACRO
const VALUE: u64 = 1;
#else
const VALUE: u64 = 0;
#endif
```

### Mistake 3: Complex Conditions

```lwanga
#if (DEBUG && FEATURE_A) || (RELEASE && FEATURE_B) || (TEST && !FEATURE_C)
// Too complex!
#endif
```

**Fix:** Break into simpler conditions or use intermediate definitions.

---

## Exercises

### Exercise 1: Platform Detection

Write code that prints different messages on Linux vs other platforms.

<details>
<summary>Solution</summary>

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn main() -> u64 {
    #if LINUX
    let msg: ptr = "Running on Linux\n";
    let len: u64 = 17;
    #else
    let msg: ptr = "Running on other platform\n";
    let len: u64 = 26;
    #endif
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    
    return 0;
}
```
</details>

### Exercise 2: Debug Assertions

Create an assert function that only works in debug builds.

<details>
<summary>Solution</summary>

```lwanga
#if DEBUG
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    if condition == 0 {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, msg, len);
            syscall(60, 1, 0, 0);  // Exit
        }
    }
    return 0;
}
#else
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    return 0;  // No-op in release
}
#endif

fn main() -> u64 {
    assert(1 == 1, "This should pass\n", 17);
    assert(2 + 2 == 4, "Math works\n", 11);
    return 0;
}
```
</details>

### Exercise 3: Feature Flags

Create a program with optional logging and encryption features.

<details>
<summary>Solution</summary>

```lwanga
#if FEATURE_LOGGING
fn log(msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    unsafe {
        syscall(SYS_WRITE, 1, msg, len);
    }
    return 0;
}
#else
fn log(msg: ptr, len: u64) -> u64 {
    return 0;
}
#endif

#if FEATURE_ENCRYPTION
fn encrypt(data: ptr, len: u64) -> u64 {
    // Simple XOR encryption
    let mut i: u64 = 0;
    unsafe {
        while i < len {
            *(data + i) = *(data + i) ^ 0xAA;
            i = i + 1;
        }
    }
    return 0;
}
#else
fn encrypt(data: ptr, len: u64) -> u64 {
    return 0;
}
#endif

fn main() -> u64 {
    log("Starting program\n", 17);
    
    let data: [u8; 5] = [0x48, 0x65, 0x6C, 0x6C, 0x6F];
    encrypt(&data as ptr, 5);
    
    log("Program complete\n", 17);
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What the preprocessor is and why it's useful
- ✅ How to use #if, #elif, #else, #endif directives
- ✅ Writing platform-specific code
- ✅ Using feature flags to enable/disable functionality
- ✅ Creating debug vs release builds
- ✅ Compile-time configuration with -D flags
- ✅ Practical examples of conditional compilation
- ✅ Common mistakes and how to avoid them

**Remember:** The preprocessor runs before compilation, so only the code that matches your conditions ends up in the final binary.

---

## Next Lesson

Now that you understand the preprocessor, let's learn about organizing code with modules in [Lesson 34: Modules](34-modules.md).

---

**Navigation**: [← Previous: Exploit Development](32-exploit-development.md) | [Tutorial Home](README.md) | [Next: Modules →](34-modules.md)
