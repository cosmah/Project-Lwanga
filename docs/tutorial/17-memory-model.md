# Lesson 17: Memory Model

**In this lesson, you'll understand how memory works in Lwanga, learn about stack and heap, variable lifetimes, and memory layout.**

---

## Understanding Memory

Memory is where your program stores data while it runs. Understanding how memory works is crucial for:
- Writing efficient code
- Avoiding bugs (use-after-free, stack overflow)
- Understanding pointers
- Low-level programming
- Security research

**Lwanga's memory philosophy:**
- Explicit over implicit (you control memory)
- Stack by default (fast and automatic)
- Manual heap management (when you need it)
- No garbage collector (predictable performance)

---

## Memory Regions

A running program's memory is divided into several regions:

```
High Address (0xFFFFFFFFFFFFFFFF)
┌────────────────────────────────┐
│         Kernel Space           │  Operating system
├────────────────────────────────┤
│          Stack                 │  Local variables, function calls
│            ↓                   │  Grows downward
├────────────────────────────────┤
│                                │
│          (unused)              │
│                                │
├────────────────────────────────┤
│            ↑                   │  Grows upward
│          Heap                  │  Dynamic allocation
├────────────────────────────────┤
│          BSS                   │  Uninitialized global variables
├────────────────────────────────┤
│          Data                  │  Initialized global variables
├────────────────────────────────┤
│          Text                  │  Program code (instructions)
└────────────────────────────────┘
Low Address (0x0000000000000000)
```

### Text Segment (Code)

Contains your compiled program instructions:

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}
// This function's machine code lives in the text segment
```

**Properties:**
- Read-only (can't modify code at runtime)
- Shared (multiple processes can share same code)
- Executable (CPU can run instructions from here)

### Data Segment

Contains initialized global variables and constants:

```lwanga
const GLOBAL_CONST: u64 = 100;  // In data segment

fn main() -> u64 {
    return GLOBAL_CONST;
}
```

**Properties:**
- Read-only for constants
- Read-write for global variables
- Fixed size (determined at compile time)

### BSS Segment

Contains uninitialized global variables (zeroed by OS):

```lwanga
// If Lwanga had global variables, they'd be here
// Currently, Lwanga doesn't support global mutable variables
```

### Heap

Dynamic memory allocation (we'll cover this in advanced lessons):

```lwanga
// Heap allocation (using syscalls)
unsafe {
    let ptr: ptr = syscall(9, 0, 4096, 3, 34, -1, 0) as ptr;  // mmap
    // ptr points to 4096 bytes on the heap
}
```

**Properties:**
- Grows upward (toward higher addresses)
- Manual management (you allocate and free)
- Slower than stack
- Can be large (limited by system memory)

### Stack

Automatic memory for local variables and function calls:

```lwanga
fn main() -> u64 {
    let x: u64 = 42;  // Allocated on stack
    let y: u64 = 10;  // Allocated on stack
    return x + y;
}  // x and y automatically freed
```

**Properties:**
- Grows downward (toward lower addresses)
- Automatic management (push/pop)
- Fast (just moving stack pointer)
- Limited size (typically 1-8 MB)

---

## The Stack

### How the Stack Works

The stack is a Last-In-First-Out (LIFO) data structure:

```lwanga
fn main() -> u64 {
    let a: u64 = 10;   // Push a onto stack
    let b: u64 = 20;   // Push b onto stack
    let c: u64 = 30;   // Push c onto stack
    
    return c;
}  // Pop c, b, a from stack
```

**Stack frame layout:**
```
High Address
┌─────────────────┐
│  Return address │  Where to return after function
├─────────────────┤
│  Saved registers│  Previous function's registers
├─────────────────┤
│  Local var: c   │  30
├─────────────────┤
│  Local var: b   │  20
├─────────────────┤
│  Local var: a   │  10
├─────────────────┤
│  Stack pointer  │  ← Current position
└─────────────────┘
Low Address
```

### Stack Allocation

Variables are allocated on the stack automatically:

```lwanga
fn calculate() -> u64 {
    let x: u64 = 10;      // 8 bytes on stack
    let y: u64 = 20;      // 8 bytes on stack
    let arr: [u8; 100] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0];  // 100 bytes on stack
    
    return x + y;
}  // All stack memory automatically freed
```

**Total stack usage:** 8 + 8 + 100 = 116 bytes

### Function Calls and Stack Frames

Each function call creates a new stack frame:

```lwanga
fn inner(x: u64) -> u64 {
    let y: u64 = x * 2;
    return y;
}

