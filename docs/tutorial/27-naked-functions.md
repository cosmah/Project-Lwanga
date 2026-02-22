# Lesson 27: Naked Functions

**Learn about functions without prologue/epilogue for shellcode.**

---

## What are Naked Functions?

Naked functions have no compiler-generated prologue or epilogue - just your code.

```lwanga
naked fn my_function() -> u64 {
    asm {
        // Your assembly code
        ret
    }
}
```

---

## Simple Naked Function

```lwanga
naked fn return_42() -> u64 {
    asm {
        mov rax, 42
        ret
    }
}

fn main() -> u64 {
    return return_42();  // Returns 42
}
```

---

## Naked Function with Syscall

```lwanga
naked fn exit_naked() -> u64 {
    asm {
        mov rax, 60      // sys_exit
        xor rdi, rdi     // exit code 0
        syscall
    }
}
```

---

## Shellcode Example

```lwanga
naked fn shellcode() -> u64 {
    asm {
        // execve("/bin/sh", NULL, NULL)
        xor rax, rax
        push rax
        mov rdi, 0x68732f6e69622f
        push rdi
        mov rdi, rsp
        xor rsi, rsi
        xor rdx, rdx
        mov rax, 59
        syscall
    }
}
```

---

**Navigation**: [← Previous: Inline Assembly](26-inline-assembly.md) | [Tutorial Home](README.md) | [Next: Registers →](28-registers.md)
