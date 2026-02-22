# Lesson 10: Pointers Basics

**In this lesson, you'll understand memory addresses, how to work with pointers, and why they're essential for systems programming.**

---

## What is a Pointer?

A pointer is a variable that holds a memory address. Instead of storing a value directly, it stores the location where a value is stored.

**Analogy:**
- A regular variable is like a box containing a value
- A pointer is like a piece of paper with the address of a box

```lwanga
let x: u64 = 42;        // Box containing 42
let ptr: ptr = &x;      // Paper with address of the box
```

---

## Why Use Pointers?

**Reasons pointers are essential:**

1. **Strings** - Strings are sequences of bytes, pointers point to the first byte
2. **Large data** - Pass address instead of copying large structures
3. **Arrays** - Access elements through pointer arithmetic
4. **System programming** - Direct memory access for low-level operations
5. **Data structures** - Build linked lists, trees, graphs

---

## The Pointer Type

In Lwanga, `ptr` is the pointer type:

```lwanga
fn main() -> u64 {
    let value: u64 = 100;
    let address: ptr = &value;  // address holds memory location of value
    
    return 0;
}
```

**Design choice:** Lwanga has one pointer type (`ptr`) instead of typed pointers like C. This keeps the language simple while still being powerful.

---

## Getting an Address (&)

Use the `&` operator to get the address of a variable:

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr_to_x: ptr = &x;  // Get address of x
    
    // ptr_to_x now contains the memory address where x is stored
    
    return 0;
}
```

**What happens:**
```
Memory:
Address    Value
0x1000     42      <- x is stored here
0x1008     0x1000  <- ptr_to_x contains address of x
```

### Multiple Pointers

```lwanga
fn main() -> u64 {
    let value: u64 = 100;
    let ptr1: ptr = &value;
    let ptr2: ptr = &value;
    
    // Both ptr1 and ptr2 point to the same location
    
    return 0;
}
```

---

## String Pointers

Strings in Lwanga are pointers to character data:

```lwanga
fn main() -> u64 {
    let message: ptr = "Hello, World!";
    // message points to the first character 'H'
    
    return 0;
}
```

**Memory layout:**
```
message -> ['H']['e']['l']['l']['o'][','][' ']['W']['o']['r']['l']['d']['!']['\0']
           ^
           |
        Points here
```

**Why pointers for strings?**
- Efficient - no copying
- Simple - just an address
- Flexible - can point anywhere in the string

### String Example

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello, Lwanga!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 15);
    }
    
    return 0;
}
```

---

## Dereferencing Pointers (*)

Use the `*` operator to access the value at an address:

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

**Why unsafe?**

Dereferencing pointers is unsafe because:
- Pointer might be null (0)
- Pointer might be invalid
- Could access wrong memory
- Could cause crashes

**Design choice:** Requiring `unsafe` makes dangerous operations explicit and visible.

### Dereferencing Example

```lwanga
fn get_value_at(ptr: ptr) -> u64 {
    unsafe {
        return *ptr;
    }
}

fn main() -> u64 {
    let num: u64 = 100;
    let result: u64 = get_value_at(&num);
    
    return result;  // Returns 100
}
```

---

## Pointer Arithmetic

You can add to pointers to move through memory:

```lwanga
fn main() -> u64 {
    let msg: ptr = "ABC";
    
    unsafe {
        let first: u8 = *msg;          // 'A' (65)
        let second: u8 = *(msg + 1);   // 'B' (66)
        let third: u8 = *(msg + 2);    // 'C' (67)
        
        return first as u64;
    }
}
```

**Memory layout:**
```
msg     -> ['A']['B']['C']['\0']
msg+1   ->      ['B']['C']['\0']
msg+2   ->           ['C']['\0']
```

### Pointer Arithmetic Example