fn outer() -> u64 {
    let a: u64 = 10;
    let b: u64 = inner(a);  // New stack frame created
    return b;
}

fn main() -> u64 {
    return outer();  // New stack frame created
}
```

**Stack during execution:**
```
┌─────────────────┐
│  main's frame   │
├─────────────────┤
│  outer's frame  │  a = 10, b = ?
├─────────────────┤
│  inner's frame  │  x = 10, y = 20  ← Current
└─────────────────┘
```

After `inner` returns:
```
┌─────────────────┐
│  main's frame   │
├─────────────────┤
│  outer's frame  │  a = 10, b = 20  ← Current
└─────────────────┘
```

---

## Variable Lifetime

### Scope-Based Lifetime

Variables live until the end of their scope:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;  // x is born
    
    if x > 5 {
        let y: u64 = 20;  // y is born
        let z: u64 = x + y;
    }  // y and z die here
    
    // x is still alive
    // y and z don't exist anymore
    
    return x;
}  // x dies here
```

**Visualization:**
```
fn main() -> u64 {
    let x: u64 = 10;           // ┐ x lifetime starts
                               // │
    if x > 5 {                 // │
        let y: u64 = 20;       // │ ┐ y lifetime starts
        let z: u64 = x + y;    // │ │ ┐ z lifetime starts
    }                          // │ ┘ ┘ y and z end
                               // │
    return x;                  // │
}                              // ┘ x ends
```

### Nested Scopes

```lwanga
fn main() -> u64 {
    let a: u64 = 1;
    
    {
        let b: u64 = 2;
        
        {
            let c: u64 = 3;
            // a, b, c all accessible
        }  // c freed
        
        // a, b accessible
        // c doesn't exist
    }  // b freed
    
    // Only a accessible
    return a;
}  // a freed
```

---

## Stack vs Heap

### Stack Allocation (Default)

```lwanga
fn main() -> u64 {
    let x: u64 = 42;  // Stack: fast, automatic
    return x;
}
```

**Advantages:**
- ✅ Very fast (just move stack pointer)
- ✅ Automatic cleanup (no manual free)
- ✅ Cache-friendly (sequential access)
- ✅ No fragmentation

**Disadvantages:**
- ❌ Limited size (stack overflow possible)
- ❌ Fixed lifetime (dies at end of scope)
- ❌ Can't return pointers to local variables

### Heap Allocation (Manual)

```lwanga
fn allocate_memory() -> ptr {
    // Allocate 4096 bytes on heap using mmap syscall
    unsafe {
        let ptr: ptr = syscall(
            9,      // mmap
            0,      // addr (let kernel choose)
            4096,   // length
            3,      // prot (PROT_READ | PROT_WRITE)
            34,     // flags (MAP_PRIVATE | MAP_ANONYMOUS)
            -1,     // fd
            0       // offset
        ) as ptr;
        
        return ptr;
    }
}

fn main() -> u64 {
    let heap_ptr: ptr = allocate_memory();
    
    // Use the memory...
    
    // Must manually free (using munmap syscall)
    unsafe {
        syscall(11, heap_ptr, 4096);  // munmap
    }
    
    return 0;
}
```

**Advantages:**
- ✅ Large allocations possible
- ✅ Flexible lifetime (lives until you free it)
- ✅ Can return pointers from functions

