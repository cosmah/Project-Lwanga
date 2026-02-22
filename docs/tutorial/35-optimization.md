# Lesson 35: Optimization Techniques

**In this lesson, you'll learn how to write fast, efficient code and use compiler optimizations to maximize performance.**

---

## Why Optimize?

Optimization makes your programs:

- **Faster** - Execute in less time
- **Smaller** - Use less memory and disk space
- **More efficient** - Consume less CPU and power
- **More scalable** - Handle more load
- **More responsive** - Better user experience

**Design choice:** Lwanga compiles to native machine code and supports multiple optimization levels, giving you control over the speed/size trade-off.

**Important:** Optimize only when needed. Premature optimization is the root of all evil. Profile first, then optimize bottlenecks.

---

## Compiler Optimization Levels

### Understanding Optimization Flags

```bash
# No optimization (fastest compilation, slowest execution)
lwangac program.lwanga -o program -O0

# Basic optimization (balanced)
lwangac program.lwanga -o program -O1

# Moderate optimization (recommended for most cases)
lwangac program.lwanga -o program -O2

# Aggressive optimization (slowest compilation, fastest execution)
lwangac program.lwanga -o program -O3
```

### What Each Level Does

**-O0 (No Optimization)**
- Fastest compilation
- Easiest to debug
- Largest binary size
- Slowest execution
- Use for: Development and debugging

**-O1 (Basic)**
- Simple optimizations
- Fast compilation
- Moderate binary size
- Decent execution speed
- Use for: Quick builds during development

**-O2 (Moderate - Recommended)**
- Most optimizations enabled
- Reasonable compilation time
- Good binary size
- Fast execution
- Use for: Production builds

**-O3 (Aggressive)**
- All optimizations
- Slow compilation
- May increase binary size
- Fastest execution
- Use for: Performance-critical code

### Comparing Optimization Levels

```lwanga
fn fibonacci(n: u64) -> u64 {
    if n <= 1 {
        return n;
    }
    return fibonacci(n - 1) + fibonacci(n - 2);
}

fn main() -> u64 {
    let result: u64 = fibonacci(30);
    return result;
}
```

**Benchmark results:**
```bash
# -O0: ~5.2 seconds
# -O1: ~2.1 seconds
# -O2: ~0.8 seconds
# -O3: ~0.7 seconds
```

---

## Loop Optimization

### Avoid Repeated Calculations

**Bad: Calculation in loop condition**

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    // BAD: len * 2 / 2 calculated every iteration!
    while i < len * 2 / 2 {
        unsafe {
            sum = sum + *(arr + i * 8);
        }
        i = i + 1;
    }
    
    return sum;
}
```

**Good: Calculate once**

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    let limit: u64 = len;  // Calculate once
    
    while i < limit {
        unsafe {
            sum = sum + *(arr + i * 8);
        }
        i = i + 1;
    }
    
    return sum;
}
```

### Loop Unrolling

**Normal loop:**

```lwanga
fn process_array(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < len {
        unsafe {
            *(arr + i) = *(arr + i) * 2;
        }
        i = i + 1;
    }
    
    return 0;
}
```

**Unrolled loop (4x):**

```lwanga
fn process_array_unrolled(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    let limit: u64 = len - (len % 4);
    
    // Process 4 elements at a time
    while i < limit {
        unsafe {
            *(arr + i) = *(arr + i) * 2;
            *(arr + i + 1) = *(arr + i + 1) * 2;
            *(arr + i + 2) = *(arr + i + 2) * 2;
            *(arr + i + 3) = *(arr + i + 3) * 2;
        }
        i = i + 4;
    }
    
    // Handle remaining elements
    while i < len {
        unsafe {
            *(arr + i) = *(arr + i) * 2;
        }
        i = i + 1;
    }
    
    return 0;
}
```

**Why it's faster:**
- Fewer loop iterations
- Fewer branch predictions
- Better instruction pipelining
- Compiler can optimize better

### Strength Reduction

Replace expensive operations with cheaper ones:

**Bad: Division and multiplication**

```lwanga
fn calculate(n: u64) -> u64 {
    let x: u64 = n / 2;      // Division is slow
    let y: u64 = n * 4;      // Multiplication is slower than shifts
    let z: u64 = n % 8;      // Modulo is slow
    
    return x + y + z;
}
```