```lwanga
fn access_array_element(arr: ptr, index: u64) -> u64 {
    unsafe {
        let element_ptr: ptr = arr + index;
        return *element_ptr;
    }
}

fn main() -> u64 {
    let numbers: ptr = [10, 20, 30, 40, 50];
    let third: u64 = access_array_element(numbers, 2);
    
    return third;  // Returns 30
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

**Always check for null before dereferencing:**

```lwanga
fn safe_dereference(ptr: ptr) -> u64 {
    if ptr == 0 {
        return 0;  // Handle null pointer
    }
    
    unsafe {
        return *ptr;
    }
}
```

### Null Pointer Example

```lwanga
fn process_data(data: ptr) -> u64 {
    // Validate pointer
    if data == 0 {
        return 0;  // Error: null pointer
    }
    
    // Safe to use
    unsafe {
        let value: u64 = *data;
        return value;
    }
}

fn main() -> u64 {
    let x: u64 = 42;
    let result: u64 = process_data(&x);
    
    return result;  // Returns 42
}
```

---

## Practical Examples

### Example 1: String Length

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

**How it works:**
1. Start at beginning of string
2. Check if current character is null terminator (0)
3. If not, increment length and move to next character
4. Repeat until null terminator found

### Example 2: String Copy

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

### Example 3: Array Sum

```lwanga
fn array_sum(arr: ptr, len: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            sum = sum + *(arr + i);
            i = i + 1;
        }
    }
    
    return sum;
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    return array_sum(numbers, 5);  // Returns 15
}
```

### Example 4: Find Character

```lwanga
fn find_char(s: ptr, c: u8) -> ptr {
    let mut p: ptr = s;
    
    unsafe {
        while *p != 0 {
            if *p == c {
                return p;  // Found it
            }
            p = p + 1;
        }
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    let msg: ptr = "Hello";
    let pos: ptr = find_char(msg, 'l' as u8);
    
    if pos != 0 {
        return 1;  // Found
    }
    
    return 0;
}
```

### Example 5: Memory Compare

```lwanga
fn memcmp(ptr1: ptr, ptr2: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            if *(ptr1 + i) != *(ptr2 + i) {
                return 0;  // Different
            }
            i = i + 1;
        }
    }
    
    return 1;  // Same
}

fn main() -> u64 {
    let str1: ptr = "Hello";
    let str2: ptr = "Hello";
    
    return memcmp(str1, str2, 5);  // Returns 1
}
```

---

## Pointer Safety Rules

### Rule 1: Always Check for Null

```lwanga
fn safe_access(ptr: ptr) -> u64 {
    if ptr == 0 {
        return 0;  // Handle null
    }
    
    unsafe {
        return *ptr;
    }
}
```

### Rule 2: Don't Access Out of Bounds

```lwanga
fn safe_array_access(arr: ptr, index: u64, len: u64) -> u64 {
    if index >= len {
        return 0;  // Out of bounds
    }
    
    unsafe {
        return *(arr + index);
    }
}
```

### Rule 3: Don't Dereference Invalid Pointers

```lwanga
// Bad: Random address
let bad_ptr: ptr = 0x12345678;
let value: u64 = *bad_ptr;  // CRASH!

