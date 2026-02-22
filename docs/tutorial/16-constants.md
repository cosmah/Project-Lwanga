# Lesson 16: Constants

**Learn how to use constants effectively in your programs.**

---

## Declaring Constants

```lwanga
const MAX_SIZE: u64 = 1024;
const PORT: u16 = 8080;
const BUFFER_LEN: u32 = 512;
```

---

## Using Constants

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 7);
    }
    
    return 0;
}
```

---

## Constants vs Variables

```lwanga
// Constant - compile time
const MAX: u64 = 100;

// Variable - runtime
let x: u64 = 10;
let y: u64 = x * 2;
```

---

## Common Use Cases

```lwanga
// File descriptors
const STDIN: u64 = 0;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;

// Syscall numbers
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_OPEN: u64 = 2;

// Configuration
const MAX_CONNECTIONS: u32 = 100;
const TIMEOUT_SECONDS: u64 = 30;
```

---

**Navigation**: [← Previous: Type System](15-type-system.md) | [Tutorial Home](README.md) | [Next: Memory Model →](17-memory-model.md)
