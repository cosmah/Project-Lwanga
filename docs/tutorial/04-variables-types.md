# Lesson 4: Variables and Data Types

**In this lesson, you'll learn how to store and work with data using variables and understand Lwanga's type system.**

---

## What is a Variable?

A variable is a named storage location in memory that holds a value. Think of it as a labeled box where you can put data.

```lwanga
let age: u64 = 25;
```

This creates a "box" labeled `age` that contains the number `25`.

---

## Declaring Variables

### Basic Variable Declaration

The syntax for declaring a variable:

```lwanga
let variable_name: type = value;
```

**Parts:**
- `let` - Keyword to declare a variable
- `variable_name` - The name you choose
- `:` - Separates name from type
- `type` - The data type (u64, u32, ptr, etc.)
- `=` - Assignment operator
- `value` - The initial value
- `;` - Statement terminator

### Example

```lwanga
fn main() -> u64 {
    let count: u64 = 42;
    let name: ptr = "Alice";
    let is_ready: u64 = 1;  // 1 for true, 0 for false
    
    return 0;
}
```

---

## Immutable vs Mutable Variables

### Immutable Variables (Default)

By default, variables in Lwanga are **immutable** - they cannot be changed after creation:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    x = 20;  // ERROR: cannot assign to immutable variable
    
    return 0;
}
```

**Error message:**
```
error[E0006]: cannot assign to immutable variable 'x'
  --> program.lwanga:3:5
   |
2  |     let x: u64 = 10;
   |         - variable declared here
3  |     x = 20;
   |     ^^^^^^ cannot assign twice to immutable variable
   |
   = help: consider making this variable mutable: `let mut x`
```

**Why immutable by default?**
- Prevents accidental changes
- Makes code easier to reason about
- Catches bugs at compile time
- Encourages better design

### Mutable Variables

Use `mut` keyword to make a variable mutable:

```lwanga
fn main() -> u64 {
    let mut x: u64 = 10;
    x = 20;  // ✓ OK: x is mutable
    x = 30;  // ✓ OK: can change multiple times
    
    return x;  // Returns 30
}
```

**When to use mutable variables:**
- Counters in loops
- Accumulating values
- State that changes over time

**Example: Counter**

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    // Increment counter
    counter = counter + 1;
    counter = counter + 1;
    counter = counter + 1;
    
    return counter;  // Returns 3
}
```

---

## Integer Types

Lwanga has four unsigned integer types. "Unsigned" means they can only hold positive numbers (including zero).

### The Four Integer Types

| Type | Size | Range | Use Case |
|------|------|-------|----------|
| `u8` | 8 bits (1 byte) | 0 to 255 | Small numbers, bytes, characters |
| `u16` | 16 bits (2 bytes) | 0 to 65,535 | Port numbers, small counts |
| `u32` | 32 bits (4 bytes) | 0 to 4,294,967,295 | Large counts, IDs |
| `u64` | 64 bits (8 bytes) | 0 to 18,446,744,073,709,551,615 | Very large numbers, pointers |

### Why Only Unsigned?

**Design choice:** Lwanga only has unsigned integers because:
1. **Simplicity** - Fewer types to learn
2. **Systems programming** - Most low-level operations use unsigned
3. **Bit operations** - Unsigned is clearer for bit manipulation
4. **Explicit** - If you need negative numbers, you handle it explicitly

### Choosing the Right Type

```lwanga
fn main() -> u64 {
    // Use u8 for small values (0-255)
    let age: u8 = 25;
    let byte_value: u8 = 0xFF;
    
    // Use u16 for medium values (0-65535)
    let port: u16 = 8080;
    let count: u16 = 1000;
    
    // Use u32 for large values
    let user_id: u32 = 123456789;
    let file_size: u32 = 1048576;
    
    // Use u64 for very large values or when unsure
    let memory_address: u64 = 0x7fff5fbff000;
    let large_number: u64 = 9999999999;
    
    return 0;
}
```

**Rule of thumb:** When in doubt, use `u64`. It's the most flexible.

---

## Type Safety

