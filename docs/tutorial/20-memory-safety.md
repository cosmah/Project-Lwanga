# Lesson 20: Memory Safety

**Best practices for writing safe, bug-free code.**

---

## Common Memory Bugs

### Buffer Overflow

```lwanga
// Bad: No bounds checking
fn copy_unsafe(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while *(src + i) != 0 {
            *(dest + i) = *(src + i);  // Could overflow dest!
            i = i + 1;
        }
    }
    return i;
}

// Good: With bounds checking
fn copy_safe(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < max_len - 1 && *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        *(dest + i) = 0;  // Null terminate
    }
    return i;
}
```

---

## Null Pointer Checks

```lwanga
fn safe_access(ptr: ptr) -> u64 {
    if ptr == 0 {
        return 0;  // Handle null
    }
    
    unsafe {
        return *ptr;
    }
}
```

---

## Bounds Checking

```lwanga
fn safe_array_access(arr: ptr, index: u64, len: u64) -> u64 {
    if index >= len {
        return 0;  // Out of bounds
    }
    
    unsafe {
        return *(arr + index);
    }
}
```

---

## Input Validation

```lwanga
fn process_buffer(buf: ptr, len: u64) -> u64 {
    // Validate inputs
    if buf == 0 {
        return 0;  // Null pointer
    }
    
    if len == 0 || len > 1024 * 1024 {
        return 0;  // Invalid length
    }
    
    // Safe to process
    return 1;
}
```

---

**Navigation**: [← Previous: Unsafe Blocks](19-unsafe-blocks.md) | [Tutorial Home](README.md) | [Next: Syscalls Intro →](21-syscalls-intro.md)
