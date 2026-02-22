# Lesson 19: Unsafe Operations

**In this lesson, you'll learn when and how to use unsafe blocks properly, understand what makes operations unsafe, and follow best practices for writing safe code with unsafe blocks.**

---

## What is Unsafe?

In Lwanga, `unsafe` is a keyword that marks blocks of code that perform operations the compiler cannot verify as safe.

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;
    
    unsafe {
        let value: u64 = *(ptr as ptr);  // Pointer dereference
        return value;
    }
}
```

**Why have unsafe?**

**Design choice:** Lwanga separates safe and unsafe code:
- Safe code: Compiler guarantees memory safety
- Unsafe code: You guarantee safety (compiler trusts you)

This is better than:
- C: Everything is unsafe (no safety guarantees)
- High-level languages: Nothing is unsafe (can't do low-level operations)

---

## Operations That Require Unsafe

### 1. System Calls

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    
    unsafe {
        syscall(1, 1, msg, 7);  // System call
    }
    
    return 0;
}
```

**Why unsafe?**

System calls interact directly with the kernel:
- Can access any memory
- Can perform privileged operations
- Can crash the program
- Bypass all safety checks

### 2. Pointer Dereferencing

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;
    
    unsafe {
        let value: u64 = *(ptr as ptr);  // Dereference
        return value;
    }
}
```

**Why unsafe?**

Dereferencing a pointer can:
- Access invalid memory
- Cause segmentation faults
- Read uninitialized data
- Violate memory safety

### 3. Inline Assembly

```lwanga
fn get_rax() -> u64 {
    let result: u64 = 0;
    
    unsafe {
        asm {
            mov result, rax
        }
    }
    
    return result;
}
```

**Why unsafe?**

Assembly code can:
- Corrupt registers
- Break calling conventions
- Access any memory
- Bypass type system

### 4. Raw Memory Access

```lwanga
fn write_to_address(addr: u64, value: u64) -> u64 {
    unsafe {
        let ptr: ptr = addr as ptr;
        *(ptr as ptr) = value;
    }
    return 0;
}
```

**Why unsafe?**

Direct memory access can:
- Write to protected memory
- Corrupt data structures
- Cause undefined behavior
- Crash the program

---

## When to Use Unsafe

### Use Case 1: System Programming

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn print(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}

fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    print(msg, 14);
    return 0;
}
```

**Justification:** System calls are necessary for I/O and OS interaction.

### Use Case 2: Performance-Critical Code

```lwanga
fn fast_memcpy(dest: ptr, src: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        // Direct memory access for speed
        while i < n {
            let src_byte: ptr = (src as u64 + i) as ptr;
            let dest_byte: ptr = (dest as u64 + i) as ptr;
            *(dest_byte as ptr) = *(src_byte as ptr);
            i = i + 1;
        }
    }
    
    return n;
}
```

**Justification:** Direct memory operations are faster than safe alternatives.

### Use Case 3: Hardware Interaction

```lwanga
fn read_port(port: u16) -> u8 {
    let value: u8 = 0;
    
    unsafe {
        asm {
            mov dx, port
            in al, dx
            mov value, al
        }
    }
    
    return value;
}
```

**Justification:** Hardware access requires direct port I/O.

### Use Case 4: Interfacing with C Code

```lwanga
fn call_c_function(ptr: ptr, len: u64) -> u64 {
    unsafe {
        // Call external C function
        // (In real code, this would use FFI)
        return 0;
    }
}
```

**Justification:** C functions don't provide safety guarantees.

---

## Minimizing Unsafe Code

### Principle 1: Keep Unsafe Blocks Small

**Bad: Large unsafe block**
```lwanga
unsafe fn process_data(data: ptr, len: u64) -> u64 {
    // 100 lines of code...
    // Hard to audit for safety
    // One mistake anywhere is dangerous
    return 0;
}
```

**Good: Small, focused unsafe blocks**
```lwanga
fn process_data(data: ptr, len: u64) -> u64 {
    // Safe validation
    if data as u64 == 0 || len == 0 {
        return 0;
    }
    
    // Small unsafe block
    unsafe {
        let first_byte: u8 = *(data as ptr);
    }
    
    // More safe code...
    
    return 1;
}
```

### Principle 2: Validate Before Unsafe

```lwanga
fn safe_read(ptr: ptr, offset: u64, max_len: u64) -> u64 {
    // Validate inputs (safe code)
    if ptr as u64 == 0 {
        return 0;  // Null pointer
    }
    
    if offset >= max_len {
        return 0;  // Out of bounds
    }
    
    // Only use unsafe after validation
    unsafe {
        let addr: ptr = (ptr as u64 + offset) as ptr;
        return *(addr as ptr);
    }
}
```

### Principle 3: Wrap Unsafe in Safe Functions

```lwanga
// Unsafe implementation (private)
fn unsafe_write_impl(fd: u64, buf: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(1, fd, buf, count);
    }
}

// Safe wrapper (public)
fn write_to_stdout(msg: ptr, len: u64) -> u64 {
    // Validate inputs
    if msg as u64 == 0 {
        return 0;
    }
    
    if len == 0 {
        return 0;
    }
    
    // Call unsafe implementation
    return unsafe_write_impl(1, msg, len);
}

fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    write_to_stdout(msg, 7);  // Safe interface
    return 0;
}
```