Lwanga is **strongly typed** - you cannot mix types without explicit conversion.

### Type Mismatch Error

```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    let y: u64 = x;  // ERROR: type mismatch
    
    return 0;
}
```

**Error message:**
```
error[E0002]: type mismatch
  --> program.lwanga:3:18
   |
3  |     let y: u64 = x;
   |                  ^ expected u64, found u32
```

**Why is this an error?**

Even though both are unsigned integers, they have different sizes. The compiler forces you to be explicit about conversions.

### Type Conversion (Casting)

To convert between types, use explicit casting:

```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    let y: u64 = x as u64;  // ✓ Explicit conversion
    
    return y;
}
```

**Casting syntax:**
```lwanga
value as target_type
```

**Examples:**

```lwanga
fn main() -> u64 {
    // Widening conversions (safe)
    let a: u8 = 100;
    let b: u16 = a as u16;  // 100 fits in u16
    let c: u32 = b as u32;  // 100 fits in u32
    let d: u64 = c as u64;  // 100 fits in u64
    
    // Narrowing conversions (can lose data)
    let x: u64 = 300;
    let y: u8 = x as u8;    // y = 44 (300 % 256)
    
    return 0;
}
```

**Warning:** Narrowing conversions can lose data! The value wraps around.

---

## The Pointer Type

The `ptr` type holds memory addresses. It's used for strings and references to data.

### String Pointers

```lwanga
fn main() -> u64 {
    let message: ptr = "Hello, World!";
    // message holds the memory address where "Hello, World!" is stored
    
    return 0;
}
```

### Why Use Pointers for Strings?

**Design choice:** Strings in Lwanga are just sequences of bytes in memory. A `ptr` points to the first byte.

This is:
- **Simple** - No complex string type
- **Efficient** - No copying or allocation
- **Explicit** - You know exactly what you're working with

### Pointer to Variable

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr_to_x: ptr = &x;  // Get address of x
    
    return 0;
}
```

The `&` operator gets the address of a variable.

---

## Variable Scope

Variables are only visible within the block where they're declared.

### Function Scope

```lwanga
fn main() -> u64 {
    let x: u64 = 10;  // x is visible in main
    
    return x;  // ✓ OK
}

fn other_function() -> u64 {
    return x;  // ERROR: x is not visible here
}
```

### Block Scope

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        let y: u64 = 20;  // y only exists in this block
        return y;  // ✓ OK
    }
    
    return y;  // ERROR: y doesn't exist here
}
```

### Shadowing

You can declare a new variable with the same name in a nested scope:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        let x: u64 = 20;  // This is a different x
        // Inner x = 20
    }
    
    // Outer x = 10
    return x;  // Returns 10
}
```

**Why allow shadowing?**
- Reuse good variable names
- Each scope has its own version
- No confusion - inner scope takes precedence

---

## Constants

Constants are like variables but:
- Must be known at compile time
- Cannot be changed (ever)
- Use `const` keyword
- Use SCREAMING_SNAKE_CASE naming

### Declaring Constants

```lwanga
const MAX_SIZE: u64 = 1024;
const PORT: u16 = 8080;
const BUFFER_LEN: u32 = 512;

fn main() -> u64 {
    let size: u64 = MAX_SIZE;
    return size;
}
```

### Constants vs Variables

```lwanga
// Constant - known at compile time
const MAX: u64 = 100;

fn main() -> u64 {
    // Variable - can be computed at runtime
    let x: u64 = 10;
    let y: u64 = x * 2;  // Computed at runtime
    
    return y;
}
```

**When to use constants:**
- Configuration values
- Magic numbers
- Syscall numbers
- File descriptors

**Example:**

```lwanga
const STDOUT: u64 = 1;
const STDERR: u64 = 2;
const SYS_WRITE: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Error!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDERR, msg, 7);
    }
    
    return 0;
}
```

---

## Type Inference (Not Available)

**Important:** Lwanga does NOT have type inference. You must always specify types explicitly.

```lwanga
// This does NOT work in Lwanga
let x = 42;  // ERROR: type annotation required

