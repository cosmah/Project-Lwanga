# Lesson 20: Memory Safety

**In this lesson, you'll learn best practices for writing safe, bug-free code, understand common memory vulnerabilities, and master defensive programming techniques.**

---

## Why Memory Safety Matters

Memory bugs are the source of most security vulnerabilities:
- **70% of security bugs** in major software are memory safety issues
- Buffer overflows, use-after-free, null pointer dereferences
- Can lead to crashes, data corruption, or code execution

**Lwanga's approach:**
- Explicit unsafe blocks (you know where danger is)
- Strong type system (prevents many bugs at compile time)
- Manual memory management (you control everything)
- Defensive programming (validate, check, verify)

---

## Common Memory Bugs

### 1. Buffer Overflow

**The problem:** Writing past the end of a buffer.

```lwanga
// DANGEROUS: No bounds checking
fn unsafe_copy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(((src as u64) + i) as ptr) != 0 {
            *(((dest as u64) + i) as ptr) = *(((src as u64) + i) as ptr);
            i = i + 1;  // Could overflow dest!
        }
    }
    
    return i;
}
```

**What can go wrong:**
- Overwrites adjacent memory
- Corrupts other variables
- Crashes the program
- Security vulnerability (code execution)

**The fix:** Always check bounds.

```lwanga
// SAFE: With bounds checking
fn safe_copy(dest: ptr, src: ptr, dest_size: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 {
        return 0;  // Null pointer check
    }
    
    if dest_size == 0 {
        return 0;  // Empty destination
    }
    
    let mut i: u64 = 0;
    
    unsafe {
        while i < dest_size - 1 {  // Leave room for null terminator
            let src_byte: u8 = *(((src as u64) + i) as ptr);
            
            if src_byte == 0 {
                break;  // End of source string
            }
            
            *(((dest as u64) + i) as ptr) = src_byte;
            i = i + 1;
        }
        
        // Null terminate
        *(((dest as u64) + i) as ptr) = 0;
    }
    
    return i;
}

fn main() -> u64 {
    let src: ptr = "Hello, World!";
    let mut dest: [u8; 20] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    let copied: u64 = safe_copy(&dest, src, 20);
    return copied;
}
```

### 2. Null Pointer Dereference

**The problem:** Dereferencing a null (zero) pointer.

```lwanga
// DANGEROUS: No null check
fn unsafe_read(ptr: ptr) -> u64 {
    unsafe {
        return *(ptr as ptr);  // Crashes if ptr is null!
    }
}
```

**The fix:** Always check for null.

```lwanga
// SAFE: With null check
fn safe_read(ptr: ptr) -> u64 {
    if ptr as u64 == 0 {
        return 0;  // Handle null pointer
    }
    
    unsafe {
        return *(ptr as ptr);
    }
}

fn main() -> u64 {
    let x: u64 = 42;
    let value: u64 = safe_read(&x);
    
    let null_value: u64 = safe_read(0 as ptr);  // Returns 0, doesn't crash
    
    return value;
}
```

### 3. Use-After-Free

**The problem:** Using memory after it's been freed.

```lwanga
// DANGEROUS: Returning pointer to local variable
fn get_pointer() -> ptr {
    let x: u64 = 42;
    return &x;  // x will be freed when function returns!
}

fn main() -> u64 {
    let ptr: ptr = get_pointer();
    
    unsafe {
        let value: u64 = *(ptr as ptr);  // DANGER: ptr points to freed memory!
        return value;
    }
}
```

**The fix:** Don't return pointers to local variables.

```lwanga
// SAFE: Return value, not pointer
fn get_value() -> u64 {
    let x: u64 = 42;
    return x;  // Return the value itself
}

fn main() -> u64 {
    let value: u64 = get_value();
    return value;  // Safe!
}
```

### 4. Out-of-Bounds Access

**The problem:** Accessing array elements beyond the array size.

```lwanga
// DANGEROUS: No bounds check
fn unsafe_access(arr: ptr, index: u64) -> u64 {
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);  // Could be out of bounds!
    }
}
```

**The fix:** Always validate indices.

```lwanga
// SAFE: With bounds checking
fn safe_access(arr: ptr, index: u64, length: u64) -> u64 {
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
    
    let valid: u64 = safe_access(&arr, 2, 5);    // Returns 30
    let invalid: u64 = safe_access(&arr, 10, 5);  // Returns 0 (safe!)
    
    return valid;
}
```

### 5. Integer Overflow

**The problem:** Arithmetic that exceeds type limits.

```lwanga
// DANGEROUS: No overflow check
fn unsafe_multiply(a: u8, b: u8) -> u8 {
    return a * b;  // Can overflow!
}

fn main() -> u64 {
    let result: u8 = unsafe_multiply(200, 200);
    // result = 39424 % 256 = 64 (wrong!)
    return result as u64;
}
```

**The fix:** Check for overflow before operating.

```lwanga
// SAFE: With overflow checking
fn safe_multiply(a: u8, b: u8) -> u64 {
    let result: u64 = (a as u64) * (b as u64);
    
    if result > 255 {
        return 0;  // Overflow would occur
    }
    
    return result;
}

fn main() -> u64 {
    let result: u64 = safe_multiply(200, 200);
    return result;  // Returns 0 (overflow detected)
}
```