---

## Safety Guidelines

### Guideline 1: Document Why Unsafe is Needed

```lwanga
fn read_memory(addr: u64) -> u64 {
    // SAFETY: Caller must ensure addr is valid and aligned
    // This function is unsafe because it dereferences a raw pointer
    // constructed from an arbitrary address
    unsafe {
        let ptr: ptr = addr as ptr;
        return *(ptr as ptr);
    }
}
```

### Guideline 2: Check Pointers for Null

```lwanga
fn safe_deref(ptr: ptr) -> u64 {
    if ptr as u64 == 0 {
        return 0;  // Null pointer check
    }
    
    unsafe {
        return *(ptr as ptr);
    }
}
```

### Guideline 3: Verify Array Bounds

```lwanga
fn safe_array_access(arr: ptr, index: u64, len: u64) -> u64 {
    if index >= len {
        return 0;  // Bounds check
    }
    
    unsafe {
        let elem_ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(elem_ptr as ptr);
    }
}
```

### Guideline 4: Validate Buffer Sizes

```lwanga
fn safe_buffer_op(buf: ptr, len: u64) -> u64 {
    // Check for null
    if buf as u64 == 0 {
        return 0;
    }
    
    // Check for reasonable size
    if len == 0 || len > 1024 * 1024 {
        return 0;  // Too small or too large
    }
    
    unsafe {
        // Safe to proceed
        return *(buf as ptr);
    }
}
```

### Guideline 5: Check Alignment

```lwanga
fn is_aligned(ptr: ptr, alignment: u64) -> u64 {
    let addr: u64 = ptr as u64;
    if addr % alignment == 0 {
        return 1;  // Aligned
    }
    return 0;  // Not aligned
}

fn safe_aligned_read(ptr: ptr) -> u64 {
    if is_aligned(ptr, 8) == 0 {
        return 0;  // Not aligned for u64
    }
    
    unsafe {
        return *(ptr as ptr);
    }
}
```

---

## Practical Examples

### Example 1: Safe String Length

```lwanga
fn safe_strlen(s: ptr, max_len: u64) -> u64 {
    if s as u64 == 0 {
        return 0;  // Null pointer
    }
    
    let mut len: u64 = 0;
    
    unsafe {
        while len < max_len {
            let char_ptr: ptr = (s as u64 + len) as ptr;
            let ch: u8 = *(char_ptr as ptr);
            
            if ch == 0 {
                break;  // Null terminator
            }
            
            len = len + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let msg: ptr = "Hello!";
    let len: u64 = safe_strlen(msg, 1000);
    return len;  // Returns 6
}
```

### Example 2: Safe Memory Copy

```lwanga
fn safe_memcpy(dest: ptr, src: ptr, n: u64) -> u64 {
    // Validate pointers
    if dest as u64 == 0 || src as u64 == 0 {
        return 0;
    }
    
    // Check for reasonable size
    if n == 0 || n > 1024 * 1024 {
        return 0;
    }
    
    // Check for overlap
    let dest_addr: u64 = dest as u64;
    let src_addr: u64 = src as u64;
    
    if dest_addr < src_addr + n && src_addr < dest_addr + n {
        return 0;  // Overlapping regions
    }
    
    // Safe to copy
    let mut i: u64 = 0;
    unsafe {
        while i < n {
            let src_byte: ptr = (src_addr + i) as ptr;
            let dest_byte: ptr = (dest_addr + i) as ptr;
            *(dest_byte as ptr) = *(src_byte as ptr);
            i = i + 1;
        }
    }
    
    return n;
}
```

### Example 3: Safe System Call Wrapper

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;

fn safe_write(fd: u64, buf: ptr, count: u64) -> u64 {
    // Validate file descriptor
    if fd != STDOUT && fd != STDERR {
        return 0;  // Only allow stdout/stderr
    }
    
    // Validate buffer
    if buf as u64 == 0 {
        return 0;
    }
    
    // Validate count
    if count == 0 || count > 4096 {
        return 0;
    }
    
    // Safe to make syscall
    unsafe {
        return syscall(SYS_WRITE, fd, buf, count);
    }
}

