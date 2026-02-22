# Lesson 18: Advanced Pointers

**In this lesson, you'll master pointer arithmetic, multi-level pointers, pointer casting, and advanced pointer techniques for low-level programming.**

---

## Pointer Arithmetic

Pointer arithmetic allows you to navigate through memory by adding or subtracting offsets from pointers.

### Basic Pointer Arithmetic

```lwanga
fn main() -> u64 {
    let numbers: [u64; 5] = [10, 20, 30, 40, 50];
    let ptr: ptr = &numbers;
    
    unsafe {
        // Access first element
        let first: u64 = *(ptr as ptr);
        
        // Move pointer forward by 8 bytes (size of u64)
        let second_ptr: ptr = (ptr as u64 + 8) as ptr;
        let second: u64 = *(second_ptr as ptr);
        
        // Move pointer forward by 16 bytes (2 * u64)
        let third_ptr: ptr = (ptr as u64 + 16) as ptr;
        let third: u64 = *(third_ptr as ptr);
        
        return third;  // Returns 30
    }
}
```

**How it works:**
- Array elements are stored sequentially in memory
- Each `u64` is 8 bytes
- Adding 8 to the pointer moves to the next element
- Adding 16 moves two elements forward

### Calculating Element Addresses

```lwanga
fn get_element_address(base: ptr, index: u64, element_size: u64) -> ptr {
    let offset: u64 = index * element_size;
    let address: u64 = base as u64 + offset;
    return address as ptr;
}

fn main() -> u64 {
    let arr: [u64; 10] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9];
    let base: ptr = &arr;
    
    // Get address of element at index 5
    let elem_ptr: ptr = get_element_address(base, 5, 8);
    
    unsafe {
        let value: u64 = *(elem_ptr as ptr);
        return value;  // Returns 5
    }
}
```

### Iterating with Pointers

```lwanga
fn sum_array(arr: ptr, length: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < length {
            let offset: u64 = i * 8;  // 8 bytes per u64
            let elem_ptr: ptr = (arr as u64 + offset) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            sum = sum + value;
            i = i + 1;
        }
    }
    
    return sum;
}

fn main() -> u64 {
    let numbers: [u64; 5] = [10, 20, 30, 40, 50];
    let total: u64 = sum_array(&numbers, 5);
    return total;  // Returns 150
}
```

---

## Multi-Level Pointers

### Pointer to Pointer

A pointer that points to another pointer:

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let p1: ptr = &x;           // Pointer to x
    let p2: ptr = &p1;          // Pointer to p1
    
    unsafe {
        // Dereference p1 to get x
        let value1: u64 = *(p1 as ptr);
        
        // Dereference p2 to get p1, then dereference p1 to get x
        let p1_value: ptr = *(p2 as ptr);
        let value2: u64 = *(p1_value as ptr);
        
        return value2;  // Returns 42
    }
}
```

**Memory layout:**
```
x:  [42]
     ↑
p1: [address of x]
     ↑
p2: [address of p1]
```

### Practical Use: Array of Pointers

```lwanga
fn main() -> u64 {
    let a: u64 = 10;
    let b: u64 = 20;
    let c: u64 = 30;
    
    // Array of pointers
    let ptrs: [ptr; 3] = [&a, &b, &c];
    let ptr_array: ptr = &ptrs;
    
    unsafe {
        // Get second pointer from array
        let second_ptr_addr: ptr = (ptr_array as u64 + 8) as ptr;
        let second_ptr: ptr = *(second_ptr_addr as ptr);
        
        // Dereference to get value
        let value: u64 = *(second_ptr as ptr);
        return value;  // Returns 20
    }
}
```

---

## Generic Memory Operations

### Memory Copy

```lwanga
fn memcpy(dest: ptr, src: ptr, n: u64) -> ptr {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            let src_byte: ptr = (src as u64 + i) as ptr;
            let dest_byte: ptr = (dest as u64 + i) as ptr;
            
            let byte: u8 = *(src_byte as ptr);
            *(dest_byte as ptr) = byte;
            
            i = i + 1;
        }
    }
    
    return dest;
}

fn main() -> u64 {
    let src: [u8; 5] = [1, 2, 3, 4, 5];
    let mut dest: [u8; 5] = [0, 0, 0, 0, 0];
    
    memcpy(&dest, &src, 5);
    
    unsafe {
        let first: u8 = *((&dest) as ptr);
        return first as u64;  // Returns 1
    }
}
```

### Memory Set

```lwanga
fn memset(ptr: ptr, value: u8, n: u64) -> ptr {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            let byte_ptr: ptr = (ptr as u64 + i) as ptr;
            *(byte_ptr as ptr) = value;
            i = i + 1;
        }
    }
    
    return ptr;
}

