# Lesson 21: System Calls Introduction

**Learn what system calls are and how to use them.**

---

## What are System Calls?

System calls are how programs ask the operating system to do something:
- Write to screen
- Read files
- Create network connections
- Manage processes

---

## The syscall Function

```lwanga
unsafe {
    syscall(number, arg1, arg2, arg3);
}
```

---

## Common System Calls (Linux x86_64)

| Number | Name | Purpose |
|--------|------|---------|
| 0 | read | Read from file |
| 1 | write | Write to file |
| 2 | open | Open file |
| 3 | close | Close file |
| 60 | exit | Exit program |

---

## Example: Write

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 7);
    }
    
    return 0;
}
```

---

## Example: Exit

```lwanga
const SYS_EXIT: u64 = 60;

fn main() -> u64 {
    unsafe {
        syscall(SYS_EXIT, 0, 0, 0);
    }
    return 0;
}
```

---

**Navigation**: [← Previous: Memory Safety](20-memory-safety.md) | [Tutorial Home](README.md) | [Next: File Operations →](22-file-operations.md)