fn main() -> u64 {
    let msg: ptr = "Safe write!\n";
    safe_write(STDOUT, msg, 12);
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting Null Checks

```lwanga
// WRONG: No null check
fn bad_deref(ptr: ptr) -> u64 {
    unsafe {
        return *(ptr as ptr);  // Crashes if ptr is null!
    }
}
```

**Fix:**
```lwanga
fn good_deref(ptr: ptr) -> u64 {
    if ptr as u64 == 0 {
        return 0;
    }
    
    unsafe {
        return *(ptr as ptr);
    }
}
```

### Mistake 2: No Bounds Checking

```lwanga
// WRONG: No bounds check
fn bad_access(arr: ptr, index: u64) -> u64 {
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);  // Could be out of bounds!
    }
}
```

**Fix:**
```lwanga
fn good_access(arr: ptr, index: u64, len: u64) -> u64 {
    if index >= len {
        return 0;
    }
    
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);
    }
}
```

### Mistake 3: Large Unsafe Blocks

```lwanga
// WRONG: Everything in unsafe
unsafe fn bad_function() -> u64 {
    // 100 lines of code
    // Hard to audit
    // One mistake anywhere is dangerous
    return 0;
}
```

**Fix:**
```lwanga
fn good_function() -> u64 {
    // Safe validation
    let valid: u64 = validate_inputs();
    if valid == 0 {
        return 0;
    }
    
    // Small unsafe block
    unsafe {
        // Only unsafe operation
    }
    
    // More safe code
    return 1;
}
```

---

## Exercises

### Exercise 1: Add Null Check

Add a null pointer check to this function:

```lwanga
fn read_value(ptr: ptr) -> u64 {
    unsafe {
        return *(ptr as ptr);
    }
}
```

<details>
<summary>Solution</summary>

```lwanga
fn read_value(ptr: ptr) -> u64 {
    if ptr as u64 == 0 {
        return 0;  // Null pointer
    }
    
    unsafe {
        return *(ptr as ptr);
    }
}

fn main() -> u64 {
    let x: u64 = 42;
    let value: u64 = read_value(&x);
    return value;
}
```
</details>

### Exercise 2: Add Bounds Checking

Add bounds checking to this array access function:

```lwanga
fn get_element(arr: ptr, index: u64) -> u64 {
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);
    }
}
```

<details>
<summary>Solution</summary>

```lwanga
fn get_element(arr: ptr, index: u64, length: u64) -> u64 {
    if arr as u64 == 0 {
        return 0;  // Null pointer
    }
    
    if index >= length {
        return 0;  // Out of bounds
    }
    
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);
    }
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let value: u64 = get_element(&arr, 2, 5);
    return value;  // Returns 30
}
```
</details>

### Exercise 3: Wrap Unsafe in Safe Function

Create a safe wrapper for this unsafe syscall:

```lwanga
fn unsafe_exit(code: u64) -> u64 {
    unsafe {
        return syscall(60, code, 0, 0);
    }
}
```

<details>
<summary>Solution</summary>

```lwanga
fn safe_exit(code: u64) -> u64 {
    // Validate exit code (0-255)
    if code > 255 {
        return unsafe_exit(1);  // Use error code
    }
    
    return unsafe_exit(code);
}

fn unsafe_exit(code: u64) -> u64 {
    unsafe {
        return syscall(60, code, 0, 0);
    }
}

fn main() -> u64 {
    safe_exit(0);
    return 0;
}
```
</details>

### Exercise 4: Validate Buffer Size

Add size validation to this buffer operation:

```lwanga
fn process_buffer(buf: ptr, len: u64) -> u64 {
    unsafe {
        return *(buf as ptr);
    }
}
```

<details>
<summary>Solution</summary>

```lwanga
fn process_buffer(buf: ptr, len: u64) -> u64 {
    if buf as u64 == 0 {
        return 0;  // Null pointer
    }
    
    if len == 0 {
        return 0;  // Empty buffer
    }
    
    if len > 1024 * 1024 {
        return 0;  // Too large (1MB limit)
    }
    
    unsafe {
        return *(buf as ptr);
    }
}

fn main() -> u64 {
    let buffer: [u8; 100] = [42, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                             0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let result: u64 = process_buffer(&buffer, 100);
    return result;
}
```
</details>

### Exercise 5: Minimize Unsafe Block

Refactor this function to minimize the unsafe block:

```lwanga
unsafe fn calculate(ptr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    while i < len {
        let elem_ptr: ptr = (ptr as u64 + i * 8) as ptr;
        sum = sum + *(elem_ptr as ptr);
        i = i + 1;
    }
    
    return sum;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn calculate(ptr: ptr, len: u64) -> u64 {
    // Safe validation
    if ptr as u64 == 0 || len == 0 {
        return 0;
    }
    
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    // Safe loop logic
    while i < len {
        // Small unsafe block for pointer operation
        unsafe {
            let elem_ptr: ptr = (ptr as u64 + i * 8) as ptr;
            sum = sum + *(elem_ptr as ptr);
        }
        i = i + 1;
    }
    
    return sum;
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let total: u64 = calculate(&arr, 5);
    return total;  // Returns 150
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What unsafe means and why it exists
- ✅ Operations that require unsafe: syscalls, pointer deref, assembly, raw memory
- ✅ When to use unsafe: system programming, performance, hardware, C interop
- ✅ How to minimize unsafe code: small blocks, validation, safe wrappers
- ✅ Safety guidelines: null checks, bounds checking, size validation, alignment
- ✅ Practical examples of safe unsafe code
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand unsafe operations, let's learn comprehensive memory safety practices in [Lesson 20: Memory Safety](20-memory-safety.md).

---

**Navigation**: [← Previous: Advanced Pointers](18-pointers-advanced.md) | [Tutorial Home](README.md) | [Next: Memory Safety →](20-memory-safety.md)