fn main() -> u64 {
    let mut buffer: [u8; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    // Fill with 0xFF
    memset(&buffer, 0xFF, 10);
    
    unsafe {
        let first: u8 = *((&buffer) as ptr);
        return first as u64;  // Returns 255
    }
}
```

### Memory Compare

```lwanga
fn memcmp(ptr1: ptr, ptr2: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            let byte1_ptr: ptr = (ptr1 as u64 + i) as ptr;
            let byte2_ptr: ptr = (ptr2 as u64 + i) as ptr;
            
            let byte1: u8 = *(byte1_ptr as ptr);
            let byte2: u8 = *(byte2_ptr as ptr);
            
            if byte1 != byte2 {
                return 0;  // Not equal
            }
            
            i = i + 1;
        }
    }
    
    return 1;  // Equal
}

fn main() -> u64 {
    let arr1: [u8; 5] = [1, 2, 3, 4, 5];
    let arr2: [u8; 5] = [1, 2, 3, 4, 5];
    
    let equal: u64 = memcmp(&arr1, &arr2, 5);
    return equal;  // Returns 1
}
```

---

## Pointer Comparison

### Comparing Addresses

```lwanga
fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let p1: ptr = &arr;
    let p2: ptr = ((&arr) as u64 + 16) as ptr;  // Third element
    
    let addr1: u64 = p1 as u64;
    let addr2: u64 = p2 as u64;
    
    if addr1 < addr2 {
        return 1;  // p1 is before p2 in memory
    }
    
    return 0;
}
```

### Calculating Distance Between Pointers

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
    let start: ptr = &arr;
    let end: ptr = ((&arr) as u64 + 72) as ptr;  // 9 elements * 8 bytes
    
    let distance: u64 = pointer_distance(start, end);
    return distance;  // Returns 72
}
```

---

## Pointer Casting

### Reinterpreting Data

```lwanga
fn main() -> u64 {
    let value: u64 = 0x0102030405060708;
    let ptr: ptr = &value;
    
    unsafe {
        // Read as u64
        let as_u64: u64 = *(ptr as ptr);
        
        // Read first byte as u8
        let as_u8: u8 = *(ptr as ptr);
        
        // Read first 4 bytes as u32
        let as_u32: u32 = *(ptr as ptr);
        
        return as_u8 as u64;  // Returns 8 (little-endian)
    }
}
```

### Type Punning

```lwanga
fn float_to_bits(f: u64) -> u64 {
    // In a real implementation, this would reinterpret float bits
    // For demonstration, we'll just return the value
    return f;
}

fn bits_to_float(bits: u64) -> u64 {
    return bits;
}

fn main() -> u64 {
    let value: u64 = 42;
    let bits: u64 = float_to_bits(value);
    let back: u64 = bits_to_float(bits);
    return back;  // Returns 42
}
```

---

## Practical Examples

### Example 1: String Length

```lwanga
fn strlen(s: ptr) -> u64 {
    let mut len: u64 = 0;
    
    unsafe {
        while *(((s as u64) + len) as ptr) != 0 {
            len = len + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    let length: u64 = strlen(msg);
    return length;  // Returns 13
}
```

### Example 2: Reverse Array

```lwanga
fn reverse_array(arr: ptr, length: u64) -> u64 {
    let mut left: u64 = 0;
    let mut right: u64 = length - 1;
    
    unsafe {
        while left < right {
            // Get pointers to elements
            let left_ptr: ptr = (arr as u64 + left * 8) as ptr;
            let right_ptr: ptr = (arr as u64 + right * 8) as ptr;
            
            // Swap elements
            let temp: u64 = *(left_ptr as ptr);
            *(left_ptr as ptr) = *(right_ptr as ptr);
            *(right_ptr as ptr) = temp;
            
            left = left + 1;
            right = right - 1;
        }
    }
    
    return 0;
}

fn main() -> u64 {
    let mut arr: [u64; 5] = [1, 2, 3, 4, 5];
    reverse_array(&arr, 5);
    
    unsafe {
        let first: u64 = *((&arr) as ptr);
        return first;  // Returns 5
    }
}
```

### Example 3: Find Element

```lwanga
fn find_element(arr: ptr, length: u64, target: u64) -> ptr {
    let mut i: u64 = 0;
    
    unsafe {
        while i < length {
            let elem_ptr: ptr = (arr as u64 + i * 8) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            
            if value == target {
                return elem_ptr;
            }
            
            i = i + 1;
        }
    }
    
    return 0 as ptr;  // Not found
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let result: ptr = find_element(&arr, 5, 30);
    
    if result as u64 != 0 {
        return 1;  // Found
    }
    
    return 0;  // Not found
}
```

---

## Common Mistakes

### Mistake 1: Wrong Offset Calculation

```lwanga
// WRONG: Forgetting element size
fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let ptr: ptr = ((&arr) as u64 + 2) as ptr;  // Only 2 bytes!
    
    unsafe {
        let value: u64 = *(ptr as ptr);  // Wrong value!
        return value;
    }
}
```

**Fix:**
```lwanga
fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let ptr: ptr = ((&arr) as u64 + 16) as ptr;  // 2 * 8 bytes
    
    unsafe {
        let value: u64 = *(ptr as ptr);
        return value;  // Returns 30
    }
}
```

### Mistake 2: Out of Bounds Access

