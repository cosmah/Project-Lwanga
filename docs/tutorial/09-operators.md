# Lesson 9: Operators

**In this lesson, you'll master all operators in Lwanga: arithmetic, comparison, logical, and bitwise operations.**

---

## Arithmetic Operators

### Basic Arithmetic

```lwanga
fn main() -> u64 {
    let a: u64 = 10;
    let b: u64 = 3;
    
    let sum: u64 = a + b;        // 13
    let difference: u64 = a - b;  // 7
    let product: u64 = a * b;     // 30
    let quotient: u64 = a / b;    // 3 (integer division)
    let remainder: u64 = a % b;   // 1
    
    return sum;
}
```

### Addition (+)

```lwanga
fn test_addition() -> u64 {
    let x: u64 = 100;
    let y: u64 = 200;
    let z: u64 = x + y;  // 300
    
    // Can chain additions
    let total: u64 = 10 + 20 + 30 + 40;  // 100
    
    return total;
}
```

**Overflow behavior:**
```lwanga
fn overflow_example() -> u64 {
    let max: u64 = 18446744073709551615;  // Max u64
    let result: u64 = max + 1;  // Wraps to 0
    return result;
}
```

### Subtraction (-)

```lwanga
fn test_subtraction() -> u64 {
    let x: u64 = 100;
    let y: u64 = 30;
    let z: u64 = x - y;  // 70
    
    return z;
}
```

**Underflow warning:**
```lwanga
fn underflow_example() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    let result: u64 = x - y;  // Wraps to large number!
    return result;
}
```

### Multiplication (*)

```lwanga
fn test_multiplication() -> u64 {
    let width: u64 = 10;
    let height: u64 = 20;
    let area: u64 = width * height;  // 200
    
    // Powers of 2
    let power: u64 = 2 * 2 * 2 * 2;  // 16
    
    return area;
}
```

### Division (/)

```lwanga
fn test_division() -> u64 {
    let x: u64 = 100;
    let y: u64 = 3;
    let result: u64 = x / y;  // 33 (integer division)
    
    // More examples
    let a: u64 = 10 / 2;  // 5
    let b: u64 = 10 / 3;  // 3
    let c: u64 = 10 / 4;  // 2
    
    return result;
}
```

**Division by zero:**
```lwanga
fn safe_divide(a: u64, b: u64) -> u64 {
    if b == 0 {
        return 0;  // Handle error
    }
    return a / b;
}
```

### Modulo (%)

```lwanga
fn test_modulo() -> u64 {
    let x: u64 = 17;
    let y: u64 = 5;
    let remainder: u64 = x % y;  // 2 (17 = 5*3 + 2)
    
    // Check even/odd
    let is_even: u64 = 42 % 2;  // 0 (even)
    let is_odd: u64 = 43 % 2;   // 1 (odd)
    
    return remainder;
}
```

---

## Comparison Operators

### Equality (==)

```lwanga
fn test_equality() -> u64 {
    let x: u64 = 10;
    let y: u64 = 10;
    
    if x == y {
        return 1;  // Equal
    }
    
    return 0;
}
```

### Inequality (!=)

```lwanga
fn test_inequality() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    
    if x != y {
        return 1;  // Not equal
    }
    
    return 0;
}
```

### Less Than (<)

```lwanga
fn test_less_than() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    
    if x < y {
        return 1;  // x is less than y
    }
    
    return 0;
}
```

### Greater Than (>)

```lwanga
fn test_greater_than() -> u64 {
    let x: u64 = 10;
    let y: u64 = 5;
    
    if x > y {
        return 1;  // x is greater than y
    }
    
    return 0;
}
```

### Less Than or Equal (<=)

```lwanga
fn test_less_equal() -> u64 {
    let x: u64 = 10;
    let y: u64 = 10;
    
    if x <= y {
        return 1;  // x is less than or equal to y
    }
    
    return 0;
}
```

### Greater Than or Equal (>=)

```lwanga
fn test_greater_equal() -> u64 {
    let x: u64 = 10;
    let y: u64 = 5;
    
    if x >= y {
        return 1;  // x is greater than or equal to y
    }
    
    return 0;
}
```

---

## Logical Operators

### AND (&&)

```lwanga
fn test_and() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    
    if x > 5 && y < 30 {
        return 1;  // Both conditions true
    }
    
    return 0;
}
```

**Truth table:**
```lwanga
fn and_truth_table() -> u64 {
    let t1: u64 = 0 && 0;  // 0
    let t2: u64 = 0 && 1;  // 0
    let t3: u64 = 1 && 0;  // 0
    let t4: u64 = 1 && 1;  // 1
    
    return t4;
}
```

### OR (||)

```lwanga
fn test_or() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    
    if x > 100 || y > 10 {
        return 1;  // At least one condition true
    }
    
    return 0;
}
```

