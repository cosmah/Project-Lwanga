# Lesson 28: Register Access

**Learn about CPU registers and direct register manipulation.**

---

## x86_64 Registers

### General Purpose Registers

- `rax` - Accumulator (return values, syscall number)
- `rbx` - Base
- `rcx` - Counter
- `rdx` - Data
- `rsi` - Source index
- `rdi` - Destination index
- `rbp` - Base pointer
- `rsp` - Stack pointer
- `r8-r15` - Additional registers

---

## Using Registers in Assembly

```lwanga
fn main() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 100
            mov rbx, 200
            add rax, rbx
            mov result, rax
        }
    }
    
    return result;  // Returns 300
}
```

---

## Syscall Register Convention

For Linux x86_64 syscalls:
- `rax` - Syscall number
- `rdi` - 1st argument
- `rsi` - 2nd argument
- `rdx` - 3rd argument
- `r10` - 4th argument
- `r8` - 5th argument
- `r9` - 6th argument

---

## Example: Write Syscall

```lwanga
fn write_asm(fd: u64, buf: ptr, count: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 1       // sys_write
            mov rdi, fd      // file descriptor
            mov rsi, buf     // buffer
            mov rdx, count   // count
            syscall
            mov result, rax
        }
    }
    
    return result;
}
```

---

**Navigation**: [← Previous: Naked Functions](27-naked-functions.md) | [Tutorial Home](README.md) | [Next: Encrypted Strings →](29-encrypted-strings.md)
