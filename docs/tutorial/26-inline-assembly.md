# Lesson 26: Inline Assembly

**In this lesson, you'll learn how to write assembly code directly inside Lwanga programs, mix high-level and low-level code, and access CPU features directly.**

---

## What is Inline Assembly?

Inline assembly lets you write assembly instructions inside your Lwanga code. This gives you:
- Direct CPU control
- Access to special instructions
- Performance optimization
- Hardware-specific features
- Low-level debugging

**When to use:**
- Performance-critical code
- CPU-specific instructions (CPUID, RDTSC)
- System programming
- Cryptography (AES-NI instructions)
- Atomic operations

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

**Syntax:**
```lwanga
unsafe {
    asm {
        // Assembly instructions here
    }
}
```

**Why unsafe?**

Assembly bypasses all safety checks and can:
- Corrupt memory
- Break calling conventions
- Crash the program
- Violate type safety

---

## Register Operations

### Moving Data

```lwanga
fn move_example() -> u64 {
    let a: u64 = 100;
    let b: u64 = 0;
    
    unsafe {
        asm {
            mov rax, a      // Load a into rax
            mov b, rax      // Store rax into b
        }
    }
    
    return b;  // Returns 100
}
```

### Arithmetic Operations

```lwanga
fn add_asm(x: u64, y: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, x
            add rax, y
            mov result, rax
        }
    }
    
    return result;
}

fn multiply_asm(x: u64, y: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, x
            imul rax, y
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let sum: u64 = add_asm(10, 20);      // 30
    let product: u64 = multiply_asm(5, 6); // 30
    
    return sum + product;  // Returns 60
}
```

---

## System Calls in Assembly

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
            mov result, rax  // Save return value
        }
    }
    
    return result;
}

fn exit_asm(code: u64) -> u64 {
    unsafe {
        asm {
            mov rax, 60      // sys_exit
            mov rdi, code    // exit code
            syscall
        }
    }
    return 0;  // Never reached
}

fn main() -> u64 {
    let msg: ptr = "Hello from assembly!\n";
    write_asm(1, msg, 21);
    
    exit_asm(0);
    return 0;
}
```

---

## CPU Information

### CPUID Instruction

```lwanga
fn get_cpu_vendor() -> u64 {
    let ebx: u64 = 0;
    let ecx: u64 = 0;
    let edx: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 0       // CPUID function 0
            cpuid
            mov ebx, rbx
            mov ecx, rcx
            mov edx, rdx
        }
    }
    
    // ebx:edx:ecx contains vendor string
    return ebx;
}

fn get_cpu_features() -> u64 {
    let edx: u64 = 0;
    
    unsafe {
        asm {
            mov rax, 1       // CPUID function 1
            cpuid
            mov edx, rdx     // Feature flags in EDX
        }
    }
    
    return edx;
}

fn main() -> u64 {
    let vendor: u64 = get_cpu_vendor();
    let features: u64 = get_cpu_features();
    
    return features;
}
```

### Reading Time Stamp Counter

```lwanga
fn rdtsc() -> u64 {
    let low: u64 = 0;
    let high: u64 = 0;
    
    unsafe {
        asm {
            rdtsc
            mov low, rax
            mov high, rdx
        }
    }
    
    return (high << 32) | low;
}

fn benchmark_function() -> u64 {
    let start: u64 = rdtsc();
    
    // Do some work
    let mut i: u64 = 0;
    while i < 1000000 {
        i = i + 1;
    }
    
    let end: u64 = rdtsc();
    
    return end - start;  // Cycles elapsed
}

fn main() -> u64 {
    return benchmark_function();
}
```

---

## Bit Manipulation

### Bit Scanning

```lwanga
fn count_trailing_zeros(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            bsf rax, rax     // Bit scan forward
            mov result, rax
        }
    }
    
    return result;
}

fn count_leading_zeros(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            bsr rax, rax     // Bit scan reverse
            mov result, rax
        }
    }
    
    return 63 - result;
}

fn main() -> u64 {
    let trailing: u64 = count_trailing_zeros(0b1000);  // 3
    let leading: u64 = count_leading_zeros(0b1000);    // 60
    
    return trailing;
}
```

### Population Count

```lwanga
fn popcount(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            popcnt rax, rax  // Count set bits
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let count: u64 = popcount(0b10101010);  // 4 bits set
    return count;
}
```

---

## Atomic Operations

### Compare and Swap

```lwanga
fn compare_and_swap(ptr: ptr, old_val: u64, new_val: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, old_val
            mov rbx, new_val
            mov rcx, ptr
            lock cmpxchg [rcx], rbx
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let mut value: u64 = 10;
    
    // Try to swap 10 with 20
    let old: u64 = compare_and_swap(&value, 10, 20);
    
    return value;  // Returns 20
}
```

### Atomic Increment

```lwanga
fn atomic_increment(ptr: ptr) -> u64 {
    unsafe {
        asm {
            mov rax, ptr
            lock inc qword ptr [rax]
        }
    }
    return 0;
}

fn main() -> u64 {
    let mut counter: u64 = 0;
    
    atomic_increment(&counter);
    atomic_increment(&counter);
    atomic_increment(&counter);
    
    return counter;  // Returns 3
}
```

---

## Practical Examples

### Example 1: Fast Memory Copy

```lwanga
fn fast_memcpy(dest: ptr, src: ptr, count: u64) -> u64 {
    unsafe {
        asm {
            mov rdi, dest
            mov rsi, src
            mov rcx, count
            rep movsb        // Repeat move string byte
        }
    }
    return count;
}

