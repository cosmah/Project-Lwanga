# Lesson 28: Register Access

**In this lesson, you'll learn about CPU registers, their purposes, and how to manipulate them directly for low-level programming.**

---

## Understanding x86_64 Registers

### General Purpose Registers

**64-bit registers:**
- `rax` - Accumulator (arithmetic, return values, syscall number)
- `rbx` - Base (callee-saved)
- `rcx` - Counter (loops, shifts)
- `rdx` - Data (arithmetic, I/O)
- `rsi` - Source Index (string operations, 2nd parameter)
- `rdi` - Destination Index (string operations, 1st parameter)
- `rbp` - Base Pointer (stack frame base)
- `rsp` - Stack Pointer (top of stack)
- `r8-r15` - Additional general purpose registers

**32-bit versions:** eax, ebx, ecx, edx, esi, edi, ebp, esp, r8d-r15d

**16-bit versions:** ax, bx, cx, dx, si, di, bp, sp, r8w-r15w

**8-bit versions:** al, bl, cl, dl, sil, dil, bpl, spl, r8b-r15b

### Special Purpose Registers

- `rip` - Instruction Pointer (program counter)
- `rflags` - Flags register (status flags)
- `cs, ds, es, fs, gs, ss` - Segment registers

---

## Register Purposes

### Syscall Convention (Linux x86_64)

```lwanga
// syscall(number, arg1, arg2, arg3, arg4, arg5, arg6)
// rax = syscall number
// rdi = 1st argument
// rsi = 2nd argument
// rdx = 3rd argument
// r10 = 4th argument
// r8  = 5th argument
// r9  = 6th argument
// Return value in rax
```

### Function Calling Convention (System V AMD64 ABI)

```lwanga
// Parameters: rdi, rsi, rdx, rcx, r8, r9, then stack
// Return value: rax (rdx for 128-bit)
// Caller-saved: rax, rcx, rdx, rsi, rdi, r8-r11
// Callee-saved: rbx, rbp, r12-r15
```

---

## Reading Registers

### Get Register Values

```lwanga
fn get_rax() -> u64 {
    let value: u64 = 0;
    
    unsafe {
        asm {
            mov value, rax
        }
    }
    
    return value;
}

fn get_all_registers() -> u64 {
    let rax_val: u64 = 0;
    let rbx_val: u64 = 0;
    let rcx_val: u64 = 0;
    let rdx_val: u64 = 0;
    
    unsafe {
        asm {
            mov rax_val, rax
            mov rbx_val, rbx
            mov rcx_val, rcx
            mov rdx_val, rdx
        }
    }
    
    return rax_val;
}

fn main() -> u64 {
    return get_rax();
}
```

### Get Stack and Base Pointers

```lwanga
fn get_stack_info() -> u64 {
    let rsp_val: u64 = 0;
    let rbp_val: u64 = 0;
    
    unsafe {
        asm {
            mov rsp_val, rsp
            mov rbp_val, rbp
        }
    }
    
    return rsp_val;
}

fn main() -> u64 {
    let stack_ptr: u64 = get_stack_info();
    return stack_ptr;
}
```

---

## Writing Registers

### Set Register Values

```lwanga
fn set_registers() -> u64 {
    unsafe {
        asm {
            mov rax, 100
            mov rbx, 200
            mov rcx, 300
            mov rdx, 400
        }
    }
    
    return 0;
}

fn compute_with_registers(a: u64, b: u64, c: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a
            mov rbx, b
            mov rcx, c
            add rax, rbx
            add rax, rcx
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    return compute_with_registers(10, 20, 30);  // Returns 60
}
```

---

## Flags Register

### Reading Flags

```lwanga
fn get_flags() -> u64 {
    let flags: u64 = 0;
    
    unsafe {
        asm {
            pushfq          // Push flags onto stack
            pop flags       // Pop into variable
        }
    }
    
    return flags;
}

fn main() -> u64 {
    let flags: u64 = get_flags();
    return flags;
}
```

### Flag Bits