**Disadvantages:**
- ❌ Slower (syscall overhead)
- ❌ Manual management (must free)
- ❌ Fragmentation possible
- ❌ Memory leaks if you forget to free

---

## Memory Safety

### Safe: Stack Variables

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let y: u64 = x;  // Copy value
    return y;
}  // Automatic cleanup
```

### Unsafe: Dangling Pointers

```lwanga
fn get_pointer() -> ptr {
    let x: u64 = 42;
    return &x;  // DANGER: x will be freed!
}

fn main() -> u64 {
    let ptr: ptr = get_pointer();
    
    unsafe {
        // DANGER: ptr points to freed memory!
        let value: u64 = *(ptr as ptr);
        return value;
    }
}
```

**Why is this dangerous?**

1. `get_pointer` creates `x` on its stack frame
2. Returns pointer to `x`
3. `get_pointer` returns, stack frame is freed
4. `ptr` now points to invalid memory
5. Dereferencing `ptr` is undefined behavior!

**Fix:** Don't return pointers to local variables. Use heap allocation instead.

---

## Practical Examples

### Example 1: Stack Usage Calculation

```lwanga
fn calculate_stack_usage() -> u64 {
    let a: u64 = 0;           // 8 bytes
    let b: u32 = 0;           // 4 bytes
    let c: u16 = 0;           // 2 bytes
    let d: u8 = 0;            // 1 byte
    let arr: [u64; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];  // 80 bytes
    
    // Total: 8 + 4 + 2 + 1 + 80 = 95 bytes
    // (Plus padding for alignment)
    
    return 95;
}

fn main() -> u64 {
    return calculate_stack_usage();
}
```

### Example 2: Scope Demonstration

```lwanga
fn demonstrate_scope() -> u64 {
    let outer: u64 = 100;
    
    if outer > 50 {
        let inner: u64 = 200;
        let sum: u64 = outer + inner;  // 300
        
        // inner and sum die here
    }
    
    // Only outer is accessible
    return outer;  // Returns 100
}

fn main() -> u64 {
    return demonstrate_scope();
}
```

### Example 3: Function Call Stack

```lwanga
fn level3() -> u64 {
    let c: u64 = 3;
    return c;
}

fn level2() -> u64 {
    let b: u64 = 2;
    let result: u64 = level3();
    return b + result;  // 2 + 3 = 5
}

fn level1() -> u64 {
    let a: u64 = 1;
    let result: u64 = level2();
    return a + result;  // 1 + 5 = 6
}

fn main() -> u64 {
    return level1();  // Returns 6
}
```

**Stack depth during execution:**
```
main → level1 → level2 → level3 (deepest)
```

---

## Common Mistakes

### Mistake 1: Stack Overflow

```lwanga
fn recursive(n: u64) -> u64 {
    if n == 0 {
        return 0;
    }
    return recursive(n - 1);  // Each call uses stack space
}

fn main() -> u64 {
    return recursive(1000000);  // DANGER: stack overflow!
}
```

**Fix:** Use iteration instead of deep recursion:
```lwanga
fn iterative(n: u64) -> u64 {
    let mut i: u64 = 0;
    while i < n {
        i = i + 1;
    }
    return i;
}
```

### Mistake 2: Large Stack Allocations

```lwanga
fn main() -> u64 {
    let huge: [u64; 1000000] = [0, /* ... */];  // 8MB on stack!
    // DANGER: might overflow stack
    return 0;
}
```

**Fix:** Use heap allocation for large data:
```lwanga
fn main() -> u64 {
    // Allocate 8MB on heap
    unsafe {
        let ptr: ptr = syscall(9, 0, 8000000, 3, 34, -1, 0) as ptr;
        // Use ptr...
        syscall(11, ptr, 8000000);  // Free
    }
    return 0;
}
```

---

## Exercises

### Exercise 1: Calculate Memory Usage

Calculate the total stack memory used by this function:

```lwanga
fn memory_usage() -> u64 {
    let a: u64 = 0;
    let b: u64 = 0;
    let c: u32 = 0;
    let d: [u8; 50] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    return 0;
}
```

<details>
<summary>Solution</summary>

```lwanga
// a: 8 bytes (u64)
// b: 8 bytes (u64)
// c: 4 bytes (u32)
// d: 50 bytes ([u8; 50])
// Total: 8 + 8 + 4 + 50 = 70 bytes