---

## Defensive Programming Techniques

### Technique 1: Input Validation

Always validate inputs before using them:

```lwanga
fn process_buffer(buf: ptr, len: u64) -> u64 {
    // Validate pointer
    if buf as u64 == 0 {
        return 0;  // Null pointer
    }
    
    // Validate length
    if len == 0 {
        return 0;  // Empty buffer
    }
    
    if len > 1024 * 1024 {
        return 0;  // Too large (1MB limit)
    }
    
    // Safe to process
    unsafe {
        let first_byte: u8 = *(buf as ptr);
        return first_byte as u64;
    }
}
```

### Technique 2: Bounds Checking

Always check array indices:

```lwanga
fn safe_array_get(arr: ptr, index: u64, len: u64) -> u64 {
    if arr as u64 == 0 {
        return 0;
    }
    
    if index >= len {
        return 0;  // Out of bounds
    }
    
    unsafe {
        let elem_ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(elem_ptr as ptr);
    }
}
```

### Technique 3: Size Limits

Enforce maximum sizes:

```lwanga
const MAX_BUFFER_SIZE: u64 = 4096;

fn allocate_buffer(size: u64) -> u64 {
    if size == 0 || size > MAX_BUFFER_SIZE {
        return 0;  // Invalid size
    }
    
    // Safe to allocate
    return size;
}
```

### Technique 4: Null Termination

Always null-terminate strings:

```lwanga
fn safe_string_copy(dest: ptr, src: ptr, dest_size: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 || dest_size == 0 {
        return 0;
    }
    
    let mut i: u64 = 0;
    
    unsafe {
        while i < dest_size - 1 {
            let ch: u8 = *(((src as u64) + i) as ptr);
            
            if ch == 0 {
                break;
            }
            
            *(((dest as u64) + i) as ptr) = ch;
            i = i + 1;
        }
        
        // Always null terminate
        *(((dest as u64) + i) as ptr) = 0;
    }
    
    return i;
}
```

### Technique 5: Overlap Detection

Check for overlapping memory regions:

```lwanga
fn safe_memcpy(dest: ptr, src: ptr, n: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 || n == 0 {
        return 0;
    }
    
    let dest_addr: u64 = dest as u64;
    let src_addr: u64 = src as u64;
    
    // Check for overlap
    if dest_addr < src_addr + n && src_addr < dest_addr + n {
        return 0;  // Overlapping regions!
    }
    
    let mut i: u64 = 0;
    unsafe {
        while i < n {
            *(((dest_addr + i) as ptr) as ptr) = *(((src_addr + i) as ptr) as ptr);
            i = i + 1;
        }
    }
    
    return n;
}
```

---

## Practical Examples

### Example 1: Safe String Length

```lwanga
fn safe_strlen(s: ptr, max_len: u64) -> u64 {
    if s as u64 == 0 {
        return 0;
    }
    
    let mut len: u64 = 0;
    
    unsafe {
        while len < max_len {
            let ch: u8 = *(((s as u64) + len) as ptr);
            
            if ch == 0 {
                break;
            }
            
            len = len + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    let len: u64 = safe_strlen(msg, 1000);
    return len;  // Returns 13
}
```

### Example 2: Safe Array Sum

```lwanga
fn safe_array_sum(arr: ptr, len: u64) -> u64 {
    if arr as u64 == 0 || len == 0 {
        return 0;
    }
    
    if len > 10000 {
        return 0;  // Sanity check
    }
    
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            let elem_ptr: ptr = (arr as u64 + i * 8) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            
            // Check for overflow
            if sum > 18446744073709551615 - value {
                return 0;  // Would overflow
            }
            
            sum = sum + value;
            i = i + 1;
        }
    }
    
    return sum;
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let total: u64 = safe_array_sum(&arr, 5);
    return total;  // Returns 150
}
```

### Example 3: Safe Buffer Write

```lwanga
fn safe_buffer_write(buf: ptr, buf_size: u64, data: ptr, data_len: u64) -> u64 {
    // Validate all inputs
    if buf as u64 == 0 || data as u64 == 0 {
        return 0;
    }
    
    if buf_size == 0 || data_len == 0 {
        return 0;
    }
    
    // Check if data fits in buffer
    if data_len > buf_size {
        return 0;  // Won't fit
    }
    
    // Safe to copy
    let mut i: u64 = 0;
    unsafe {
        while i < data_len {
            *(((buf as u64) + i) as ptr) = *(((data as u64) + i) as ptr);
            i = i + 1;
        }
    }
    
    return data_len;
}
```

---

## Security Checklist

Before using unsafe code, verify:

- [ ] All pointers checked for null
- [ ] All array indices validated
- [ ] All buffer sizes checked
- [ ] No integer overflow possible
- [ ] No use-after-free possible
- [ ] No buffer overflow possible
- [ ] Memory regions don't overlap (for copy operations)
- [ ] Strings are null-terminated
- [ ] Alignment requirements met
- [ ] Input validation performed

