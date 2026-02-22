# Lesson 9: Operators

**Master all operators in Lwanga: arithmetic, comparison, logical, and bitwise.**

---

## Arithmetic Operators

```lwanga
let a: u64 = 10 + 5;   // Addition: 15
let b: u64 = 10 - 5;   // Subtraction: 5
let c: u64 = 10 * 5;   // Multiplication: 50
let d: u64 = 10 / 5;   // Division: 2
let e: u64 = 10 % 3;   // Modulo: 1
```

---

## Comparison Operators

```lwanga
let eq: u64 = (10 == 10);  // 1 (true)
let ne: u64 = (10 != 5);   // 1 (true)
let lt: u64 = (5 < 10);    // 1 (true)
let gt: u64 = (10 > 5);    // 1 (true)
let le: u64 = (5 <= 5);    // 1 (true)
let ge: u64 = (10 >= 5);   // 1 (true)
```

---

## Logical Operators

```lwanga
// AND: both must be true
if x > 5 && y < 10 {
    return 1;
}

// OR: at least one must be true
if x > 100 || y > 100 {
    return 1;
}

// NOT: inverts boolean
if !(x == 0) {
    return 1;
}
```

---

## Bitwise Operators

```lwanga
let a: u64 = 0b1010;  // 10
let b: u64 = 0b1100;  // 12

let and: u64 = a & b;   // 0b1000 = 8
let or: u64 = a | b;    // 0b1110 = 14
let xor: u64 = a ^ b;   // 0b0110 = 6
let not: u64 = ~a;      // Bitwise NOT
let shl: u64 = a << 1;  // 0b10100 = 20
let shr: u64 = a >> 1;  // 0b0101 = 5
```

---

## Operator Precedence

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

---

## Practical Examples

### Check Bit Flag

```lwanga
fn has_flag(value: u64, flag: u64) -> u64 {
    if (value & flag) != 0 {
        return 1;
    }
    return 0;
}
```

### Set Bit Flag

```lwanga
fn set_flag(value: u64, flag: u64) -> u64 {
    return value | flag;
}
```

### Clear Bit Flag

```lwanga
fn clear_flag(value: u64, flag: u64) -> u64 {
    return value & (~flag);
}
```

---

**Navigation**: [← Previous: Loops](08-loops.md) | [Tutorial Home](README.md) | [Next: Pointers →](10-pointers.md)
