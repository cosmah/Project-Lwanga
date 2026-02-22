# Lesson 15: Type System Deep Dive

**In this lesson, you'll master Lwanga's type system, understand type safety, learn type conversions, and see how the type system prevents bugs.**

---

## What is a Type System?

A type system is a set of rules that assigns types to values and enforces how they can be used.

**Why have a type system?**
- **Catch bugs early** - Type errors found at compile time, not runtime
- **Self-documenting** - Types explain what data means
- **Performance** - Compiler knows exact sizes and layouts
- **Safety** - Prevents common vulnerabilities (buffer overflows, type confusion)
- **Tooling** - Enables better IDE support and refactoring

**Lwanga's type philosophy:**
- Strong typing (no implicit conversions)
- Static typing (types checked at compile time)
- Explicit over implicit (you always know what's happening)

---

## Lwanga's Type System

### Primitive Types

```lwanga
// Unsigned integers
u8   // 0 to 255
u16  // 0 to 65,535
u32  // 0 to 4,294,967,295
u64  // 0 to 18,446,744,073,709,551,615

// Signed integers
i8   // -128 to 127
i16  // -32,768 to 32,767
i32  // -2,147,483,648 to 2,147,483,647
i64  // -9,223,372,036,854,775,808 to 9,223,372,036,854,775,807

// Pointer
ptr  // Memory address (64-bit on x86_64)
```

**Design choice:** Lwanga uses explicit sizes (u8, u16, u32, u64) instead of ambiguous types like `int`. This makes code portable and clear.

### Composite Types

```lwanga
// Arrays
[u8; 10]      // Array of 10 bytes
[u64; 100]    // Array of 100 u64s

// Structs
struct Point {
    x: u64,
    y: u64,
}

// Packed structs
packed struct Header {
    magic: u32,
    version: u16,
}
```

---

## Type Safety

### No Implicit Conversions

Lwanga is **strongly typed** - types don't automatically convert:

```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    let y: u64 = x;  // ERROR: type mismatch
    
    return 0;
}
```

**Error message:**
```
error[E0010]: type mismatch
  --> program.lwanga:3:18
   |
3  |     let y: u64 = x;
   |                  ^ expected u64, found u32
   |
   = help: use 'as u64' to cast
```

**Why no implicit conversion?**

**Design choice:** Implicit conversions hide bugs:
```c
// C code - implicit conversion
unsigned char x = 255;
int y = x + 1;  // y = 256, but what if you expected 0?
```

In Lwanga, you must be explicit:
```lwanga
let x: u8 = 255;
let y: u64 = (x as u64) + 1;  // Clear: converting to u64 first
```

### Type Checking at Compile Time

All type errors are caught before your program runs:

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let x: u32 = 10;
    let result: u64 = add(x, 20);  // ERROR: x is u32, not u64
    return result;
}
```

**Benefit:** No runtime type errors! If it compiles, types are correct.

---

## Type Casting

### Explicit Casting with `as`

Use the `as` keyword to convert between types:

```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    let y: u64 = x as u64;  // Explicit cast
    
    return y;  // Returns 42
}
```

**Syntax:**
```lwanga
value as target_type
```

### Widening Conversions (Safe)

Converting to a larger type is always safe:

```lwanga
fn main() -> u64 {
    let a: u8 = 100;
    let b: u16 = a as u16;  // 100 fits in u16
    let c: u32 = b as u32;  // 100 fits in u32
    let d: u64 = c as u64;  // 100 fits in u64
    
    return d;  // Returns 100
}
```

**Why safe?**

The target type can hold all values of the source type:
- u8 (0-255) → u16 (0-65535) ✓
- u16 → u32 ✓
- u32 → u64 ✓

No data loss possible!

### Narrowing Conversions (Potentially Unsafe)

Converting to a smaller type can lose data:

```lwanga
fn main() -> u64 {
    let x: u64 = 300;
    let y: u8 = x as u8;  // Truncates to 44
    
    return y as u64;  // Returns 44, not 300!
}
```

**What happened?**

300 in binary: `0000000100101100`
u8 can only hold 8 bits: `00101100` = 44

**Formula:** `result = value % 256` (for u8)

**Examples:**
```lwanga
fn main() -> u64 {
    let a: u64 = 256;
    let b: u8 = a as u8;  // 256 % 256 = 0
    
    let c: u64 = 257;
    let d: u8 = c as u8;  // 257 % 256 = 1
    
    let e: u64 = 1000;
    let f: u8 = e as u8;  // 1000 % 256 = 232
    
    return f as u64;  // Returns 232
}
```

**When to use narrowing:**
- You know the value fits
- You want truncation behavior
- You're working with bit manipulation

**How to be safe:**
```lwanga
fn safe_narrow(x: u64) -> u8 {
    if x > 255 {
        return 255;  // Clamp to maximum
    }
    return x as u8;
}
```

### Signed to Unsigned Conversion

```lwanga
fn main() -> u64 {
    let x: i32 = -1;
    let y: u32 = x as u32;  // Reinterprets bits
    
    return y as u64;  // Returns 4294967295 (2^32 - 1)
}
```

**What happened?**

-1 in two's complement: `11111111111111111111111111111111`
Interpreted as unsigned: 4,294,967,295

**Be careful with signed/unsigned conversions!**

### Pointer to Integer Conversion

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;
    let addr: u64 = ptr as u64;  // Get memory address
    
    return addr;  // Returns the address of x
}
```