```lwanga
const CF: u64 = 1 << 0;   // Carry Flag
const PF: u64 = 1 << 2;   // Parity Flag
const AF: u64 = 1 << 4;   // Auxiliary Carry Flag
const ZF: u64 = 1 << 6;   // Zero Flag
const SF: u64 = 1 << 7;   // Sign Flag
const TF: u64 = 1 << 8;   // Trap Flag
const IF: u64 = 1 << 9;   // Interrupt Enable Flag
const DF: u64 = 1 << 10;  // Direction Flag
const OF: u64 = 1 << 11;  // Overflow Flag

fn check_zero_flag() -> u64 {
    let flags: u64 = get_flags();
    
    if (flags & ZF) != 0 {
        return 1;  // Zero flag is set
    }
    
    return 0;
}
```

---

## Practical Examples

### Example 1: Manual Syscall

```lwanga
fn manual_write(fd: u64, buf: ptr, count: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 1       // sys_write
            mov rdi, fd
            mov rsi, buf
            mov rdx, count
            syscall
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let msg: ptr = "Hello from registers!\n";
    let bytes: u64 = manual_write(1, msg, 22);
    return bytes;
}
```

### Example 2: Register Swap

```lwanga
fn swap_registers() -> u64 {
    let a: u64 = 10;
    let b: u64 = 20;
    let result_a: u64 = 0;
    let result_b: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a
            mov rbx, b
            xchg rax, rbx    // Swap rax and rbx
            mov result_a, rax
            mov result_b, rbx
        }
    }
    
    return result_a;  // Returns 20
}

fn main() -> u64 {
    return swap_registers();
}
```

### Example 3: Conditional Moves

```lwanga
fn conditional_max(a: u64, b: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a
            mov rbx, b
            cmp rax, rbx
            cmovl rax, rbx   // Move if less
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let max: u64 = conditional_max(42, 17);
    return max;  // Returns 42
}
```

### Example 4: Bit Testing

```lwanga
fn test_bit(value: u64, bit: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            mov rcx, bit
            bt rax, rcx      // Bit test
            setc al          // Set al if carry (bit was set)
            movzx rax, al
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let is_set: u64 = test_bit(0b1010, 1);  // Test bit 1
    return is_set;  // Returns 1
}
```

### Example 5: Register Rotation

```lwanga
fn rotate_through_registers(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            mov rbx, rax
            mov rcx, rbx
            mov rdx, rcx
            mov rsi, rdx
            mov rdi, rsi
            mov result, rdi
        }
    }
    
    return result;
}

fn main() -> u64 {
    return rotate_through_registers(42);
}
```

---

## Register Preservation

### Caller-Saved Registers

```lwanga
fn caller_saved_example() -> u64 {
    let value: u64 = 42;
    
    unsafe {
        asm {
            mov rax, value
            // rax might be clobbered by function call
        }
    }
    
    // Call another function
    some_function();
    
    // rax value is lost!
    return 0;
}

fn some_function() -> u64 {
    return 0;
}
```

### Callee-Saved Registers

```lwanga
fn callee_saved_example() -> u64 {
    unsafe {
        asm {
            push rbx         // Save
            push r12
            push r13
            
            mov rbx, 100
            mov r12, 200
            mov r13, 300
            
            // Use registers...
            
            pop r13          // Restore
            pop r12
            pop rbx
        }
    }
    
    return 0;
}
```

---

## Advanced Register Techniques

### Register Renaming

```lwanga
fn use_high_registers() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov r8, 10
            mov r9, 20
            mov r10, 30
            mov r11, 40
            
            add r8, r9
            add r8, r10
            add r8, r11
            
            mov result, r8
        }
    }
    
    return result;  // Returns 100
}

fn main() -> u64 {
    return use_high_registers();
}
```

### Partial Register Access

```lwanga
fn partial_registers() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 0x1122334455667788
            mov al, 0xFF     // Modify low byte
            mov result, rax
        }
    }
    
    return result;  // Returns 0x11223344556677FF
}

fn main() -> u64 {
    return partial_registers();
}
```

---

## Common Mistakes

### Mistake 1: Not Preserving Callee-Saved Registers

```lwanga
// WRONG: Modifying rbx without saving
fn bad_function() -> u64 {
    unsafe {
        asm {
            mov rbx, 42  // Corrupts caller's rbx!
        }
    }
    return 0;
}
```

