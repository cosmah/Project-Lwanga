# Lesson 34: Modules and Imports

**In this lesson, you'll learn how to organize code across multiple files using modules, making your projects more maintainable and reusable.**

---

## What are Modules?

Modules are separate files that contain related code. They help you:

- **Organize code** - Group related functions together
- **Reuse code** - Share code across multiple programs
- **Manage complexity** - Break large programs into smaller pieces
- **Collaborate** - Multiple people can work on different modules
- **Maintain code** - Easier to find and fix bugs
- **Create libraries** - Build reusable components

**Design choice:** Lwanga uses a simple `import` statement similar to Python and Go, making module usage straightforward and intuitive.

---

## Creating Your First Module

### Simple I/O Module

Create a file named `lib/io.lwanga`:

```lwanga
// lib/io.lwanga
const STDOUT: u64 = 1;
const STDERR: u64 = 2;
const SYS_WRITE: u64 = 1;

fn print(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}

fn println(msg: ptr, len: u64) -> u64 {
    print(msg, len);
    print("\n", 1);
    return 0;
}

fn eprint(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDERR, msg, len);
    }
}

fn eprintln(msg: ptr, len: u64) -> u64 {
    eprint(msg, len);
    eprint("\n", 1);
    return 0;
}
```

### Using the Module

Create `main.lwanga`:

```lwanga
import "lib/io.lwanga";

fn main() -> u64 {
    println("Hello from module!", 19);
    println("This is easy!", 13);
    return 0;
}
```

**Compile and run:**
```bash
lwangac main.lwanga -o program
./program
```

**Output:**
```
Hello from module!
This is easy!
```

---

## Import Syntax

### Basic Import

```lwanga
import "lib/io.lwanga";
```

**Rules:**
- Path is relative to the source file
- Use forward slashes (/) even on Windows
- Include the .lwanga extension
- Path is in quotes

### Multiple Imports

```lwanga
import "lib/io.lwanga";
import "lib/string.lwanga";
import "lib/math.lwanga";
import "utils/network.lwanga";

fn main() -> u64 {
    // Use functions from all imported modules
    return 0;
}
```

### Nested Imports

Modules can import other modules:

```lwanga
// lib/advanced_io.lwanga
import "lib/io.lwanga";

fn print_header(title: ptr, len: u64) -> u64 {
    println("===================", 19);
    println(title, len);
    println("===================", 19);
    return 0;
}
```

---

## Module Organization

### Recommended Project Structure

```
project/
├── main.lwanga
├── lib/
│   ├── io.lwanga
│   ├── string.lwanga
│   ├── math.lwanga
│   └── net.lwanga
├── utils/
│   ├── crypto.lwanga
│   └── encoding.lwanga
└── tests/
    ├── test_io.lwanga
    └── test_string.lwanga
```

**Best practices:**
- `lib/` - Core library modules
- `utils/` - Utility functions
- `tests/` - Test files
- Keep related code together
- Use descriptive names

---

## Practical Module Examples

### Example 1: String Module

Create `lib/string.lwanga`:

```lwanga
// lib/string.lwanga

fn string_length(s: ptr) -> u64 {
    let mut len: u64 = 0;
    
    unsafe {
        while *(s + len) != 0 {
            len = len + 1;
        }
    }
    
    return len;
}

fn string_compare(s1: ptr, s2: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let c1: u8 = *(s1 + i);
            let c2: u8 = *(s2 + i);
            
            if c1 != c2 {
                return 0;  // Not equal
            }
            
            i = i + 1;
        }
    }
    
    return 1;  // Equal
}

fn string_copy(dest: ptr, src: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return 0;
}

fn string_concat(dest: ptr, src: ptr, dest_len: u64, src_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < src_len {
            *(dest + dest_len + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return dest_len + src_len;
}
```

**Using the string module:**

```lwanga
import "lib/string.lwanga";
import "lib/io.lwanga";

fn main() -> u64 {
    let s1: ptr = "Hello";
    let s2: ptr = "World";
    
    let len1: u64 = string_length(s1);
    let len2: u64 = string_length(s2);
    
    println("String lengths calculated", 25);
    
    return 0;
}
```

### Example 2: Math Module

Create `lib/math.lwanga`:

```lwanga
// lib/math.lwanga

fn abs(n: i64) -> u64 {
    if n < 0 {
        return (-n) as u64;
    }
    return n as u64;
}

fn min(a: u64, b: u64) -> u64 {
    if a < b {
        return a;
    }
    return b;
}

fn max(a: u64, b: u64) -> u64 {
    if a > b {
        return a;
    }
    return b;
}

fn power(base: u64, exp: u64) -> u64 {
    if exp == 0 {
        return 1;
    }
    
    let mut result: u64 = 1;
    let mut i: u64 = 0;
    
    while i < exp {
        result = result * base;
        i = i + 1;
    }
    
    return result;
}

fn gcd(a: u64, b: u64) -> u64 {
    let mut x: u64 = a;
    let mut y: u64 = b;
    
    while y != 0 {
        let temp: u64 = y;
        y = x % y;
        x = temp;
    }
    
    return x;
}

fn factorial(n: u64) -> u64 {
    if n == 0 || n == 1 {
        return 1;
    }
    
    let mut result: u64 = 1;
    let mut i: u64 = 2;
    
    while i <= n {
        result = result * i;
        i = i + 1;
    }
    
    return result;
}
```

**Using the math module:**

```lwanga
import "lib/math.lwanga";
import "lib/io.lwanga";

fn main() -> u64 {
    let a: u64 = 10;
    let b: u64 = 20;
    
    let minimum: u64 = min(a, b);
    let maximum: u64 = max(a, b);
    let pow: u64 = power(2, 10);
    let fact: u64 = factorial(5);
    
    println("Math operations complete", 24);
    
    return 0;
}
```

### Example 3: Network Module

Create `lib/net.lwanga`:

```lwanga
// lib/net.lwanga

const SYS_SOCKET: u64 = 41;
const SYS_CONNECT: u64 = 42;
const SYS_BIND: u64 = 49;
const SYS_LISTEN: u64 = 50;
const SYS_ACCEPT: u64 = 43;
const SYS_CLOSE: u64 = 3;

const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;

packed struct sockaddr_in {
    sin_family: u16,
    sin_port: u16,
    sin_addr: u32,
    sin_zero: [u8; 8],
}

fn htons(port: u16) -> u16 {
    return ((port & 0xFF) << 8) | ((port >> 8) & 0xFF);
}

fn create_socket() -> u64 {
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    return sockfd;
}

fn connect_socket(sockfd: u64, ip: u32, port: u16) -> u64 {
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: ip,
        sin_zero: [0; 8],
    };
    
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_CONNECT, sockfd, &addr, 16);
    }
    
    return result;
}

fn bind_socket(sockfd: u64, port: u16) -> u64 {
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: 0,
        sin_zero: [0; 8],
    };
    
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_BIND, sockfd, &addr, 16);
    }
    
    return result;
}

fn listen_socket(sockfd: u64, backlog: u32) -> u64 {
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_LISTEN, sockfd, backlog, 0);
    }
    return result;
}

fn accept_connection(sockfd: u64) -> u64 {
    let clientfd: u64 = 0;
    unsafe {
        clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
    }
    return clientfd;
}

fn close_socket(sockfd: u64) -> u64 {
    unsafe {
        return syscall(SYS_CLOSE, sockfd, 0, 0);
    }
}
```

**Using the network module:**

```lwanga
import "lib/net.lwanga";
import "lib/io.lwanga";

fn main() -> u64 {
    let sockfd: u64 = create_socket();
    
    if sockfd == -1 {
        eprintln("Failed to create socket", 24);
        return 1;
    }
    
    println("Socket created successfully", 27);
    
    close_socket(sockfd);
    return 0;
}
```

### Example 4: Crypto Module

Create `utils/crypto.lwanga`:

```lwanga
// utils/crypto.lwanga

fn xor_encrypt(data: ptr, len: u64, key: u8) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            *(data + i) = *(data + i) ^ key;
            i = i + 1;
        }
    }
    
    return 0;
}

fn xor_decrypt(data: ptr, len: u64, key: u8) -> u64 {
    // XOR encryption is symmetric
    return xor_encrypt(data, len, key);
}

fn rot13(data: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let c: u8 = *(data + i);
            
            // Uppercase letters
            if c >= 65 && c <= 90 {
                *(data + i) = ((c - 65 + 13) % 26) + 65;
            }
            // Lowercase letters
            else if c >= 97 && c <= 122 {
                *(data + i) = ((c - 97 + 13) % 26) + 97;
            }
            
            i = i + 1;
        }
    }
    
    return 0;
}

fn simple_hash(data: ptr, len: u64) -> u64 {
    let mut hash: u64 = 5381;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let c: u8 = *(data + i);
            hash = ((hash << 5) + hash) + c as u64;
            i = i + 1;
        }
    }
    
    return hash;
}
```

**Using the crypto module:**

```lwanga
import "utils/crypto.lwanga";
import "lib/io.lwanga";

fn main() -> u64 {
    let mut message: [u8; 13] = [72, 101, 108, 108, 111, 32, 87, 111, 114, 108, 100, 33, 0];
    
    println("Original message", 16);
    
    // Encrypt
    xor_encrypt(&message as ptr, 12, 0xAA);
    println("Encrypted", 9);
    
    // Decrypt
    xor_decrypt(&message as ptr, 12, 0xAA);
    println("Decrypted", 9);
    
    // Hash
    let hash: u64 = simple_hash(&message as ptr, 12);
    
    return 0;
}
```

### Example 5: Complete Application with Modules

**Project structure:**
```
app/
├── main.lwanga
├── lib/
│   ├── io.lwanga
│   ├── string.lwanga
│   └── net.lwanga
└── utils/
    └── crypto.lwanga
```

**main.lwanga:**

```lwanga
import "lib/io.lwanga";
import "lib/string.lwanga";
import "lib/net.lwanga";
import "utils/crypto.lwanga";

fn process_message(msg: ptr, len: u64) -> u64 {
    // Hash the message
    let hash: u64 = simple_hash(msg, len);
    
    // Encrypt it
    xor_encrypt(msg, len, 0x42);
    
    println("Message processed", 17);
    
    return hash;
}

fn main() -> u64 {
    println("=== Application Started ===", 27);
    
    // Create network socket
    let sockfd: u64 = create_socket();
    
    if sockfd == -1 {
        eprintln("Network error", 13);
        return 1;
    }
    
    // Process some data
    let mut data: [u8; 20] = [0x48, 0x65, 0x6C, 0x6C, 0x6F, 0; 20];
    let hash: u64 = process_message(&data as ptr, 5);
    
    // Clean up
    close_socket(sockfd);
    
    println("=== Application Complete ===", 28);
    
    return 0;
}
```

---

## Module Best Practices

### 1. Single Responsibility

Each module should have one clear purpose:

```lwanga
// Good: Focused module
// lib/file_io.lwanga - Only file operations

// Bad: Mixed responsibilities
// lib/everything.lwanga - File I/O, networking, crypto, etc.
```

### 2. Clear Naming

Use descriptive names that explain what the module does:

```lwanga
// Good
import "lib/network.lwanga";
import "utils/string_helpers.lwanga";

// Bad
import "lib/stuff.lwanga";
import "utils/misc.lwanga";
```

### 3. Minimal Dependencies

Keep modules independent when possible:

```lwanga
// Good: io.lwanga doesn't import anything

// Acceptable: advanced_io.lwanga imports io.lwanga

// Bad: Circular dependencies
// a.lwanga imports b.lwanga
// b.lwanga imports a.lwanga
```

### 4. Document Your Modules

Add comments explaining what the module does:

```lwanga
// lib/crypto.lwanga
// 
// Cryptographic utilities for the application.
// Provides XOR encryption, ROT13, and simple hashing.
//
// Functions:
// - xor_encrypt: XOR-based encryption
// - xor_decrypt: XOR-based decryption
// - rot13: ROT13 cipher
// - simple_hash: DJB2 hash function

fn xor_encrypt(data: ptr, len: u64, key: u8) -> u64 {
    // Implementation
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Wrong Import Path

```lwanga
// Wrong: Absolute path
import "/home/user/project/lib/io.lwanga";

// Wrong: Missing extension
import "lib/io";

// Correct: Relative path with extension
import "lib/io.lwanga";
```

### Mistake 2: Circular Imports

```lwanga
// module_a.lwanga
import "module_b.lwanga";