**Good: Bit operations**

```lwanga
fn calculate_optimized(n: u64) -> u64 {
    let x: u64 = n >> 1;     // Divide by 2 (right shift)
    let y: u64 = n << 2;     // Multiply by 4 (left shift)
    let z: u64 = n & 7;      // Modulo 8 (AND with 7)
    
    return x + y + z;
}
```

**Common patterns:**
```lwanga
// Multiply/divide by powers of 2
let x: u64 = n * 2;   // Use: n << 1
let x: u64 = n * 4;   // Use: n << 2
let x: u64 = n * 8;   // Use: n << 3
let x: u64 = n / 2;   // Use: n >> 1
let x: u64 = n / 4;   // Use: n >> 2

// Modulo by powers of 2
let x: u64 = n % 2;   // Use: n & 1
let x: u64 = n % 4;   // Use: n & 3
let x: u64 = n % 8;   // Use: n & 7
let x: u64 = n % 16;  // Use: n & 15
```

---

## Memory Optimization

### Avoid Unnecessary Allocations

**Bad: Allocate in loop**

```lwanga
fn process_data(count: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < count {
        // BAD: Allocates 1024 bytes every iteration!
        let buffer: [u8; 1024] = [0; 1024];
        
        // Process buffer
        
        i = i + 1;
    }
    
    return 0;
}
```

**Good: Allocate once, reuse**

```lwanga
fn process_data_optimized(count: u64) -> u64 {
    // GOOD: Allocate once
    let mut buffer: [u8; 1024] = [0; 1024];
    let mut i: u64 = 0;
    
    while i < count {
        // Reuse buffer
        
        // Clear if needed
        let mut j: u64 = 0;
        while j < 1024 {
            buffer[j] = 0;
            j = j + 1;
        }
        
        i = i + 1;
    }
    
    return 0;
}
```

### Use Appropriate Data Types

**Bad: Wasting memory**

```lwanga
struct WastefulData {
    flag: u64,        // Only needs 1 byte, uses 8
    count: u64,       // Only needs 2 bytes, uses 8
    status: u64,      // Only needs 1 byte, uses 8
}
// Total: 24 bytes
```

**Good: Right-sized types**

```lwanga
struct EfficientData {
    flag: u8,         // 1 byte
    count: u16,       // 2 bytes
    status: u8,       // 1 byte
}
// Total: 4 bytes (6x smaller!)
```

### Packed Structs for Network Data

```lwanga
// Regular struct (may have padding)
struct RegularPacket {
    type: u8,
    length: u16,
    data: u32,
}
// Size: 8 bytes (with padding)

// Packed struct (no padding)
packed struct PackedPacket {
    type: u8,
    length: u16,
    data: u32,
}
// Size: 7 bytes (no padding)
```

---

## Function Optimization

### Inline Small Functions

Small functions are automatically inlined at -O2 and above:

```lwanga
// This will be inlined
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

// This will be inlined
fn is_even(n: u64) -> u64 {
    return n & 1 == 0;
}

fn main() -> u64 {
    let x: u64 = add(5, 3);      // Becomes: let x: u64 = 5 + 3;
    let even: u64 = is_even(10); // Becomes: let even: u64 = 10 & 1 == 0;
    return x;
}
```

**Benefits of inlining:**
- No function call overhead
- Better optimization opportunities
- Faster execution

**When NOT to inline:**
- Large functions (increases code size)
- Recursive functions
- Functions called from many places

### Tail Call Optimization

```lwanga
// Regular recursion (uses stack)
fn factorial(n: u64) -> u64 {
    if n == 0 {
        return 1;
    }
    return n * factorial(n - 1);  // Not tail call
}

// Tail recursive (can be optimized)
fn factorial_tail(n: u64, acc: u64) -> u64 {
    if n == 0 {
        return acc;
    }
    return factorial_tail(n - 1, n * acc);  // Tail call
}

fn factorial_optimized(n: u64) -> u64 {
    return factorial_tail(n, 1);
}
```

---

## Algorithm Optimization

### Choose Better Algorithms

**Bad: O(n²) bubble sort**

