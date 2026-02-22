# Lesson 5: Working with Numbers

**In this lesson, you'll learn how to perform calculations, use different number formats, and understand arithmetic operations in Lwanga.**

---

## Arithmetic Operations

Lwanga supports all standard arithmetic operations.

### Basic Operations

```lwanga
fn main() -> u64 {
    let a: u64 = 10;
    let b: u64 = 3;
    
    let sum: u64 = a + b;        // Addition: 13
    let difference: u64 = a - b;  // Subtraction: 7
    let product: u64 = a * b;     // Multiplication: 30
    let quotient: u64 = a / b;    // Division: 3 (integer division)
    let remainder: u64 = a % b;   // Modulo: 1
    
    return sum;
}
```

### Operator Precedence

Operations follow standard mathematical precedence:

1. **Parentheses** `()`
2. **Multiplication, Division, Modulo** `*`, `/`, `%`
3. **Addition, Subtraction** `+`, `-`

```lwanga
fn main() -> u64 {
    let result1: u64 = 2 + 3 * 4;      // 14 (not 20)
    let result2: u64 = (2 + 3) * 4;    // 20
    let result3: u64 = 10 - 2 + 3;     // 11 (left to right)
    let result4: u64 = 10 / 2 * 3;     // 15 (left to right)
    
    return result1;
}
```

**Why this order?**
- Matches mathematical conventions
- Predictable behavior
- Same as most programming languages

---

## Addition

### Basic Addition

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    let sum: u64 = x + y;
    
    return sum;  // Returns 30
}
```

### Adding Multiple Numbers

```lwanga
fn main() -> u64 {
    let a: u64 = 5;
    let b: u64 = 10;
    let c: u64 = 15;
    let total: u64 = a + b + c;
    
    return total;  // Returns 30
}
```

### Incrementing

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    counter = counter + 1;  // Increment by 1
    counter = counter + 1;
    counter = counter + 1;
    
    return counter;  // Returns 3
}
```

**Note:** Lwanga doesn't have `++` operator. Use `counter = counter + 1`.

**Why no `++`?**
- Simpler language
- Explicit is better
- Avoids confusion about pre/post increment

---

## Subtraction

### Basic Subtraction

```lwanga
fn main() -> u64 {
    let x: u64 = 50;
    let y: u64 = 20;
    let difference: u64 = x - y;
    
    return difference;  // Returns 30
}
```

### Underflow Warning

**Important:** Subtracting a larger number from a smaller one causes underflow:

```lwanga
fn main() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    let result: u64 = x - y;  // Underflow!
    
    return result;  // Returns 18446744073709551611 (wraps around)
}
```

**Why does this happen?**

Unsigned integers can't be negative. When you go below zero, they wrap around to the maximum value.

```
5 - 10 = -5
But u64 can't hold -5, so it wraps:
-5 becomes 18446744073709551611 (2^64 - 5)
```

**How to avoid:**

```lwanga
fn main() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    
    // Check before subtracting
    if x >= y {
        let result: u64 = x - y;
        return result;
    } else {
        return 0;  // Handle error case
    }
}
```

---

## Multiplication

### Basic Multiplication

```lwanga
fn main() -> u64 {
    let width: u64 = 10;
    let height: u64 = 20;
    let area: u64 = width * height;
    
    return area;  // Returns 200
}
```

### Overflow Warning

Multiplying large numbers can overflow:

```lwanga
fn main() -> u64 {
    let x: u64 = 10000000000;  // 10 billion
    let y: u64 = 10000000000;  // 10 billion
    let result: u64 = x * y;    // Overflow! Result wraps around
    
    return result;
}
```

**Maximum u64 value:** 18,446,744,073,709,551,615

If multiplication exceeds this, it wraps around.

---

## Division

### Integer Division

Division in Lwanga is **integer division** - it discards the remainder:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 3;
    let result: u64 = x / y;
    
    return result;  // Returns 3 (not 3.333...)
}
```

**Examples:**

```lwanga
fn main() -> u64 {
    let a: u64 = 10 / 2;  // 5
    let b: u64 = 10 / 3;  // 3
    let c: u64 = 10 / 4;  // 2
    let d: u64 = 10 / 5;  // 2
    let e: u64 = 10 / 6;  // 1
    
    return a;
}
```

### Division by Zero

**Critical:** Division by zero is undefined behavior!

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 0;
    let result: u64 = x / y;  // CRASH or undefined behavior!
    
    return result;
}
```

