# Lesson 27: Naked Functions

**In this lesson, you'll learn about naked functions - functions without compiler-generated prologue/epilogue code, essential for shellcode and low-level programming.**

---

## What are Naked Functions?

A naked function is a function where the compiler generates NO code except what you write. No function prologue, no epilogue, no stack frame setup - just your assembly code.

**Normal function:**
```asm
push rbp          ; Prologue
mov rbp, rsp
sub rsp, 16
; Your code here
leave             ; Epilogue
ret
```

**Naked function:**
```asm
; Your code here (that's it!)
```

**Why use naked functions?**
- Shellcode development (position-independent code)
- Bootloaders and firmware
- Interrupt handlers
- Performance-critical code
- Exploit development
- Custom calling conventions

---

## Declaring Naked Functions

```lwanga
naked fn function_name() -> u64 {
    asm {
        // Your assembly code
        // YOU must handle everything
        ret
    }
}
```

**Important:** You are responsible for:
- Saving/restoring registers
- Stack management
- Return instruction
- Calling conventions
- Everything!

---

## Simple Naked Functions

### Return a Constant

```lwanga
naked fn return_42() -> u64 {
    asm {
        mov rax, 42
        ret
    }
}

fn main() -> u64 {
    let value: u64 = return_42();
    return value;  // Returns 42
}
```

**What happens:**
1. `mov rax, 42` - Put return value in rax
2. `ret` - Return to caller
3. No prologue/epilogue generated

### Add Two Numbers

```lwanga
naked fn add_naked(a: u64, b: u64) -> u64 {
    asm {
        mov rax, rdi    // First parameter in rdi
        add rax, rsi    // Second parameter in rsi
        ret             // Return value in rax
    }
}

fn main() -> u64 {
    let sum: u64 = add_naked(10, 20);
    return sum;  // Returns 30
}
```

**Calling convention (x86_64 System V):**
- Parameters: rdi, rsi, rdx, rcx, r8, r9
- Return value: rax
- Caller-saved: rax, rcx, rdx, rsi, rdi, r8-r11
- Callee-saved: rbx, rbp, r12-r15

---

## System Calls in Naked Functions

### Exit Program

```lwanga
naked fn exit_naked(code: u64) -> u64 {
    asm {
        mov rax, 60      // sys_exit
        // code already in rdi (first parameter)
        syscall
    }
}

fn main() -> u64 {
    exit_naked(0);
    return 0;  // Never reached
}
```

### Write to Stdout

```lwanga
naked fn write_naked(fd: u64, buf: ptr, count: u64) -> u64 {
    asm {
        mov rax, 1       // sys_write
        // fd already in rdi
        // buf already in rsi
        // count already in rdx
        syscall
        ret
    }
}

fn main() -> u64 {
    let msg: ptr = "Hello from naked function!\n";
    write_naked(1, msg, 27);
    return 0;
}
```

---

## Shellcode Development

### Basic Shellcode Structure

```lwanga
naked fn shellcode_exit() -> u64 {
    asm {
        xor rdi, rdi     // exit code 0
        mov rax, 60      // sys_exit
        syscall
    }
}
```

**Why `xor rdi, rdi` instead of `mov rdi, 0`?**
- Shorter opcode (2 bytes vs 7 bytes)
- No null bytes (important for shellcode)
- Position-independent

### Execve Shellcode

```lwanga
naked fn shellcode_execve() -> u64 {
    asm {
        // execve("/bin/sh", NULL, NULL)
        xor rax, rax
        push rax                    // NULL terminator
        mov rdi, 0x68732f6e69622f   // "/bin/sh" in reverse
        push rdi
        mov rdi, rsp                // rdi = pointer to "/bin/sh"
        xor rsi, rsi                // argv = NULL
        xor rdx, rdx                // envp = NULL
        mov rax, 59                 // sys_execve
        syscall
    }
}

fn main() -> u64 {
    shellcode_execve();
    return 0;  // Never reached
}
```

**How it works:**
1. Push NULL terminator on stack
2. Push "/bin/sh" string on stack
3. Point rdi to the string
4. Set argv and envp to NULL
5. Call execve syscall

### Position-Independent Shellcode

```lwanga
naked fn pic_shellcode() -> u64 {
    asm {
        jmp get_string
    do_call:
        pop rdi                     // Get string address
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

**Why position-independent?**
- Works at any memory address
- Essential for exploits
- No hardcoded addresses

---

## Practical Examples

### Example 1: Fast String Length

```lwanga
naked fn strlen_naked(s: ptr) -> u64 {
    asm {
        xor rax, rax        // counter = 0
        mov rcx, rdi        // rcx = string pointer
    loop_start:
        cmp byte ptr [rcx], 0
        je loop_end
        inc rax
        inc rcx
        jmp loop_start
    loop_end:
        ret
    }
}

fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    let len: u64 = strlen_naked(msg);
    return len;  // Returns 13
}
```

### Example 2: Memory Copy

```lwanga
naked fn memcpy_naked(dest: ptr, src: ptr, n: u64) -> ptr {
    asm {
        mov rax, rdi        // Save dest for return
        mov rcx, rdx        // rcx = count
        rep movsb           // Copy bytes
        ret
    }
}

fn main() -> u64 {
    let src: [u8; 5] = [1, 2, 3, 4, 5];
    let mut dest: [u8; 5] = [0, 0, 0, 0, 0];
    
    memcpy_naked(&dest, &src, 5);
    
    return dest[2] as u64;  // Returns 3
}
```

### Example 3: Get Return Address

```lwanga
naked fn get_return_address() -> u64 {
    asm {
        mov rax, [rsp]      // Return address is on stack
        ret
    }
}