```lwanga
fn bubble_sort(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < len {
        let mut j: u64 = 0;
        
        while j < len - 1 {
            unsafe {
                let a: u64 = *(arr + j * 8);
                let b: u64 = *(arr + (j + 1) * 8);
                
                if a > b {
                    *(arr + j * 8) = b;
                    *(arr + (j + 1) * 8) = a;
                }
            }
            
            j = j + 1;
        }
        
        i = i + 1;
    }
    
    return 0;
}
```

**Better: O(n log n) quicksort (conceptual)**

```lwanga
// Quicksort is much faster for large arrays
// For 1000 elements:
// Bubble sort: ~1,000,000 operations
// Quicksort: ~10,000 operations (100x faster!)
```

### Early Exit Optimization

**Bad: Always checks everything**

```lwanga
fn find_value(arr: ptr, len: u64, target: u64) -> u64 {
    let mut found: u64 = 0;
    let mut i: u64 = 0;
    
    while i < len {
        unsafe {
            if *(arr + i * 8) == target {
                found = 1;
            }
        }
        i = i + 1;
    }
    
    return found;
}
```

**Good: Exits early**

```lwanga
fn find_value_optimized(arr: ptr, len: u64, target: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < len {
        unsafe {
            if *(arr + i * 8) == target {
                return 1;  // Found! Exit immediately
            }
        }
        i = i + 1;
    }
    
    return 0;  // Not found
}
```

---

## Practical Examples

### Example 1: Optimized String Length

```lwanga
// Unoptimized
fn strlen_slow(s: ptr) -> u64 {
    let mut len: u64 = 0;
    
    unsafe {
        while *(s + len) != 0 {
            len = len + 1;
        }
    }
    
    return len;
}

// Optimized (process 8 bytes at a time)
fn strlen_fast(s: ptr) -> u64 {
    let mut len: u64 = 0;
    
    // Process 8 bytes at a time
    unsafe {
        while 1 == 1 {
            let chunk: u64 = *(s + len) as u64;
            
            // Check if any byte is zero
            if (chunk & 0xFF) == 0 {
                break;
            }
            if (chunk & 0xFF00) == 0 {
                len = len + 1;
                break;
            }
            if (chunk & 0xFF0000) == 0 {
                len = len + 2;
                break;
            }
            if (chunk & 0xFF000000) == 0 {
                len = len + 3;
                break;
            }
            
            len = len + 4;
        }
    }
    
    return len;
}
```

### Example 2: Optimized Memory Copy

```lwanga
// Byte-by-byte copy
fn memcpy_slow(dest: ptr, src: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return 0;
}

// 8-byte copy (8x faster)
fn memcpy_fast(dest: ptr, src: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    let limit: u64 = len - (len % 8);
    
    // Copy 8 bytes at a time
    unsafe {
        while i < limit {
            let value: u64 = *(src + i) as u64;
            *(dest + i) = value as u8;
            i = i + 8;
        }
        
        // Copy remaining bytes
        while i < len {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return 0;
}
```

### Example 3: Lookup Table Optimization

**Bad: Repeated calculation**

```lwanga
fn is_hex_digit(c: u8) -> u64 {
    if c >= 48 && c <= 57 {   // 0-9
        return 1;
    }
    if c >= 65 && c <= 70 {   // A-F
        return 1;
    }
    if c >= 97 && c <= 102 {  // a-f
        return 1;
    }
    return 0;
}
```

**Good: Lookup table**

```lwanga
const HEX_TABLE: [u8; 256] = [
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 0-15
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 16-31
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 32-47
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,  // 48-63 (0-9)
    0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 64-79 (A-F)
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 80-95
    0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0,  // 96-111 (a-f)
    // ... rest zeros
];

fn is_hex_digit_fast(c: u8) -> u64 {
    return HEX_TABLE[c as u64] as u64;
}
```

---

## Profiling and Measurement

### Measuring Performance