```lwanga
// WRONG: No bounds checking
fn main() -> u64 {
    let arr: [u64; 5] = [1, 2, 3, 4, 5];
    let ptr: ptr = ((&arr) as u64 + 100) as ptr;  // Way past end!
    
    unsafe {
        let value: u64 = *(ptr as ptr);  // Undefined behavior!
        return value;
    }
}
```

**Fix:**
```lwanga
fn safe_access(arr: ptr, index: u64, length: u64) -> u64 {
    if index >= length {
        return 0;  // Out of bounds
    }
    
    unsafe {
        let ptr: ptr = (arr as u64 + index * 8) as ptr;
        return *(ptr as ptr);
    }
}
```

---

## Exercises

### Exercise 1: Implement Array Max

Write a function that finds the maximum value in an array using pointers.

<details>
<summary>Solution</summary>

```lwanga
fn array_max(arr: ptr, length: u64) -> u64 {
    if length == 0 {
        return 0;
    }
    
    unsafe {
        let mut max: u64 = *(arr as ptr);
        let mut i: u64 = 1;
        
        while i < length {
            let elem_ptr: ptr = (arr as u64 + i * 8) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            
            if value > max {
                max = value;
            }
            
            i = i + 1;
        }
        
        return max;
    }
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 50, 30, 20, 40];
    let max: u64 = array_max(&arr, 5);
    return max;  // Returns 50
}
```
</details>

### Exercise 2: Pointer Swap

Write a function that swaps two values using pointers.

<details>
<summary>Solution</summary>

```lwanga
fn swap(p1: ptr, p2: ptr) -> u64 {
    unsafe {
        let temp: u64 = *(p1 as ptr);
        *(p1 as ptr) = *(p2 as ptr);
        *(p2 as ptr) = temp;
    }
    return 0;
}

fn main() -> u64 {
    let mut a: u64 = 10;
    let mut b: u64 = 20;
    
    swap(&a, &b);
    
    return a;  // Returns 20
}
```
</details>

### Exercise 3: Count Occurrences

Count how many times a value appears in an array using pointers.

<details>
<summary>Solution</summary>

```lwanga
fn count_occurrences(arr: ptr, length: u64, target: u64) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < length {
            let elem_ptr: ptr = (arr as u64 + i * 8) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            
            if value == target {
                count = count + 1;
            }
            
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    let arr: [u64; 7] = [1, 2, 3, 2, 4, 2, 5];
    let count: u64 = count_occurrences(&arr, 7, 2);
    return count;  // Returns 3
}
```
</details>

### Exercise 4: Array Copy with Overlap Check

Write a safe memory copy that checks for overlapping regions.

<details>
<summary>Solution</summary>

```lwanga
fn safe_memcpy(dest: ptr, src: ptr, n: u64) -> u64 {
    let dest_addr: u64 = dest as u64;
    let src_addr: u64 = src as u64;
    
    // Check for overlap
    if dest_addr < src_addr + n && src_addr < dest_addr + n {
        return 0;  // Overlapping regions!
    }
    
    let mut i: u64 = 0;
    unsafe {
        while i < n {
            let src_byte: ptr = (src_addr + i) as ptr;
            let dest_byte: ptr = (dest_addr + i) as ptr;
            *(dest_byte as ptr) = *(src_byte as ptr);
            i = i + 1;
        }
    }
    
    return 1;  // Success
}

fn main() -> u64 {
    let src: [u8; 5] = [1, 2, 3, 4, 5];
    let mut dest: [u8; 5] = [0, 0, 0, 0, 0];
    
    let result: u64 = safe_memcpy(&dest, &src, 5);
    return result;
}
```
</details>

### Exercise 5: Calculate Pointer Offset

Given two pointers into the same array, calculate how many elements apart they are.

<details>
<summary>Solution</summary>

```lwanga
fn element_distance(p1: ptr, p2: ptr, element_size: u64) -> u64 {
    let addr1: u64 = p1 as u64;
    let addr2: u64 = p2 as u64;
    
    let byte_distance: u64 = if addr1 > addr2 {
        addr1 - addr2
    } else {
        addr2 - addr1
    };
    
    return byte_distance / element_size;
}

fn main() -> u64 {
    let arr: [u64; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let p1: ptr = &arr;
    let p2: ptr = ((&arr) as u64 + 40) as ptr;  // 5 elements away
    
    let distance: u64 = element_distance(p1, p2, 8);
    return distance;  // Returns 5
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Pointer arithmetic: calculating addresses and offsets
- ✅ Multi-level pointers: pointers to pointers
- ✅ Generic memory operations: memcpy, memset, memcmp
- ✅ Pointer comparison and distance calculation
- ✅ Pointer casting and type reinterpretation
- ✅ Practical examples: string operations, array manipulation
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you've mastered pointers, let's learn about unsafe blocks and when to use them in [Lesson 19: Unsafe Blocks](19-unsafe-blocks.md).

---

**Navigation**: [← Previous: Memory Model](17-memory-model.md) | [Tutorial Home](README.md) | [Next: Unsafe Blocks →](19-unsafe-blocks.md)
