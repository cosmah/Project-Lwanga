# Lesson 17: Memory Model

**Understand how memory works in Lwanga.**

---

## Stack vs Heap

Lwanga uses stack allocation by default:

```lwanga
fn main() -> u64 {
    let x: u64 = 42;  // Allocated on stack
    return x;
}  // x is automatically freed
```

---

## Variable Lifetime

Variables live until the end of their scope:

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        let y: u64 = 20;  // y lives here
    }  // y is freed
    
    return x;
}  // x is freed
```

---

## Memory Layout

```
High Address
+------------------+
|      Stack       |  Local variables
|        ↓         |
+------------------+
|                  |
|       ...        |
|                  |
+------------------+
|        ↑         |
|      Heap        |  Dynamic allocation
+------------------+
|      Data        |  Global variables
+------------------+
|      Text        |  Program code
+------------------+
Low Address
```

---

**Navigation**: [← Previous: Constants](16-constants.md) | [Tutorial Home](README.md) | [Next: Advanced Pointers →](18-pointers-advanced.md)