// You must write
let x: u64 = 42;  // ✓ Correct
```

**Why no type inference?**
- **Explicit is better** - You always know the type
- **Prevents mistakes** - No surprises about types
- **Simpler compiler** - Easier to implement and maintain
- **Better for systems programming** - Type size matters

---

## Practical Examples

### Example 1: Counter

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    // Count to 10
    while counter < 10 {
        counter = counter + 1;
    }
    
    return counter;  // Returns 10
}
```

### Example 2: Multiple Variables

```lwanga
fn main() -> u64 {
    let width: u64 = 80;
    let height: u64 = 24;
    let area: u64 = width * height;
    
    return area;  // Returns 1920
}
```

### Example 3: Type Conversion

```lwanga
fn main() -> u64 {
    // Port number (u16)
    let port: u16 = 8080;
    
    // Convert to u64 for syscall
    let port_u64: u64 = port as u64;
    
    return port_u64;
}
```

### Example 4: Using Constants

```lwanga
const BUFFER_SIZE: u64 = 1024;
const MAX_CONNECTIONS: u32 = 100;

fn main() -> u64 {
    let buffer_len: u64 = BUFFER_SIZE;
    let max_conn: u32 = MAX_CONNECTIONS;
    
    return buffer_len;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting Type Annotation

```lwanga
let x = 42;  // ERROR
```

**Fix:**
```lwanga
let x: u64 = 42;  // ✓
```

### Mistake 2: Modifying Immutable Variable

```lwanga
let x: u64 = 10;
x = 20;  // ERROR
```

**Fix:**
```lwanga
let mut x: u64 = 10;
x = 20;  // ✓
```

### Mistake 3: Type Mismatch

```lwanga
let x: u32 = 42;
let y: u64 = x;  // ERROR
```

**Fix:**
```lwanga
let x: u32 = 42;
let y: u64 = x as u64;  // ✓
```

### Mistake 4: Using Variable Out of Scope

```lwanga
if x > 5 {
    let y: u64 = 10;
}
return y;  // ERROR: y not in scope
```

**Fix:**
```lwanga
let y: u64 = 0;
if x > 5 {
    y = 10;  // Note: y must be mut
}
return y;  // ✓
```

---

## Exercises

### Exercise 1: Declare Variables

Declare variables for:
- Your age (use appropriate type)
- Your name (as a string)
- A counter starting at 0

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let age: u8 = 25;
    let name: ptr = "Alice";
    let mut counter: u64 = 0;
    
    return 0;
}
```
</details>

### Exercise 2: Mutable Counter

Create a mutable counter, increment it 5 times, and return its value.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    counter = counter + 1;
    counter = counter + 1;
    counter = counter + 1;
    counter = counter + 1;
    counter = counter + 1;
    
    return counter;  // Returns 5
}
```
</details>

### Exercise 3: Type Conversion

Convert a u8 value to u64 and return it.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let small: u8 = 42;
    let large: u64 = small as u64;
    
    return large;  // Returns 42
}
```
</details>

### Exercise 4: Calculate Area

Calculate the area of a rectangle (width × height) and return it.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let width: u64 = 10;
    let height: u64 = 20;
    let area: u64 = width * height;
    
    return area;  // Returns 200
}
```
</details>

### Exercise 5: Use Constants

Define constants for STDOUT (1) and a message, then use them.

<details>
<summary>Solution</summary>

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 7);
    }
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to declare variables with `let`
- ✅ The difference between immutable and mutable variables
- ✅ All four integer types (u8, u16, u32, u64)
- ✅ How to choose the right type
- ✅ Type safety and explicit casting
- ✅ The pointer type for strings and addresses
- ✅ Variable scope and shadowing
- ✅ Constants and when to use them
- ✅ Why Lwanga requires explicit types

---

## Next Lesson

Now that you understand variables and types, let's learn how to work with numbers and perform calculations in [Lesson 5: Working with Numbers](05-numbers.md).

---

**Navigation**: [← Previous: Basic Syntax](03-basic-syntax.md) | [Tutorial Home](README.md) | [Next: Working with Numbers →](05-numbers.md)