**Truth table:**
```lwanga
fn or_truth_table() -> u64 {
    let t1: u64 = 0 || 0;  // 0
    let t2: u64 = 0 || 1;  // 1
    let t3: u64 = 1 || 0;  // 1
    let t4: u64 = 1 || 1;  // 1
    
    return t2;
}
```

### NOT (!)

```lwanga
fn test_not() -> u64 {
    let x: u64 = 0;
    
    if !x {
        return 1;  // x is false (0)
    }
    
    return 0;
}
```

---

## Bitwise Operators

### Bitwise AND (&)

```lwanga
fn test_bitwise_and() -> u64 {
    let a: u64 = 0b1010;  // 10
    let b: u64 = 0b1100;  // 12
    let result: u64 = a & b;  // 0b1000 = 8
    
    return result;
}
```

**Use case: Check if bit is set**
```lwanga
fn has_flag(value: u64, flag: u64) -> u64 {
    if (value & flag) != 0 {
        return 1;
    }
    return 0;
}
```

### Bitwise OR (|)

```lwanga
fn test_bitwise_or() -> u64 {
    let a: u64 = 0b1010;  // 10
    let b: u64 = 0b1100;  // 12
    let result: u64 = a | b;  // 0b1110 = 14
    
    return result;
}
```

**Use case: Set bit flag**
```lwanga
fn set_flag(value: u64, flag: u64) -> u64 {
    return value | flag;
}
```

### Bitwise XOR (^)

```lwanga
fn test_bitwise_xor() -> u64 {
    let a: u64 = 0b1010;  // 10
    let b: u64 = 0b1100;  // 12
    let result: u64 = a ^ b;  // 0b0110 = 6
    
    return result;
}
```

**Use case: Toggle bit**
```lwanga
fn toggle_flag(value: u64, flag: u64) -> u64 {
    return value ^ flag;
}
```

### Bitwise NOT (~)

```lwanga
fn test_bitwise_not() -> u64 {
    let a: u64 = 0b1010;
    let result: u64 = ~a;  // Inverts all bits
    
    return result;
}
```

### Left Shift (<<)

```lwanga
fn test_left_shift() -> u64 {
    let a: u64 = 5;  // 0b101
    let result: u64 = a << 2;  // 0b10100 = 20
    
    // Equivalent to multiplying by 2^n
    let x: u64 = 3 << 1;  // 6 (3 * 2)
    let y: u64 = 3 << 2;  // 12 (3 * 4)
    let z: u64 = 3 << 3;  // 24 (3 * 8)
    
    return result;
}
```

### Right Shift (>>)

```lwanga
fn test_right_shift() -> u64 {
    let a: u64 = 20;  // 0b10100
    let result: u64 = a >> 2;  // 0b101 = 5
    
    // Equivalent to dividing by 2^n
    let x: u64 = 16 >> 1;  // 8 (16 / 2)
    let y: u64 = 16 >> 2;  // 4 (16 / 4)
    let z: u64 = 16 >> 3;  // 2 (16 / 8)
    
    return result;
}
```

---

## Operator Precedence

**From highest to lowest:**

1. Parentheses `()`
2. Unary `!`, `~`
3. Multiplication, Division, Modulo `*`, `/`, `%`
4. Addition, Subtraction `+`, `-`
5. Bit shifts `<<`, `>>`
6. Comparison `<`, `>`, `<=`, `>=`
7. Equality `==`, `!=`
8. Bitwise AND `&`
9. Bitwise XOR `^`
10. Bitwise OR `|`
11. Logical AND `&&`
12. Logical OR `||`

**Examples:**
```lwanga
fn precedence_examples() -> u64 {
    let a: u64 = 2 + 3 * 4;      // 14 (not 20)
    let b: u64 = (2 + 3) * 4;    // 20
    let c: u64 = 10 - 2 + 3;     // 11 (left to right)
    let d: u64 = 10 / 2 * 3;     // 15 (left to right)
    
    return a;
}
```

---

## Practical Examples

### Example 1: Bit Flags

```lwanga
const READ: u64 = 0b001;   // 1
const WRITE: u64 = 0b010;  // 2
const EXEC: u64 = 0b100;   // 4

fn check_permissions(perms: u64) -> u64 {
    let can_read: u64 = (perms & READ) != 0;
    let can_write: u64 = (perms & WRITE) != 0;
    let can_exec: u64 = (perms & EXEC) != 0;
    
    return can_read + can_write + can_exec;
}

fn main() -> u64 {
    let perms: u64 = READ | WRITE;  // Read + Write
    return check_permissions(perms);  // Returns 2
}
```

### Example 2: Fast Multiplication/Division