**Fix:**
```lwanga
fn good_function() -> u64 {
    unsafe {
        asm {
            push rbx
            mov rbx, 42
            // Use rbx...
            pop rbx
        }
    }
    return 0;
}
```

### Mistake 2: Wrong Register for Syscall

```lwanga
// WRONG: Using wrong registers
unsafe {
    asm {
        mov rax, 1
        mov rax, 1       // fd should be in rdi!
        mov rbx, msg     // buf should be in rsi!
        syscall
    }
}
```

**Fix:**
```lwanga
unsafe {
    asm {
        mov rax, 1       // syscall number
        mov rdi, 1       // fd
        mov rsi, msg     // buf
        mov rdx, len     // count
        syscall
    }
}
```

### Mistake 3: Assuming Register Values Persist

```lwanga
// WRONG: Assuming rax persists across calls
unsafe {
    asm {
        mov rax, 42
    }
}

some_function();  // rax is clobbered!

unsafe {
    asm {
        // rax no longer contains 42!
    }
}
```

---

## Exercises

### Exercise 1: Sum in Registers

Calculate sum of 5 numbers using only registers.

<details>
<summary>Solution</summary>

```lwanga
fn sum_in_registers() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 10
            mov rbx, 20
            mov rcx, 30
            mov rdx, 40
            mov rsi, 50
            
            add rax, rbx
            add rax, rcx
            add rax, rdx
            add rax, rsi
            
            mov result, rax
        }
    }
    
    return result;  // Returns 150
}

fn main() -> u64 {
    return sum_in_registers();
}
```
</details>

### Exercise 2: Check Overflow Flag

Perform addition and check if overflow occurred.

<details>
<summary>Solution</summary>

```lwanga
fn check_overflow(a: u64, b: u64) -> u64 {
    let overflowed: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a
            add rax, b
            seto al          // Set al if overflow
            movzx rax, al
            mov overflowed, rax
        }
    }
    
    return overflowed;
}

fn main() -> u64 {
    let of: u64 = check_overflow(0xFFFFFFFFFFFFFFFF, 1);
    return of;  // Returns 1 (overflow occurred)
}
```
</details>

### Exercise 3: Register Chain

Pass value through all general purpose registers.

<details>
<summary>Solution</summary>

```lwanga
fn register_chain(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            mov rbx, rax
            mov rcx, rbx
            mov rdx, rcx
            mov rsi, rdx
            mov rdi, rsi
            mov r8, rdi
            mov r9, r8
            mov r10, r9
            mov r11, r10
            mov result, r11
        }
    }
    
    return result;
}

fn main() -> u64 {
    return register_chain(42);  // Returns 42
}
```
</details>

### Exercise 4: Get Instruction Pointer

Get the current instruction pointer value.

<details>
<summary>Solution</summary>

```lwanga
fn get_rip() -> u64 {
    let rip_val: u64 = 0;
    
    unsafe {
        asm {
            lea rax, [rip]
            mov rip_val, rax
        }
    }
    
    return rip_val;
}

fn main() -> u64 {
    let ip: u64 = get_rip();
    return ip;
}
```
</details>

### Exercise 5: Conditional Register Move

Implement ternary operator using conditional move.

<details>
<summary>Solution</summary>

```lwanga
fn ternary(condition: u64, if_true: u64, if_false: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, if_false
            mov rbx, if_true
            test condition, condition
            cmovnz rax, rbx  // Move if not zero
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let val: u64 = ternary(1, 42, 17);
    return val;  // Returns 42
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ x86_64 register architecture and purposes
- ✅ Syscall and function calling conventions
- ✅ Reading and writing registers
- ✅ Flags register and status bits
- ✅ Register preservation (caller/callee-saved)
- ✅ Practical examples and techniques
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand registers, let's learn about encrypted strings in [Lesson 29: Encrypted Strings](29-encrypted-strings.md).

---

**Navigation**: [← Previous: Naked Functions](27-naked-functions.md) | [Tutorial Home](README.md) | [Next: Encrypted Strings →](29-encrypted-strings.md)