**Use cases:**
- Debugging (print addresses)
- Low-level memory manipulation
- Pointer arithmetic

### Integer to Pointer Conversion

```lwanga
fn main() -> u64 {
    let addr: u64 = 0x1000;
    let ptr: ptr = addr as ptr;  // Create pointer from address
    
    // Use with caution - may be invalid!
    unsafe {
        // let value: u64 = *ptr;  // Dangerous!
    }
    
    return addr;
}
```

**Warning:** Creating pointers from arbitrary addresses is dangerous! Only do this when you know the address is valid.

---

## Type Inference

### Explicit Types (Recommended)

```lwanga
fn main() -> u64 {
    let x: u64 = 42;  // Explicit type
    let y: u32 = 10;  // Explicit type
    
    return x;
}
```

**Why explicit?**
- Code is self-documenting
- No ambiguity
- Easier to understand
- Prevents mistakes

### Type Inference from Context

In some cases, Lwanga can infer types:

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let result = add(10, 20);  // Inferred as u64 from add's return type
    return result;
}
```

**Design choice:** Lwanga prefers explicit types for clarity, but allows inference when the type is obvious from context.

---

## Type Compatibility

### Exact Type Matching

Function parameters must match exactly:

```lwanga
fn process(x: u32) -> u64 {
    return x as u64;
}

fn main() -> u64 {
    let a: u32 = 10;
    let b: u64 = 20;
    
    let r1: u64 = process(a);  // OK: a is u32
    let r2: u64 = process(b);  // ERROR: b is u64, not u32
    
    return r1;
}
```

**Fix:**
```lwanga
let r2: u64 = process(b as u32);  // Explicit cast
```

### Struct Type Compatibility

Structs with the same fields but different names are NOT compatible:

```lwanga
struct Point1 {
    x: u64,
    y: u64,
}

struct Point2 {
    x: u64,
    y: u64,
}

fn main() -> u64 {
    let p1: Point1 = Point1 { x: 10, y: 20 };
    let p2: Point2 = p1;  // ERROR: different types!
    
    return 0;
}
```

**Why?**

**Design choice:** Structural typing (matching by structure) can be confusing. Nominal typing (matching by name) is clearer and safer.

---

## Practical Examples

### Example 1: Safe Integer Conversion

```lwanga
fn safe_u64_to_u8(value: u64) -> u8 {
    if value > 255 {
        return 255;  // Clamp to max
    }
    return value as u8;
}

fn main() -> u64 {
    let big: u64 = 1000;
    let small: u8 = safe_u64_to_u8(big);
    
    return small as u64;  // Returns 255
}
```

### Example 2: Pointer Arithmetic

```lwanga
fn get_array_element(arr: ptr, index: u64) -> u64 {
    // Calculate address: base + (index * element_size)
    let base: u64 = arr as u64;
    let offset: u64 = index * 8;  // 8 bytes per u64
    let element_addr: ptr = (base + offset) as ptr;
    
    unsafe {
        return *(element_addr as ptr);
    }
}

fn main() -> u64 {
    let numbers: [u64; 5] = [10, 20, 30, 40, 50];
    let ptr: ptr = &numbers;
    
    let value: u64 = get_array_element(ptr, 2);
    return value;  // Returns 30
}
```

### Example 3: Type-Safe Flags

```lwanga
const FLAG_READ: u32 = 0x01;
const FLAG_WRITE: u32 = 0x02;
const FLAG_EXECUTE: u32 = 0x04;

fn check_permission(flags: u32, required: u32) -> u64 {
    if (flags & required) == required {
        return 1;  // Has permission
    }
    return 0;  // No permission
}

fn main() -> u64 {
    let perms: u32 = FLAG_READ | FLAG_WRITE;
    
    let can_read: u64 = check_permission(perms, FLAG_READ);
    let can_exec: u64 = check_permission(perms, FLAG_EXECUTE);
    
    return can_read;  // Returns 1
}
```

### Example 4: Network Byte Order

```lwanga
fn htons(value: u16) -> u16 {
    // Host to network short (swap bytes on little-endian)
    let low: u8 = (value & 0xFF) as u8;
    let high: u8 = ((value >> 8) & 0xFF) as u8;
    
    return ((low as u16) << 8) | (high as u16);
}