---

## Exercises

### Exercise 1: Fix Buffer Overflow

Fix this vulnerable function:

```lwanga
fn vulnerable_copy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while *(((src as u64) + i) as ptr) != 0 {
            *(((dest as u64) + i) as ptr) = *(((src as u64) + i) as ptr);
            i = i + 1;
        }
    }
    return i;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn safe_copy(dest: ptr, src: ptr, dest_size: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 || dest_size == 0 {
        return 0;
    }
    
    let mut i: u64 = 0;
    
    unsafe {
        while i < dest_size - 1 {
            let ch: u8 = *(((src as u64) + i) as ptr);
            
            if ch == 0 {
                break;
            }
            
            *(((dest as u64) + i) as ptr) = ch;
            i = i + 1;
        }
        
        *(((dest as u64) + i) as ptr) = 0;
    }
    
    return i;
}

fn main() -> u64 {
    let src: ptr = "Hello!";
    let mut dest: [u8; 20] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let copied: u64 = safe_copy(&dest, src, 20);
    return copied;
}
```
</details>

### Exercise 2: Add Overflow Check

Add overflow checking to this multiplication:

```lwanga
fn multiply(a: u64, b: u64) -> u64 {
    return a * b;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn safe_multiply(a: u64, b: u64) -> u64 {
    if a == 0 || b == 0 {
        return 0;
    }
    
    // Check if multiplication would overflow
    if a > 18446744073709551615 / b {
        return 0;  // Would overflow
    }
    
    return a * b;
}

fn main() -> u64 {
    let result1: u64 = safe_multiply(1000, 1000);  // OK
    let result2: u64 = safe_multiply(10000000000000000000, 2);  // Overflow
    return result1;
}
```
</details>

### Exercise 3: Validate Array Access

Add proper validation to this function:

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
    return value;
}
```
</details>

### Exercise 4: Detect Memory Overlap

Add overlap detection to this copy function:

```lwanga
fn copy_memory(dest: ptr, src: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < n {
            *(((dest as u64) + i) as ptr) = *(((src as u64) + i) as ptr);
            i = i + 1;
        }
    }
    return n;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn copy_memory(dest: ptr, src: ptr, n: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 || n == 0 {
        return 0;
    }
    
    let dest_addr: u64 = dest as u64;
    let src_addr: u64 = src as u64;
    
    // Check for overlap
    if dest_addr < src_addr + n && src_addr < dest_addr + n {
        return 0;  // Overlapping!
    }
    
    let mut i: u64 = 0;
    unsafe {
        while i < n {
            *(((dest_addr + i) as ptr) as ptr) = *(((src_addr + i) as ptr) as ptr);
            i = i + 1;
        }
    }
    
    return n;
}

fn main() -> u64 {
    let src: [u8; 5] = [1, 2, 3, 4, 5];
    let mut dest: [u8; 5] = [0, 0, 0, 0, 0];
    let copied: u64 = copy_memory(&dest, &src, 5);
    return copied;
}
```
</details>

### Exercise 5: Safe String Concatenation

Write a safe string concatenation function with bounds checking.

<details>
<summary>Solution</summary>

```lwanga
fn safe_strcat(dest: ptr, src: ptr, dest_size: u64) -> u64 {
    if dest as u64 == 0 || src as u64 == 0 || dest_size == 0 {
        return 0;
    }
    
    // Find end of dest string
    let mut dest_len: u64 = 0;
    unsafe {
        while dest_len < dest_size && *(((dest as u64) + dest_len) as ptr) != 0 {
            dest_len = dest_len + 1;
        }
    }
    
    if dest_len >= dest_size {
        return 0;  // No room
    }
    
    // Copy src to end of dest
    let mut i: u64 = 0;
    unsafe {
        while dest_len + i < dest_size - 1 {
            let ch: u8 = *(((src as u64) + i) as ptr);
            
            if ch == 0 {
                break;
            }
            
            *(((dest as u64) + dest_len + i) as ptr) = ch;
            i = i + 1;
        }
        
        // Null terminate
        *(((dest as u64) + dest_len + i) as ptr) = 0;
    }
    
    return dest_len + i;
}

fn main() -> u64 {
    let mut dest: [u8; 20] = [72, 101, 108, 108, 111, 0,  // "Hello"
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let src: ptr = ", World!";
    
    let final_len: u64 = safe_strcat(&dest, src, 20);
    return final_len;  // Returns 13
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Common memory bugs: buffer overflow, null deref, use-after-free, out-of-bounds, integer overflow
- ✅ Defensive programming: input validation, bounds checking, size limits
- ✅ Safety techniques: null checks, overlap detection, null termination
- ✅ Practical examples of safe memory operations
- ✅ Security checklist for unsafe code
- ✅ How to fix vulnerable code

---

## Next Lesson

Now that you understand memory safety, let's learn about system calls in [Lesson 21: System Calls Introduction](21-syscalls-intro.md).

---

**Navigation**: [← Previous: Unsafe Blocks](19-unsafe-blocks.md) | [Tutorial Home](README.md) | [Next: Syscalls Intro →](21-syscalls-intro.md)