```lwanga
fn fast_multiply_by_power_of_2(n: u64, power: u64) -> u64 {
    return n << power;
}

fn fast_divide_by_power_of_2(n: u64, power: u64) -> u64 {
    return n >> power;
}

fn main() -> u64 {
    let x: u64 = fast_multiply_by_power_of_2(5, 3);  // 5 * 8 = 40
    let y: u64 = fast_divide_by_power_of_2(40, 3);   // 40 / 8 = 5
    
    return x;
}
```

### Example 3: Swap Without Temp Variable

```lwanga
fn swap_xor(a: u64, b: u64) -> u64 {
    let mut x: u64 = a;
    let mut y: u64 = b;
    
    x = x ^ y;
    y = x ^ y;  // y now has original x
    x = x ^ y;  // x now has original y
    
    return x;  // Returns original b
}

fn main() -> u64 {
    return swap_xor(10, 20);  // Returns 20
}
```

### Example 4: Check Power of 2

```lwanga
fn is_power_of_2(n: u64) -> u64 {
    if n == 0 {
        return 0;
    }
    
    // Power of 2 has only one bit set
    // n & (n-1) clears the lowest set bit
    if (n & (n - 1)) == 0 {
        return 1;
    }
    
    return 0;
}

fn main() -> u64 {
    let r1: u64 = is_power_of_2(16);  // 1
    let r2: u64 = is_power_of_2(15);  // 0
    
    return r1;
}
```

### Example 5: Count Set Bits

```lwanga
fn count_set_bits(n: u64) -> u64 {
    let mut count: u64 = 0;
    let mut num: u64 = n;
    
    while num > 0 {
        count = count + (num & 1);
        num = num >> 1;
    }
    
    return count;
}

fn main() -> u64 {
    return count_set_bits(0b10110);  // Returns 3
}
```

---

## Common Mistakes

### Mistake 1: Confusing = and ==

```lwanga
let x: u64 = 10;
if x = 10 {  // ERROR: assignment, not comparison
    return 1;
}
```

**Fix:**
```lwanga
if x == 10 {  // ✓ Comparison
    return 1;
}
```

### Mistake 2: Wrong Precedence

```lwanga
let result: u64 = 10 + 5 * 2;  // 20, not 30
```

**Fix:**
```lwanga
let result: u64 = (10 + 5) * 2;  // ✓ 30
```

### Mistake 3: Integer Division Surprise

```lwanga
let result: u64 = 10 / 3;  // 3, not 3.33
```

### Mistake 4: Bitwise vs Logical

```lwanga
// Bitwise (works on bits)
let a: u64 = 5 & 3;  // 1

// Logical (works on boolean values)
if 5 && 3 {  // Both non-zero, so true
    return 1;
}
```

---

## Exercises

### Exercise 1: Calculate Expression

Calculate: (15 + 25) * 2 - 10 / 2

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let result: u64 = (15 + 25) * 2 - 10 / 2;
    return result;  // Returns 75
}
```
</details>

### Exercise 2: Check Even Using Bitwise

Write a function that checks if a number is even using bitwise AND.

<details>
<summary>Solution</summary>

```lwanga
fn is_even_bitwise(n: u64) -> u64 {
    if (n & 1) == 0 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    return is_even_bitwise(42);  // Returns 1
}
```
</details>

### Exercise 3: Set Nth Bit

Write a function that sets the nth bit of a number.

<details>
<summary>Solution</summary>

```lwanga
fn set_bit(num: u64, n: u64) -> u64 {
    return num | (1 << n);
}

fn main() -> u64 {
    let result: u64 = set_bit(0b1000, 1);
    return result;  // Returns 0b1010 = 10
}
```
</details>

### Exercise 4: Clear Nth Bit

Write a function that clears the nth bit of a number.

<details>
<summary>Solution</summary>

```lwanga
fn clear_bit(num: u64, n: u64) -> u64 {
    return num & ~(1 << n);
}

fn main() -> u64 {
    let result: u64 = clear_bit(0b1111, 2);
    return result;  // Returns 0b1011 = 11
}
```
</details>

### Exercise 5: Toggle Nth Bit

Write a function that toggles the nth bit of a number.

<details>
<summary>Solution</summary>

```lwanga
fn toggle_bit(num: u64, n: u64) -> u64 {
    return num ^ (1 << n);
}

fn main() -> u64 {
    let result: u64 = toggle_bit(0b1010, 0);
    return result;  // Returns 0b1011 = 11
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ All arithmetic operators and their behavior
- ✅ Comparison operators for conditions
- ✅ Logical operators for boolean logic
- ✅ Bitwise operators for bit manipulation
- ✅ Operator precedence rules
- ✅ Practical applications of operators
- ✅ Common mistakes and how to avoid them
- ✅ Bit manipulation techniques

---

## Next Lesson

Now that you master all operators, let's learn about pointers and memory addresses in [Lesson 10: Pointers Basics](10-pointers.md).

---

**Navigation**: [← Previous: Loops](08-loops.md) | [Tutorial Home](README.md) | [Next: Pointers →](10-pointers.md)
