# Lesson 26: Inline Assembly

**Learn how to write assembly code inside Lwanga programs.**

---

## Basic Inline Assembly

```lwanga
fn main() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 42
            mov result, rax
        }
    }
    
    return result;  // Returns 42
}
```

---

## System Call in Assembly

```lwanga
fn exit_asm(code: u64) -> u64 {
    unsafe {
        asm {
            mov rax, 60      // sys_exit
            mov rdi, code    // exit code
            syscall
        }
    }
    return 0;
}
```

---

## Getting Process ID

```lwanga
fn get_pid_asm() -> u64 {
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

---

## Register Operations

```lwanga
fn add_asm(a: u64, b: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a
            add rax, b
            mov result, rax
        }
    }
    
    return result;
}
```

---

**Navigation**: [← Previous: Signals](25-signals.md) | [Tutorial Home](README.md) | [Next: Naked Functions →](27-naked-functions.md)
