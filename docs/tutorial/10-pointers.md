# Lesson 10: Pointers Basics

**Understand memory addresses and how to work with pointers.**

---

## What is a Pointer?

A pointer is a variable that holds a memory address. It "points to" a location in memory.

```lwanga
let x: u64 = 42;
let ptr: ptr = &x;  // ptr holds the address of x
```

---

## Getting an Address

Use `&` to get the address of a variable:

```lwanga
fn main() -> u64 {
    let value: u64 = 100;
    let address: ptr = &value;
    
    return 0;
}
```

---

## String Pointers

Strings are pointers to character data:

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    // msg points to the first character 'H'
    
    unsafe {
        syscall(1, 1, msg, 13);
    }
    
    return 0;
}
```

---

## Dereferencing Pointers

Use `*` to access the value at an address:

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;
    
    unsafe {
        let value: u64 = *ptr;  // Get value at address
        return value;  // Returns 42
    }
}
```

---

## Pointer Arithmetic

Add to pointers to move through memory:

```lwanga
fn main() -> u64 {
    let msg: ptr = "ABC";
    
    let first: ptr = msg;      // Points to 'A'
    let second: ptr = msg + 1; // Points to 'B'
    let third: ptr = msg + 2;  // Points to 'C'
    
    return 0;
}
```

---

## Null Pointers

A pointer with value 0 is null (points nowhere):

```lwanga
fn main() -> u64 {
    let null_ptr: ptr = 0;
    
    if null_ptr == 0 {
        return 1;  // It's null
    }
    
    return 0;
}
```

---

## Practical Example: String Length

```lwanga
fn strlen(s: ptr) -> u64 {
    let mut len: u64 = 0;
    let mut p: ptr = s;
    
    unsafe {
        while *p != 0 {
            len = len + 1;
            p = p + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let msg: ptr = "Hello";
    return strlen(msg);  // Returns 5
}
```

---

## Safety Warning

Pointers are powerful but dangerous:
- Always check for null
- Don't access invalid memory
- Use `unsafe` blocks
- Be careful with arithmetic

---

**Navigation**: [← Previous: Operators](09-operators.md) | [Tutorial Home](README.md) | [Next: Strings →](11-strings.md)