fn main() -> u64 {
    let port: u16 = 8080;
    let network_port: u16 = htons(port);
    
    return network_port as u64;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting to Cast

```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    return x;  // ERROR: expected u64, found u32
}
```

**Fix:**
```lwanga
fn main() -> u64 {
    let x: u32 = 42;
    return x as u64;  // ✓ Correct
}
```

### Mistake 2: Assuming Implicit Conversion

```lwanga
fn process(x: u64) -> u64 {
    return x * 2;
}

fn main() -> u64 {
    let y: u32 = 10;
    return process(y);  // ERROR: y is u32, not u64
}
```

**Fix:**
```lwanga
return process(y as u64);  // ✓ Correct
```

### Mistake 3: Unsafe Narrowing

```lwanga
fn main() -> u64 {
    let x: u64 = 1000;
    let y: u8 = x as u8;  // Silently truncates to 232!
    
    return y as u64;  // Returns 232, not 1000
}
```

**Fix:** Check before casting:
```lwanga
fn main() -> u64 {
    let x: u64 = 1000;
    
    if x > 255 {
        return 0;  // Error indicator
    }
    
    let y: u8 = x as u8;
    return y as u64;
}
```

---

## Exercises

### Exercise 1: Temperature Conversion

Write a function that converts Celsius (i32) to Fahrenheit (i32). Formula: F = C * 9 / 5 + 32

<details>
<summary>Solution</summary>

```lwanga
fn celsius_to_fahrenheit(celsius: i32) -> i32 {
    let f: i32 = celsius * 9 / 5 + 32;
    return f;
}

fn main() -> u64 {
    let temp_c: i32 = 25;
    let temp_f: i32 = celsius_to_fahrenheit(temp_c);
    
    return temp_f as u64;  // Returns 77
}
```
</details>

### Exercise 2: Safe Division

Write a function that safely divides two u64 numbers, returning 0 if divisor is 0.

<details>
<summary>Solution</summary>

```lwanga
fn safe_divide(dividend: u64, divisor: u64) -> u64 {
    if divisor == 0 {
        return 0;  // Avoid division by zero
    }
    return dividend / divisor;
}

fn main() -> u64 {
    let result1: u64 = safe_divide(100, 5);  // 20
    let result2: u64 = safe_divide(100, 0);  // 0
    
    return result1;
}
```
</details>

### Exercise 3: Byte Swap

Write a function that swaps the bytes of a u16 value.

<details>
<summary>Solution</summary>

```lwanga
fn swap_bytes(value: u16) -> u16 {
    let low: u8 = (value & 0xFF) as u8;
    let high: u8 = ((value >> 8) & 0xFF) as u8;
    
    return ((low as u16) << 8) | (high as u16);
}

fn main() -> u64 {
    let x: u16 = 0x1234;
    let swapped: u16 = swap_bytes(x);  // 0x3412
    
    return swapped as u64;
}
```
</details>

### Exercise 4: Range Check

Write a function that checks if a u64 value fits in a u8 (0-255).

<details>
<summary>Solution</summary>

```lwanga
fn fits_in_u8(value: u64) -> u64 {
    if value <= 255 {
        return 1;  // True
    }
    return 0;  // False
}

fn main() -> u64 {
    let test1: u64 = fits_in_u8(100);   // 1
    let test2: u64 = fits_in_u8(1000);  // 0
    
    return test1;
}
```
</details>

### Exercise 5: Pointer Distance

Write a function that calculates the distance between two pointers in bytes.

<details>
<summary>Solution</summary>

```lwanga
fn pointer_distance(p1: ptr, p2: ptr) -> u64 {
    let addr1: u64 = p1 as u64;
    let addr2: u64 = p2 as u64;
    
    if addr1 > addr2 {
        return addr1 - addr2;
    }
    return addr2 - addr1;
}

fn main() -> u64 {
    let arr: [u64; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let p1: ptr = &arr;
    let p2: ptr = (&arr as u64 + 40) as ptr;  // 5 elements away
    
    let distance: u64 = pointer_distance(p1, p2);
    return distance;  // Returns 40 (5 * 8 bytes)
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Lwanga's type system: strong, static, explicit
- ✅ Primitive types: u8, u16, u32, u64, i8, i16, i32, i64, ptr
- ✅ Type safety: no implicit conversions
- ✅ Explicit casting with `as` keyword
- ✅ Widening conversions (safe) vs narrowing conversions (potentially unsafe)
- ✅ Pointer/integer conversions
- ✅ Type compatibility and exact matching
- ✅ Common mistakes and how to avoid them
- ✅ Practical examples of type conversions

---

## Next Lesson

Now that you understand the type system, let's learn about constants and compile-time values in [Lesson 16: Constants](16-constants.md).

---

**Navigation**: [← Previous: Packed Structs](14-packed-structs.md) | [Tutorial Home](README.md) | [Next: Constants →](16-constants.md)