**Always check before dividing:**

```lwanga
fn safe_divide(a: u64, b: u64) -> u64 {
    if b == 0 {
        return 0;  // Or handle error appropriately
    }
    return a / b;
}

fn main() -> u64 {
    let result: u64 = safe_divide(10, 0);
    return result;  // Returns 0 (safe)
}
```

---

## Modulo (Remainder)

The modulo operator `%` gives the remainder after division:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 3;
    let remainder: u64 = x % y;
    
    return remainder;  // Returns 1 (10 = 3*3 + 1)
}
```

### Examples

```lwanga
fn main() -> u64 {
    let a: u64 = 10 % 2;  // 0 (10 is even)
    let b: u64 = 11 % 2;  // 1 (11 is odd)
    let c: u64 = 17 % 5;  // 2 (17 = 5*3 + 2)
    let d: u64 = 100 % 7; // 2 (100 = 7*14 + 2)
    
    return a;
}
```

### Practical Use: Even/Odd Check

```lwanga
fn is_even(n: u64) -> u64 {
    if n % 2 == 0 {
        return 1;  // True (even)
    } else {
        return 0;  // False (odd)
    }
}

fn main() -> u64 {
    let result: u64 = is_even(42);
    return result;  // Returns 1 (42 is even)
}
```

---

## Number Literals

### Decimal (Base 10)

Standard decimal numbers:

```lwanga
fn main() -> u64 {
    let a: u64 = 0;
    let b: u64 = 42;
    let c: u64 = 1000;
    let d: u64 = 123456789;
    
    return a;
}
```

### Hexadecimal (Base 16)

Use `0x` prefix for hexadecimal:

```lwanga
fn main() -> u64 {
    let a: u64 = 0x00;      // 0
    let b: u64 = 0xFF;      // 255
    let c: u64 = 0x100;     // 256
    let d: u64 = 0xDEADBEEF; // 3735928559
    
    return a;
}
```

**When to use hex:**
- Memory addresses
- Bit patterns
- Color values
- Low-level programming

**Example: Memory address**

```lwanga
fn main() -> u64 {
    let addr: u64 = 0x7fff5fbff000;
    return addr;
}
```

### Binary (Base 2)

Use `0b` prefix for binary:

```lwanga
fn main() -> u64 {
    let a: u64 = 0b0;        // 0
    let b: u64 = 0b1;        // 1
    let c: u64 = 0b1010;     // 10
    let d: u64 = 0b11111111; // 255
    
    return a;
}
```

**When to use binary:**
- Bit flags
- Bit manipulation
- Understanding bit patterns

**Example: Flags**

```lwanga
fn main() -> u64 {
    let read_flag: u64 = 0b001;   // 1
    let write_flag: u64 = 0b010;  // 2
    let exec_flag: u64 = 0b100;   // 4
    
    let permissions: u64 = read_flag | write_flag;  // 3 (read + write)
    
    return permissions;
}
```

### Octal (Base 8)

Use `0o` prefix for octal:

```lwanga
fn main() -> u64 {
    let a: u64 = 0o0;    // 0
    let b: u64 = 0o10;   // 8
    let c: u64 = 0o777;  // 511
    
    return a;
}
```

**When to use octal:**
- Unix file permissions
- Legacy systems

---

## Type-Specific Operations

### Working with Different Types

Remember: you must use the same type for operations:

```lwanga
fn main() -> u64 {
    let a: u32 = 10;
    let b: u32 = 20;
    let sum: u32 = a + b;  // ✓ Both u32
    
    let c: u64 = sum as u64;  // Convert to u64
    
    return c;
}
```

### Mixing Types (Error)

```lwanga
fn main() -> u64 {
    let a: u32 = 10;
    let b: u64 = 20;
    let sum: u64 = a + b;  // ERROR: type mismatch
    
    return sum;
}
```

**Fix with casting:**

```lwanga
fn main() -> u64 {
    let a: u32 = 10;
    let b: u64 = 20;
    let sum: u64 = (a as u64) + b;  // ✓ Both u64 now
    
    return sum;
}
```

---

## Practical Examples

### Example 1: Calculate Average

```lwanga
fn main() -> u64 {
    let num1: u64 = 10;
    let num2: u64 = 20;
    let num3: u64 = 30;
    
    let sum: u64 = num1 + num2 + num3;
    let count: u64 = 3;
    let average: u64 = sum / count;
    
    return average;  // Returns 20
}
```

### Example 2: Convert Bytes to Kilobytes

```lwanga
fn main() -> u64 {
    let bytes: u64 = 4096;
    let kilobytes: u64 = bytes / 1024;
    
    return kilobytes;  // Returns 4
}
```

### Example 3: Calculate Circle Area (Approximation)

```lwanga
fn main() -> u64 {
    let radius: u64 = 10;
    let pi_approx: u64 = 3;  // Simplified
    let area: u64 = pi_approx * radius * radius;
    
    return area;  // Returns 300 (approximate)
}
```

### Example 4: Time Conversion

```lwanga
fn main() -> u64 {
    let seconds: u64 = 3665;
    
    let hours: u64 = seconds / 3600;
    let remaining: u64 = seconds % 3600;
    let minutes: u64 = remaining / 60;
    let secs: u64 = remaining % 60;
    
    // hours = 1, minutes = 1, secs = 5
    return hours;
}
```

### Example 5: Check if Power of 2

```lwanga
fn is_power_of_2(n: u64) -> u64 {
    if n == 0 {
        return 0;  // False
    }
    
    // Power of 2 has only one bit set
    // n & (n-1) == 0 for powers of 2
    let result: u64 = n & (n - 1);
    
    if result == 0 {
        return 1;  // True
    } else {
        return 0;  // False
    }
}

