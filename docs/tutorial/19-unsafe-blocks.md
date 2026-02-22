# Lesson 19: Unsafe Operations

**Learn when and how to use unsafe blocks properly.**

---

## What is Unsafe?

Unsafe blocks allow operations that bypass safety checks:

```lwanga
unsafe {
    // Dangerous operations here
    syscall(1, 1, msg, len);
    let value: u64 = *ptr;
}
```

---

## When to Use Unsafe

- System calls
- Pointer dereferencing
- Inline assembly
- Direct memory access

---

## Minimizing Unsafe

Keep unsafe blocks small:

```lwanga
// Good: Small unsafe block
fn write_msg(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(1, 1, msg, len);
    }
}

// Bad: Large unsafe block
unsafe fn process() -> u64 {
    // 100 lines of code...
}
```

---

## Safety Guidelines

1. Keep unsafe blocks minimal
2. Document why unsafe is needed
3. Validate inputs before unsafe
4. Check pointers for null
5. Verify array bounds

---

**Navigation**: [← Previous: Advanced Pointers](18-pointers-advanced.md) | [Tutorial Home](README.md) | [Next: Memory Safety →](20-memory-safety.md)