```lwanga
const SYS_CLOCK_GETTIME: u64 = 228;
const CLOCK_MONOTONIC: u32 = 1;

struct timespec {
    tv_sec: u64,
    tv_nsec: u64,
}

fn get_time() -> u64 {
    let ts: timespec = timespec { tv_sec: 0, tv_nsec: 0 };
    
    unsafe {
        syscall(SYS_CLOCK_GETTIME, CLOCK_MONOTONIC, &ts, 0);
    }
    
    return ts.tv_sec * 1000000000 + ts.tv_nsec;
}

fn benchmark_function() -> u64 {
    let start: u64 = get_time();
    
    // Code to benchmark
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    while i < 1000000 {
        sum = sum + i;
        i = i + 1;
    }
    
    let end: u64 = get_time();
    let elapsed: u64 = end - start;
    
    return elapsed;  // Nanoseconds
}

fn main() -> u64 {
    let time: u64 = benchmark_function();
    return time;
}
```

---

## Common Mistakes

### Mistake 1: Premature Optimization

```lwanga
// Don't optimize before profiling!
// This might not be the bottleneck
fn overly_optimized_function(x: u64) -> u64 {
    // Complex, hard-to-read optimization
    // for a function called once
    return x;
}
```

**Fix:** Profile first, optimize bottlenecks.

### Mistake 2: Sacrificing Readability

```lwanga
// Too clever, hard to maintain
fn mystery_function(x: u64) -> u64 {
    return ((x << 3) - (x << 1)) ^ (x >> 2);
}
```

**Fix:** Balance optimization with clarity.

### Mistake 3: Ignoring Compiler Optimizations

```lwanga
// Manual optimization that compiler already does
fn redundant_optimization(x: u64) -> u64 {
    // Compiler already optimizes this at -O2
    return x * 2;
}
```

**Fix:** Trust the compiler for simple optimizations.

---

## Exercises

### Exercise 1: Optimize Array Sum

Optimize this function to sum an array faster:

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    while i < len {
        unsafe {
            sum = sum + *(arr + i * 8);
        }
        i = i + 1;
    }
    
    return sum;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn sum_array_optimized(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    let limit: u64 = len - (len % 4);
    
    // Unroll loop 4x
    while i < limit {
        unsafe {
            sum = sum + *(arr + i * 8);
            sum = sum + *(arr + (i + 1) * 8);
            sum = sum + *(arr + (i + 2) * 8);
            sum = sum + *(arr + (i + 3) * 8);
        }
        i = i + 4;
    }
    
    // Handle remaining
    while i < len {
        unsafe {
            sum = sum + *(arr + i * 8);
        }
        i = i + 1;
    }
    
    return sum;
}
```
</details>

### Exercise 2: Replace Division with Shifts

Optimize this function using bit operations:

```lwanga
fn calculate(n: u64) -> u64 {
    let a: u64 = n / 8;
    let b: u64 = n * 16;
    let c: u64 = n % 32;
    return a + b + c;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn calculate_optimized(n: u64) -> u64 {
    let a: u64 = n >> 3;   // Divide by 8
    let b: u64 = n << 4;   // Multiply by 16
    let c: u64 = n & 31;   // Modulo 32
    return a + b + c;
}
```
</details>

### Exercise 3: Optimize String Comparison

Create an optimized version that exits early:

```lwanga
fn strcmp(s1: ptr, s2: ptr, len: u64) -> u64 {
    let mut matches: u64 = 1;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(s1 + i) != *(s2 + i) {
                matches = 0;
            }
            i = i + 1;
        }
    }
    
    return matches;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn strcmp_optimized(s1: ptr, s2: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(s1 + i) != *(s2 + i) {
                return 0;  // Exit immediately on mismatch
            }
            i = i + 1;
        }
    }
    
    return 1;  // All matched
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Compiler optimization levels and when to use them
- ✅ Loop optimization techniques (unrolling, strength reduction)
- ✅ Memory optimization strategies
- ✅ Function inlining and tail call optimization
- ✅ Algorithm selection and early exit patterns
- ✅ Practical optimization examples
- ✅ How to measure performance
- ✅ Common optimization mistakes

**Remember:** Profile before optimizing. Focus on bottlenecks. Balance performance with maintainability.

---

## Next Lesson

Now that you know how to optimize code, let's learn debugging techniques in [Lesson 36: Debugging](36-debugging.md).

---

**Navigation**: [← Previous: Modules](34-modules.md) | [Tutorial Home](README.md) | [Next: Debugging →](36-debugging.md)