// module_b.lwanga
import "module_a.lwanga";  // ERROR: Circular dependency!
```

**Fix:** Restructure code to avoid circular dependencies.

### Mistake 3: Duplicate Function Names

```lwanga
// lib/io.lwanga
fn print(msg: ptr, len: u64) -> u64 {
    return 0;
}

// lib/debug.lwanga
fn print(msg: ptr, len: u64) -> u64 {  // Conflict!
    return 0;
}

// main.lwanga
import "lib/io.lwanga";
import "lib/debug.lwanga";

fn main() -> u64 {
    print("Hello", 5);  // Which print()?
    return 0;
}
```

**Fix:** Use unique function names or prefixes:
```lwanga
// lib/debug.lwanga
fn debug_print(msg: ptr, len: u64) -> u64 {
    return 0;
}
```

---

## Exercises

### Exercise 1: Create a Buffer Module

Create a module with buffer manipulation functions.

<details>
<summary>Solution</summary>

```lwanga
// lib/buffer.lwanga

fn buffer_fill(buf: ptr, len: u64, value: u8) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < len {
            *(buf + i) = value;
            i = i + 1;
        }
    }
    return 0;
}

fn buffer_copy(dest: ptr, src: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < len {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    return 0;
}

fn buffer_compare(buf1: ptr, buf2: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < len {
            if *(buf1 + i) != *(buf2 + i) {
                return 0;
            }
            i = i + 1;
        }
    }
    return 1;
}
```

**Usage:**
```lwanga
import "lib/buffer.lwanga";

fn main() -> u64 {
    let mut buf: [u8; 10] = [0; 10];
    buffer_fill(&buf as ptr, 10, 0x42);
    return 0;
}
```
</details>

### Exercise 2: Create a Conversion Module

Create a module that converts numbers to strings.

<details>
<summary>Solution</summary>

```lwanga
// lib/convert.lwanga

fn u64_to_string(n: u64, buf: ptr, buf_size: u64) -> u64 {
    if n == 0 {
        unsafe {
            *(buf + 0) = 48;  // '0'
        }
        return 1;
    }
    
    let mut num: u64 = n;
    let mut len: u64 = 0;
    
    // Convert digits (reversed)
    unsafe {
        while num > 0 && len < buf_size {
            *(buf + len) = (num % 10) as u8 + 48;
            num = num / 10;
            len = len + 1;
        }
    }
    
    // Reverse the string
    let mut i: u64 = 0;
    unsafe {
        while i < len / 2 {
            let temp: u8 = *(buf + i);
            *(buf + i) = *(buf + len - i - 1);
            *(buf + len - i - 1) = temp;
            i = i + 1;
        }
    }
    
    return len;
}
```

**Usage:**
```lwanga
import "lib/convert.lwanga";
import "lib/io.lwanga";

fn main() -> u64 {
    let mut buffer: [u8; 20] = [0; 20];
    let len: u64 = u64_to_string(12345, &buffer as ptr, 20);
    println(&buffer as ptr, len);
    return 0;
}
```
</details>

### Exercise 3: Combine Multiple Modules

Create a program that uses io, string, and math modules together.

<details>
<summary>Solution</summary>

```lwanga
import "lib/io.lwanga";
import "lib/string.lwanga";
import "lib/math.lwanga";

fn main() -> u64 {
    println("=== Module Demo ===", 19);
    
    // String operations
    let s1: ptr = "Hello";
    let len: u64 = string_length(s1);
    
    // Math operations
    let pow: u64 = power(2, len);
    let fact: u64 = factorial(len);
    
    println("Calculations complete", 21);
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What modules are and why they're useful
- ✅ How to create and organize modules
- ✅ Import syntax and usage
- ✅ Recommended project structure
- ✅ Practical module examples (I/O, string, math, network, crypto)
- ✅ Module best practices
- ✅ Common mistakes and how to avoid them
- ✅ Building complete applications with multiple modules

**Remember:** Good module organization makes your code easier to understand, maintain, and reuse.

---

## Next Lesson

Now that you can organize code with modules, let's learn about optimization techniques in [Lesson 35: Optimization](35-optimization.md).

---

**Navigation**: [← Previous: Preprocessor](33-preprocessor.md) | [Tutorial Home](README.md) | [Next: Optimization →](35-optimization.md)
