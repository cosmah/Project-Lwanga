# Lesson 35: Optimization Techniques

**Learn how to write fast, efficient code.**

---

## Compiler Optimization Levels

```bash
lwangac program.lwanga -o program -O0  # No optimization
lwangac program.lwanga -o program -O1  # Basic
lwangac program.lwanga -o program -O2  # Moderate (recommended)
lwangac program.lwanga -o program -O3  # Aggressive
```

---

## Loop Optimization

### Bad: Repeated Calculation

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    while i < len * 2 / 2 {  // Calculated every iteration!
        sum = sum + *(arr + i);
        i = i + 1;
    }
    
    return sum;
}
```

### Good: Calculate Once

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    let limit: u64 = len;  // Calculate once
    
    while i < limit {
        sum = sum + *(arr + i);
        i = i + 1;
    }
    
    return sum;
}
```

---

## Inline Functions

Small functions are automatically inlined by the compiler at -O2 and above.

---

## Bit Operations vs Arithmetic

```lwanga
// Slower
let x: u64 = n / 2;
let y: u64 = n * 4;

// Faster
let x: u64 = n >> 1;  // Divide by 2
let y: u64 = n << 2;  // Multiply by 4
```

---

## Avoid Unnecessary Allocations

```lwanga
// Good: Reuse variable
let mut buffer: [u8; 1024] = [0; 1024];
for i in 0..10 {
    process_buffer(&buffer);
}

// Bad: Allocate every iteration
for i in 0..10 {
    let buffer: [u8; 1024] = [0; 1024];
    process_buffer(&buffer);
}
```

---

**Navigation**: [← Previous: Modules](34-modules.md) | [Tutorial Home](README.md) | [Next: Debugging →](36-debugging.md)
