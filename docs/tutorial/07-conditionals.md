# Lesson 7: Conditionals

**Learn how to make decisions in your code using if statements and comparison operators.**

---

## If Statements

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        return 1;
    }
    
    return 0;
}
```

---

## If-Else

```lwanga
fn main() -> u64 {
    let age: u64 = 18;
    
    if age >= 18 {
        return 1;  // Adult
    } else {
        return 0;  // Minor
    }
}
```

---

## Comparison Operators

```lwanga
x == y    // Equal
x != y    // Not equal
x < y     // Less than
x > y     // Greater than
x <= y    // Less than or equal
x >= y    // Greater than or equal
```

---

## Logical Operators

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    
    if x > 5 && y < 30 {
        return 1;  // Both conditions true
    }
    
    if x > 100 || y > 100 {
        return 2;  // At least one true
    }
    
    return 0;
}
```

---

## Nested Conditions

```lwanga
fn check_range(n: u64) -> u64 {
    if n < 10 {
        return 1;
    } else {
        if n < 20 {
            return 2;
        } else {
            return 3;
        }
    }
}
```

---

**Navigation**: [← Previous: Functions](06-functions.md) | [Tutorial Home](README.md) | [Next: Loops →](08-loops.md)