fn main() -> u64 {
    return 70;
}
```
</details>

### Exercise 2: Identify Variable Lifetimes

Mark where each variable's lifetime begins and ends:

```lwanga
fn lifetimes() -> u64 {
    let a: u64 = 1;
    
    if a > 0 {
        let b: u64 = 2;
        let c: u64 = 3;
    }
    
    let d: u64 = 4;
    return d;
}
```

<details>
<summary>Solution</summary>

```lwanga
fn lifetimes() -> u64 {
    let a: u64 = 1;        // a: starts
    
    if a > 0 {
        let b: u64 = 2;    // b: starts
        let c: u64 = 3;    // c: starts
    }                      // b, c: end
    
    let d: u64 = 4;        // d: starts
    return d;
}                          // a, d: end
```
</details>

### Exercise 3: Fix Dangling Pointer

Fix this code that returns a pointer to a local variable:

```lwanga
fn get_value() -> ptr {
    let x: u64 = 42;
    return &x;  // WRONG: dangling pointer
}
```

<details>
<summary>Solution</summary>

```lwanga
// Option 1: Return value instead of pointer
fn get_value() -> u64 {
    let x: u64 = 42;
    return x;  // Return value, not pointer
}

// Option 2: Use heap allocation
fn get_value_heap() -> ptr {
    unsafe {
        let ptr: ptr = syscall(9, 0, 8, 3, 34, -1, 0) as ptr;
        *(ptr as ptr) = 42;
        return ptr;  // Caller must free this!
    }
}

fn main() -> u64 {
    let value: u64 = get_value();
    return value;
}
```
</details>

### Exercise 4: Stack Depth

How many stack frames are active when `innermost` is executing?

```lwanga
fn innermost() -> u64 {
    return 42;
}

fn middle() -> u64 {
    return innermost();
}

fn outer() -> u64 {
    return middle();
}

fn main() -> u64 {
    return outer();
}
```

<details>
<summary>Solution</summary>

```lwanga
// When innermost is executing:
// 1. main's frame
// 2. outer's frame
// 3. middle's frame
// 4. innermost's frame
// Total: 4 stack frames

fn main() -> u64 {
    return 4;
}
```
</details>

### Exercise 5: Scope Rules

Which variables are accessible at the marked line?

```lwanga
fn scope_test() -> u64 {
    let a: u64 = 1;
    
    {
        let b: u64 = 2;
        
        {
            let c: u64 = 3;
        }
        
        // HERE: which variables exist?
        let d: u64 = 4;
    }
    
    return a;
}
```

<details>
<summary>Solution</summary>

```lwanga
// At the marked line:
// - a: accessible (outer scope)
// - b: accessible (current scope)
// - c: NOT accessible (inner scope ended)
// - d: NOT accessible (not declared yet)

fn main() -> u64 {
    return 2;  // a and b are accessible
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Memory regions: text, data, BSS, heap, stack
- ✅ How the stack works: LIFO, automatic management
- ✅ Stack frames and function calls
- ✅ Variable lifetime and scope
- ✅ Stack vs heap: trade-offs and when to use each
- ✅ Memory safety: avoiding dangling pointers
- ✅ Common mistakes: stack overflow, large allocations
- ✅ Practical examples and exercises

---

## Next Lesson

Now that you understand memory, let's explore advanced pointer techniques in [Lesson 18: Advanced Pointers](18-pointers-advanced.md).

---

**Navigation**: [← Previous: Constants](16-constants.md) | [Tutorial Home](README.md) | [Next: Advanced Pointers →](18-pointers-advanced.md)