fn main() -> u64 {
    let src: [u8; 10] = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10];
    let mut dest: [u8; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    fast_memcpy(&dest, &src, 10);
    
    return dest[5] as u64;  // Returns 6
}
```

### Example 2: XOR Encryption

```lwanga
fn xor_encrypt(data: ptr, key: u8, len: u64) -> u64 {
    unsafe {
        asm {
            mov rsi, data
            mov al, key
            mov rcx, len
        loop_start:
            xor byte ptr [rsi], al
            inc rsi
            loop loop_start
        }
    }
    return 0;
}

fn main() -> u64 {
    let mut data: [u8; 5] = [72, 101, 108, 108, 111];  // "Hello"
    
    xor_encrypt(&data, 0x42, 5);  // Encrypt
    xor_encrypt(&data, 0x42, 5);  // Decrypt
    
    return data[0] as u64;  // Returns 72 ('H')
}
```

### Example 3: Get Stack Pointer

```lwanga
fn get_stack_pointer() -> u64 {
    let sp: u64 = 0;
    
    unsafe {
        asm {
            mov sp, rsp
        }
    }
    
    return sp;
}

fn main() -> u64 {
    let stack_addr: u64 = get_stack_pointer();
    return stack_addr;
}
```

### Example 4: Pause Instruction

```lwanga
fn spin_wait(iterations: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < iterations {
        unsafe {
            asm {
                pause  // Hint to CPU we're spinning
            }
        }
        i = i + 1;
    }
    
    return 0;
}

fn main() -> u64 {
    return spin_wait(1000);
}
```

---

## Common Mistakes

### Mistake 1: Clobbering Registers

```lwanga
// WRONG: Not preserving registers
unsafe {
    asm {
        mov rbx, 42  // Might break calling convention!
    }
}
```

**Fix:** Use caller-saved registers (rax, rcx, rdx, rsi, rdi, r8-r11):
```lwanga
unsafe {
    asm {
        mov rax, 42  // Safe to use
    }
}
```

### Mistake 2: Wrong Operand Size

```lwanga
// WRONG: Size mismatch
let x: u8 = 0;
unsafe {
    asm {
        mov rax, x  // Loading byte into 64-bit register
    }
}
```

**Fix:**
```lwanga
let x: u8 = 0;
unsafe {
    asm {
        movzx rax, byte ptr x  // Zero-extend byte to 64-bit
    }
}
```

### Mistake 3: Forgetting Unsafe

```lwanga
// WRONG: Assembly requires unsafe
asm {
    mov rax, 42
}
```

**Fix:**
```lwanga
unsafe {
    asm {
        mov rax, 42
    }
}
```

---

## Exercises

### Exercise 1: Swap Two Values

Write a function that swaps two u64 values using assembly.

<details>
<summary>Solution</summary>

```lwanga
fn swap_asm(a: ptr, b: ptr) -> u64 {
    unsafe {
        asm {
            mov rax, a
            mov rbx, b
            mov rcx, [rax]
            mov rdx, [rbx]
            mov [rax], rdx
            mov [rbx], rcx
        }
    }
    return 0;
}

fn main() -> u64 {
    let mut x: u64 = 10;
    let mut y: u64 = 20;
    
    swap_asm(&x, &y);
    
    return x;  // Returns 20
}
```
</details>

### Exercise 2: Absolute Value

Implement absolute value using assembly (without branches).

<details>
<summary>Solution</summary>

```lwanga
fn abs_asm(value: i64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            mov rdx, rax
            sar rdx, 63      // Sign bit to all bits
            xor rax, rdx
            sub rax, rdx
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let neg: u64 = abs_asm(-42);  // 42
    let pos: u64 = abs_asm(42);   // 42
    
    return neg;
}
```
</details>

### Exercise 3: Byte Swap

Swap bytes in a 64-bit value (endianness conversion).

<details>
<summary>Solution</summary>

```lwanga
fn bswap64(value: u64) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            bswap rax
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let x: u64 = 0x0102030405060708;
    let swapped: u64 = bswap64(x);
    
    return swapped;  // 0x0807060504030201
}
```
</details>

### Exercise 4: Find First Set Bit

Find the position of the first set bit (1-indexed).

<details>
<summary>Solution</summary>

```lwanga
fn ffs(value: u64) -> u64 {
    let result: u64 = 0;
    
    if value == 0 {
        return 0;
    }
    
    unsafe {
        asm {
            mov rax, value
            bsf rax, rax
            inc rax          // Make 1-indexed
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let pos: u64 = ffs(0b1000);  // Returns 4
    return pos;
}
```
</details>

### Exercise 5: Rotate Left

Implement rotate left operation.

<details>
<summary>Solution</summary>

```lwanga
fn rotl(value: u64, shift: u8) -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov rax, value
            mov cl, shift
            rol rax, cl
            mov result, rax
        }
    }
    
    return result;
}

fn main() -> u64 {
    let x: u64 = 0x0000000000000001;
    let rotated: u64 = rotl(x, 4);
    
    return rotated;  // 0x0000000000000010
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What inline assembly is and when to use it
- ✅ Basic assembly syntax in Lwanga
- ✅ Register operations and arithmetic
- ✅ System calls in assembly
- ✅ CPU information (CPUID, RDTSC)
- ✅ Bit manipulation instructions
- ✅ Atomic operations
- ✅ Practical examples and optimizations
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand inline assembly, let's learn about naked functions in [Lesson 27: Naked Functions](27-naked-functions.md).

---

**Navigation**: [← Previous: Signals](25-signals.md) | [Tutorial Home](README.md) | [Next: Naked Functions →](27-naked-functions.md)
