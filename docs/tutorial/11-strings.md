# Lesson 11: Working with Strings

**Learn how to handle strings, calculate length, compare, and manipulate text.**

---

## String Basics

Strings in Lwanga are null-terminated byte sequences:

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    // Stored as: H e l l o ,   W o r l d ! \0
    
    return 0;
}
```

---

## String Length

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
    return strlen("Hello");  // Returns 5
}
```

---

## String Comparison

```lwanga
fn strcmp(s1: ptr, s2: ptr) -> u64 {
    let mut p1: ptr = s1;
    let mut p2: ptr = s2;
    
    unsafe {
        while *p1 != 0 && *p2 != 0 {
            if *p1 != *p2 {
                return 0;  // Not equal
            }
            p1 = p1 + 1;
            p2 = p2 + 1;
        }
        
        if *p1 == *p2 {
            return 1;  // Equal
        }
    }
    
    return 0;
}
```

---

## String Copy

```lwanga
fn strcpy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        *(dest + i) = 0;  // Null terminator
    }
    
    return i;
}
```

---

## Printing Strings

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print(msg: ptr) -> u64 {
    let len: u64 = strlen(msg);
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    
    return len;
}

fn main() -> u64 {
    print("Hello, World!\n");
    return 0;
}
```

---

## Escape Sequences

```lwanga
let newline: ptr = "\n";      // Newline
let tab: ptr = "\t";          // Tab
let quote: ptr = "\"";        // Double quote
let backslash: ptr = "\\";    // Backslash
```

---

**Navigation**: [← Previous: Pointers](10-pointers.md) | [Tutorial Home](README.md) | [Next: Arrays →](12-arrays.md)