// Good: Valid address
let x: u64 = 42;
let good_ptr: ptr = &x;
let value: u64 = *good_ptr;  // OK
```

### Rule 4: Be Careful with Pointer Arithmetic

```lwanga
fn safe_pointer_add(ptr: ptr, offset: u64, max_offset: u64) -> ptr {
    if offset > max_offset {
        return 0;  // Would go out of bounds
    }
    
    return ptr + offset;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting unsafe Block

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let ptr: ptr = &x;
    let value: u64 = *ptr;  // ERROR: requires unsafe
    return value;
}
```

**Fix:**
```lwanga
unsafe {
    let value: u64 = *ptr;  // ✓ Correct
}
```

### Mistake 2: Dereferencing Null Pointer

```lwanga
fn main() -> u64 {
    let ptr: ptr = 0;
    unsafe {
        let value: u64 = *ptr;  // CRASH!
    }
    return value;
}
```

**Fix:**
```lwanga
if ptr != 0 {
    unsafe {
        let value: u64 = *ptr;  // ✓ Safe
    }
}
```

### Mistake 3: Pointer to Local Variable

```lwanga
fn get_pointer() -> ptr {
    let x: u64 = 42;
    return &x;  // Dangerous! x is destroyed after function returns
}
```

**Why dangerous?** The variable `x` is destroyed when the function returns, so the pointer becomes invalid.

### Mistake 4: Wrong Pointer Arithmetic

```lwanga
let msg: ptr = "Hello";
let ptr: ptr = msg + 10;  // Goes past end of string!
unsafe {
    let c: u8 = *ptr;  // Undefined behavior
}
```

---

## Exercises

### Exercise 1: Count Character

Write a function that counts how many times a character appears in a string.

<details>
<summary>Solution</summary>

```lwanga
fn count_char(s: ptr, c: u8) -> u64 {
    let mut count: u64 = 0;
    let mut p: ptr = s;
    
    unsafe {
        while *p != 0 {
            if *p == c {
                count = count + 1;
            }
            p = p + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    let msg: ptr = "Hello";
    return count_char(msg, 'l' as u8);  // Returns 2
}
```
</details>

### Exercise 2: Array Maximum

Write a function that finds the maximum value in an array.

<details>
<summary>Solution</summary>

```lwanga
fn array_max(arr: ptr, len: u64) -> u64 {
    if len == 0 {
        return 0;
    }
    
    let mut max: u64 = 0;
    unsafe {
        max = *arr;
    }
    
    let mut i: u64 = 1;
    
    unsafe {
        while i < len {
            let val: u64 = *(arr + i);
            if val > max {
                max = val;
            }
            i = i + 1;
        }
    }
    
    return max;
}

fn main() -> u64 {
    let numbers: ptr = [5, 2, 9, 1, 7];
    return array_max(numbers, 5);  // Returns 9
}
```
</details>

### Exercise 3: String Reverse

Write a function that reverses a string in place.

<details>
<summary>Solution</summary>

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

fn str_reverse(s: ptr) -> u64 {
    let len: u64 = strlen(s);
    let mut i: u64 = 0;
    
    unsafe {
        while i < len / 2 {
            let temp: u8 = *(s + i);
            *(s + i) = *(s + len - i - 1);
            *(s + len - i - 1) = temp;
            i = i + 1;
        }
    }
    
    return len;
}
```
</details>

### Exercise 4: Array Contains

Write a function that checks if an array contains a specific value.

<details>
<summary>Solution</summary>

```lwanga
fn array_contains(arr: ptr, len: u64, value: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(arr + i) == value {
                return 1;  // Found
            }
            i = i + 1;
        }
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    return array_contains(numbers, 5, 3);  // Returns 1
}
```
</details>

### Exercise 5: Memory Set

Write a function that sets all bytes in a memory region to a specific value.

<details>
<summary>Solution</summary>

```lwanga
fn memset(ptr: ptr, value: u8, n: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            *(ptr + i) = value;
            i = i + 1;
        }
    }
    
    return n;
}

fn main() -> u64 {
    let buffer: [u8; 10] = [0; 10];
    memset(&buffer, 0xFF, 10);
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What pointers are and why they're useful
- ✅ How to get addresses with &
- ✅ How to dereference pointers with *
- ✅ String pointers and memory layout
- ✅ Pointer arithmetic for array access
- ✅ Null pointers and how to check for them
- ✅ Safety rules for using pointers
- ✅ Common mistakes and how to avoid them
- ✅ Practical pointer operations

---

## Next Lesson

Now that you understand pointers, let's learn how to work with strings in [Lesson 11: Working with Strings](11-strings.md).

---

**Navigation**: [← Previous: Operators](09-operators.md) | [Tutorial Home](README.md) | [Next: Strings →](11-strings.md)