fn main() -> u64 {
    let result: u64 = is_power_of_2(16);
    return result;  // Returns 1 (16 is 2^4)
}
```

---

## Common Mistakes

### Mistake 1: Integer Division Surprise

```lwanga
let result: u64 = 10 / 3;  // Expects 3.33, gets 3
```

**Remember:** Integer division discards the remainder.

### Mistake 2: Underflow

```lwanga
let result: u64 = 5 - 10;  // Wraps around to huge number
```

**Fix:** Check before subtracting.

### Mistake 3: Division by Zero

```lwanga
let result: u64 = x / 0;  // CRASH!
```

**Fix:** Always check divisor is not zero.

### Mistake 4: Overflow

```lwanga
let huge: u8 = 200 + 100;  // Overflow! (max u8 is 255)
```

**Fix:** Use larger type or check bounds.

---

## Exercises

### Exercise 1: Basic Arithmetic

Calculate: (10 + 5) * 3 - 2

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let result: u64 = (10 + 5) * 3 - 2;
    return result;  // Returns 43
}
```
</details>

### Exercise 2: Temperature Conversion

Convert 100 Fahrenheit to Celsius using: C = (F - 32) * 5 / 9

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let fahrenheit: u64 = 100;
    let celsius: u64 = (fahrenheit - 32) * 5 / 9;
    return celsius;  // Returns 37
}
```
</details>

### Exercise 3: Even or Odd

Write a function that returns 1 if a number is even, 0 if odd.

<details>
<summary>Solution</summary>

```lwanga
fn is_even(n: u64) -> u64 {
    if n % 2 == 0 {
        return 1;
    } else {
        return 0;
    }
}

fn main() -> u64 {
    return is_even(42);  // Returns 1
}
```
</details>

### Exercise 4: Sum of First N Numbers

Calculate 1 + 2 + 3 + ... + 10

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while i <= 10 {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;  // Returns 55
}
```
</details>

### Exercise 5: Hexadecimal

Create a variable with hex value 0xFF and add 1 to it.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let hex_value: u64 = 0xFF;  // 255
    let result: u64 = hex_value + 1;
    return result;  // Returns 256
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ All arithmetic operators (+, -, *, /, %)
- ✅ Operator precedence and evaluation order
- ✅ Integer division and its behavior
- ✅ Underflow and overflow issues
- ✅ Number literals (decimal, hex, binary, octal)
- ✅ Type safety in arithmetic operations
- ✅ Practical calculation examples
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you can work with numbers, let's learn about functions in [Lesson 6: Functions](06-functions.md).

---

**Navigation**: [← Previous: Variables and Types](04-variables-types.md) | [Tutorial Home](README.md) | [Next: Functions →](06-functions.md)