fn main() -> u64 {
    let ret_addr: u64 = get_return_address();
    return ret_addr;
}
```

### Example 4: Stack Pivot

```lwanga
naked fn stack_pivot(new_stack: ptr) -> u64 {
    asm {
        mov rsp, rdi        // Switch to new stack
        xor rax, rax
        ret
    }
}
```

---

## Shellcode Techniques

### Avoiding Null Bytes

```lwanga
// BAD: Contains null bytes
naked fn bad_shellcode() -> u64 {
    asm {
        mov rax, 0          // 48 c7 c0 00 00 00 00 (null bytes!)
        syscall
    }
}

// GOOD: No null bytes
naked fn good_shellcode() -> u64 {
    asm {
        xor rax, rax        // 48 31 c0 (no nulls)
        syscall
    }
}
```

### Self-Modifying Code

```lwanga
naked fn self_modify() -> u64 {
    asm {
        lea rax, [rip + target]
        mov byte ptr [rax], 0xc3    // Write 'ret' instruction
    target:
        nop                          // Will be replaced with 'ret'
    }
}
```

### Polymorphic Shellcode

```lwanga
naked fn polymorphic() -> u64 {
    asm {
        // Decrypt payload
        lea rsi, [rip + payload]
        mov rcx, 10
    decrypt_loop:
        xor byte ptr [rsi], 0x42
        inc rsi
        loop decrypt_loop
        
        // Execute decrypted code
        lea rax, [rip + payload]
        jmp rax
        
    payload:
        .byte 0x42, 0x42, 0x42  // Encrypted instructions
    }
}
```

---

## Common Mistakes

### Mistake 1: Forgetting Return

```lwanga
// WRONG: No return instruction
naked fn bad_function() -> u64 {
    asm {
        mov rax, 42
        // Missing ret!
    }
}
```

**Fix:**
```lwanga
naked fn good_function() -> u64 {
    asm {
        mov rax, 42
        ret
    }
}
```

### Mistake 2: Not Following Calling Convention

```lwanga
// WRONG: Wrong register for parameter
naked fn bad_add(a: u64, b: u64) -> u64 {
    asm {
        mov rax, rax    // a is in rdi, not rax!
        add rax, rbx    // b is in rsi, not rbx!
        ret
    }
}
```

**Fix:**
```lwanga
naked fn good_add(a: u64, b: u64) -> u64 {
    asm {
        mov rax, rdi    // First parameter
        add rax, rsi    // Second parameter
        ret
    }
}
```

### Mistake 3: Corrupting Callee-Saved Registers

```lwanga
// WRONG: Modifying rbx without saving
naked fn bad_function() -> u64 {
    asm {
        mov rbx, 42     // Corrupts caller's rbx!
        mov rax, rbx
        ret
    }
}
```

**Fix:**
```lwanga
naked fn good_function() -> u64 {
    asm {
        push rbx        // Save
        mov rbx, 42
        mov rax, rbx
        pop rbx         // Restore
        ret
    }
}
```

---

## Exercises

### Exercise 1: Naked Multiply

Write a naked function that multiplies two numbers.

<details>
<summary>Solution</summary>

```lwanga
naked fn multiply_naked(a: u64, b: u64) -> u64 {
    asm {
        mov rax, rdi
        imul rax, rsi
        ret
    }
}

fn main() -> u64 {
    let result: u64 = multiply_naked(6, 7);
    return result;  // Returns 42
}
```
</details>

### Exercise 2: Naked Max

Write a naked function that returns the maximum of two numbers.

<details>
<summary>Solution</summary>

```lwanga
naked fn max_naked(a: u64, b: u64) -> u64 {
    asm {
        mov rax, rdi
        cmp rax, rsi
        cmovl rax, rsi  // Conditional move if less
        ret
    }
}

fn main() -> u64 {
    let result: u64 = max_naked(10, 20);
    return result;  // Returns 20
}
```
</details>

### Exercise 3: Shellcode Write

Write shellcode that prints "Hi\n" and exits.

<details>
<summary>Solution</summary>

```lwanga
naked fn shellcode_write() -> u64 {
    asm {
        // write(1, "Hi\n", 3)
        mov rax, 1
        mov rdi, 1
        lea rsi, [rip + msg]
        mov rdx, 3
        syscall
        
        // exit(0)
        mov rax, 60
        xor rdi, rdi
        syscall
        
    msg:
        .ascii "Hi\n"
    }
}

fn main() -> u64 {
    shellcode_write();
    return 0;
}
```
</details>

### Exercise 4: Get Stack Pointer

Write a naked function that returns the current stack pointer.

<details>
<summary>Solution</summary>

```lwanga
naked fn get_rsp() -> u64 {
    asm {
        mov rax, rsp
        ret
    }
}

fn main() -> u64 {
    let sp: u64 = get_rsp();
    return sp;
}
```
</details>

### Exercise 5: Null-Free Shellcode

Write shellcode that exits with code 0, avoiding all null bytes.

<details>
<summary>Solution</summary>

```lwanga
naked fn nullfree_exit() -> u64 {
    asm {
        xor rax, rax    // rax = 0 (no nulls)
        mov al, 60      // rax = 60 (no nulls)
        xor rdi, rdi    // rdi = 0 (no nulls)
        syscall
    }
}

fn main() -> u64 {
    nullfree_exit();
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What naked functions are and why they're useful
- ✅ How to declare naked functions
- ✅ Calling conventions and register usage
- ✅ System calls in naked functions
- ✅ Shellcode development techniques
- ✅ Position-independent code
- ✅ Avoiding null bytes
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand naked functions, let's learn about direct register access in [Lesson 28: Register Access](28-registers.md).

---

**Navigation**: [← Previous: Inline Assembly](26-inline-assembly.md) | [Tutorial Home](README.md) | [Next: Registers →](28-registers.md)
