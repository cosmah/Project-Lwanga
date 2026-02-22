# Lesson 12: Arrays

**In this lesson, you'll learn how to work with arrays, access elements, iterate through collections, and understand array memory layout.**

---

## What are Arrays?

Arrays are collections of elements of the same type stored in contiguous memory. They allow you to work with multiple values efficiently.

**Why use arrays?**
- Store multiple related values
- Process collections of data
- Efficient memory access
- Foundation for data structures

---

## Array Literals

### Creating Arrays

```lwanga
fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    let bytes: ptr = [0x48, 0x65, 0x6c, 0x6c, 0x6f];
    let flags: ptr = [1, 0, 1, 1, 0];
    
    return 0;
}
```

**Memory layout:**
```
numbers -> [1][2][3][4][5]
           ^
           |
        Points to first element
```

**Design choice:** Array literals in Lwanga create a pointer to the first element. This is simple and efficient.

---

## Accessing Array Elements

### Basic Access

```lwanga
fn main() -> u64 {
    let arr: ptr = [10, 20, 30, 40, 50];
    
    unsafe {
        let first: u64 = *arr;          // 10
        let second: u64 = *(arr + 1);   // 20
        let third: u64 = *(arr + 2);    // 30
        let fourth: u64 = *(arr + 3);   // 40
        let fifth: u64 = *(arr + 5);    // 50
        
        return first;
    }
}
```

**How it works:**
- `arr` points to first element
- `arr + 1` points to second element
- `arr + 2` points to third element
- And so on...

### Access by Index

```lwanga
fn array_get(arr: ptr, index: u64) -> u64 {
    unsafe {
        return *(arr + index);
    }
}

fn main() -> u64 {
    let numbers: ptr = [5, 10, 15, 20, 25];
    
    let val0: u64 = array_get(numbers, 0);  // 5
    let val2: u64 = array_get(numbers, 2);  // 15
    let val4: u64 = array_get(numbers, 4);  // 25
    
    return val2;
}
```

---

## Modifying Array Elements

### Basic Modification

```lwanga
fn main() -> u64 {
    let arr: ptr = [1, 2, 3, 4, 5];
    
    unsafe {
        *(arr + 0) = 10;  // Change first element to 10
        *(arr + 2) = 30;  // Change third element to 30
        *(arr + 4) = 50;  // Change fifth element to 50
    }
    
    // Array is now [10, 2, 30, 4, 50]
    
    unsafe {
        return *arr;  // Returns 10
    }
}
```

### Set by Index

```lwanga
fn array_set(arr: ptr, index: u64, value: u64) -> u64 {
    unsafe {
        *(arr + index) = value;
    }
    return value;
}

fn main() -> u64 {
    let numbers: ptr = [0, 0, 0, 0, 0];
    
    array_set(numbers, 0, 10);
    array_set(numbers, 1, 20);
    array_set(numbers, 2, 30);
    
    // Array is now [10, 20, 30, 0, 0]
    
    return 0;
}
```

---

## Iterating Over Arrays

### Basic Iteration

```lwanga
fn print_array(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < len {
        unsafe {
            let value: u64 = *(arr + i);
            // Print value here
        }
        i = i + 1;
    }
    
    return len;
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    print_array(numbers, 5);
    
    return 0;
}
```

### Sum Array Elements

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

### Find Maximum

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

### Find Minimum

```lwanga
fn array_min(arr: ptr, len: u64) -> u64 {
    if len == 0 {
        return 0;
    }
    
    let mut min: u64 = 0;
    unsafe {
        min = *arr;
    }
    
    let mut i: u64 = 1;
    
    unsafe {
        while i < len {
            let val: u64 = *(arr + i);
            if val < min {
                min = val;
            }
            i = i + 1;
        }
    }
    
    return min;
}

fn main() -> u64 {
    let numbers: ptr = [5, 2, 9, 1, 7];
    return array_min(numbers, 5);  // Returns 1
}
```

---

## Array Operations

### Copy Array

```lwanga
fn array_copy(dest: ptr, src: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let source: ptr = [1, 2, 3, 4, 5];
    let dest: [u64; 5] = [0; 5];
    
    array_copy(&dest, source, 5);
    // dest is now [1, 2, 3, 4, 5]
    
    return 0;
}
```

### Reverse Array

```lwanga
fn array_reverse(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len / 2 {
            let temp: u64 = *(arr + i);
            *(arr + i) = *(arr + len - i - 1);
            *(arr + len - i - 1) = temp;
            i = i + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    array_reverse(numbers, 5);
    // numbers is now [5, 4, 3, 2, 1]
    
    return 0;
}
```

### Fill Array

```lwanga
fn array_fill(arr: ptr, len: u64, value: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            *(arr + i) = value;
            i = i + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let numbers: [u64; 10] = [0; 10];
    array_fill(&numbers, 10, 42);
    // All elements are now 42
    
    return 0;
}
```

---

## Searching Arrays

### Linear Search

```lwanga
fn array_find(arr: ptr, len: u64, value: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(arr + i) == value {
                return i;  // Found at index i
            }
            i = i + 1;
        }
    }
    
    return len;  // Not found (return invalid index)
}

fn main() -> u64 {
    let numbers: ptr = [10, 20, 30, 40, 50];
    let index: u64 = array_find(numbers, 5, 30);
    
    return index;  // Returns 2
}
```

### Contains Check

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

### Count Occurrences

```lwanga
fn array_count(arr: ptr, len: u64, value: u64) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(arr + i) == value {
                count = count + 1;
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 2, 1, 2];
    return array_count(numbers, 6, 2);  // Returns 3
}
```

---

## Byte Arrays

### Working with Bytes

```lwanga
fn main() -> u64 {
    // Array of bytes (u8)
    let data: ptr = [0xFF, 0xFE, 0xFD, 0xFC];
    
    unsafe {
        let first_byte: u8 = *data;
        let second_byte: u8 = *(data + 1);
        
        return first_byte as u64;  // Returns 255
    }
}
```

### Byte Array Operations

```lwanga
fn bytes_to_u64(bytes: ptr) -> u64 {
    let mut result: u64 = 0;
    
    unsafe {
        result = result | (*(bytes + 0) as u64);
        result = result | ((*(bytes + 1) as u64) << 8);
        result = result | ((*(bytes + 2) as u64) << 16);
        result = result | ((*(bytes + 3) as u64) << 24);
    }
    
    return result;
}

fn main() -> u64 {
    let bytes: ptr = [0x01, 0x02, 0x03, 0x04];
    return bytes_to_u64(bytes);  // Returns 0x04030201
}
```

---

## Multi-Dimensional Arrays

### 2D Array Simulation

```lwanga
fn get_2d_element(arr: ptr, row: u64, col: u64, cols: u64) -> u64 {
    let index: u64 = row * cols + col;
    unsafe {
        return *(arr + index);
    }
}

fn set_2d_element(arr: ptr, row: u64, col: u64, cols: u64, value: u64) -> u64 {
    let index: u64 = row * cols + col;
    unsafe {
        *(arr + index) = value;
    }
    return value;
}

fn main() -> u64 {
    // 3x3 matrix stored as 1D array
    let matrix: ptr = [
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    ];
    
    let val: u64 = get_2d_element(matrix, 1, 1, 3);  // Gets 5
    
    return val;
}
```

---

## Practical Examples

### Example 1: Calculate Average

```lwanga
fn array_average(arr: ptr, len: u64) -> u64 {
    if len == 0 {
        return 0;
    }
    
    let sum: u64 = array_sum(arr, len);
    return sum / len;
}

fn main() -> u64 {
    let numbers: ptr = [10, 20, 30, 40, 50];
    return array_average(numbers, 5);  // Returns 30
}
```

### Example 2: Remove Duplicates

```lwanga
fn array_remove_duplicates(arr: ptr, len: u64) -> u64 {
    if len == 0 {
        return 0;
    }
    
    let mut new_len: u64 = 1;
    let mut i: u64 = 1;
    
    unsafe {
        while i < len {
            let mut is_duplicate: u64 = 0;
            let mut j: u64 = 0;
            
            while j < new_len {
                if *(arr + i) == *(arr + j) {
                    is_duplicate = 1;
                    break;
                }
                j = j + 1;
            }
            
            if is_duplicate == 0 {
                *(arr + new_len) = *(arr + i);
                new_len = new_len + 1;
            }
            
            i = i + 1;
        }
    }
    
    return new_len;
}
```

### Example 3: Bubble Sort

```lwanga
fn bubble_sort(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < len {
        let mut j: u64 = 0;
        
        unsafe {
            while j < len - i - 1 {
                if *(arr + j) > *(arr + j + 1) {
                    let temp: u64 = *(arr + j);
                    *(arr + j) = *(arr + j + 1);
                    *(arr + j + 1) = temp;
                }
                j = j + 1;
            }
        }
        
        i = i + 1;
    }
    
    return len;
}

fn main() -> u64 {
    let numbers: ptr = [5, 2, 8, 1, 9];
    bubble_sort(numbers, 5);
    // numbers is now [1, 2, 5, 8, 9]
    
    return 0;
}
```

### Example 4: Binary Search (Sorted Array)

```lwanga
fn binary_search(arr: ptr, len: u64, target: u64) -> u64 {
    let mut left: u64 = 0;
    let mut right: u64 = len;
    
    unsafe {
        while left < right {
            let mid: u64 = (left + right) / 2;
            let mid_val: u64 = *(arr + mid);
            
            if mid_val == target {
                return mid;  // Found
            }
            
            if mid_val < target {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
    }
    
    return len;  // Not found
}

fn main() -> u64 {
    let numbers: ptr = [1, 3, 5, 7, 9, 11, 13];
    return binary_search(numbers, 7, 7);  // Returns 3
}
```

### Example 5: Rotate Array

```lwanga
fn array_rotate_left(arr: ptr, len: u64, positions: u64) -> u64 {
    let rotate: u64 = positions % len;
    let mut i: u64 = 0;
    
    while i < rotate {
        unsafe {
            let first: u64 = *arr;
            let mut j: u64 = 0;
            
            while j < len - 1 {
                *(arr + j) = *(arr + j + 1);
                j = j + 1;
            }
            
            *(arr + len - 1) = first;
        }
        i = i + 1;
    }
    
    return len;
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    array_rotate_left(numbers, 5, 2);
    // numbers is now [3, 4, 5, 1, 2]
    
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Out of Bounds Access

```lwanga
fn main() -> u64 {
    let arr: ptr = [1, 2, 3];
    
    unsafe {
        let val: u64 = *(arr + 10);  // ERROR: Out of bounds!
    }
    
    return val;
}
```

**Fix:** Always check bounds
```lwanga
fn safe_get(arr: ptr, index: u64, len: u64) -> u64 {
    if index >= len {
        return 0;  // Error value
    }
    
    unsafe {
        return *(arr + index);
    }
}
```

### Mistake 2: Forgetting Array Length

```lwanga
// Bad: No way to know array length
fn process(arr: ptr) -> u64 {
    // How many elements?
    return 0;
}

// Good: Pass length
fn process(arr: ptr, len: u64) -> u64 {
    // Now we know the length
    return len;
}
```

### Mistake 3: Modifying During Iteration

```lwanga
// Dangerous pattern
fn remove_element(arr: ptr, len: u64, index: u64) -> u64 {
    let mut i: u64 = index;
    
    unsafe {
        while i < len - 1 {
            *(arr + i) = *(arr + i + 1);
            i = i + 1;
        }
    }
    
    return len - 1;
}
```

---

## Exercises

### Exercise 1: Array Product

Write a function that returns the product of all elements in an array.

<details>
<summary>Solution</summary>

```lwanga
fn array_product(arr: ptr, len: u64) -> u64 {
    let mut product: u64 = 1;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            product = product * *(arr + i);
            i = i + 1;
        }
    }
    
    return product;
}

fn main() -> u64 {
    let numbers: ptr = [2, 3, 4];
    return array_product(numbers, 3);  // Returns 24
}
```
</details>

### Exercise 2: Find Second Largest

Write a function that finds the second largest element in an array.

<details>
<summary>Solution</summary>

```lwanga
fn array_second_max(arr: ptr, len: u64) -> u64 {
    if len < 2 {
        return 0;
    }
    
    let mut max: u64 = 0;
    let mut second_max: u64 = 0;
    
    unsafe {
        max = *arr;
        second_max = *arr;
        
        let mut i: u64 = 1;
        while i < len {
            let val: u64 = *(arr + i);
            
            if val > max {
                second_max = max;
                max = val;
            } else {
                if val > second_max && val != max {
                    second_max = val;
                }
            }
            
            i = i + 1;
        }
    }
    
    return second_max;
}

fn main() -> u64 {
    let numbers: ptr = [5, 2, 9, 1, 7];
    return array_second_max(numbers, 5);  // Returns 7
}
```
</details>

### Exercise 3: Check if Sorted

Write a function that checks if an array is sorted in ascending order.

<details>
<summary>Solution</summary>

```lwanga
fn is_sorted(arr: ptr, len: u64) -> u64 {
    if len <= 1 {
        return 1;
    }
    
    let mut i: u64 = 0;
    
    unsafe {
        while i < len - 1 {
            if *(arr + i) > *(arr + i + 1) {
                return 0;  // Not sorted
            }
            i = i + 1;
        }
    }
    
    return 1;  // Sorted
}

fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    return is_sorted(numbers, 5);  // Returns 1
}
```
</details>

### Exercise 4: Array Intersection

Write a function that counts common elements between two arrays.

<details>
<summary>Solution</summary>

```lwanga
fn array_intersection_count(arr1: ptr, len1: u64, arr2: ptr, len2: u64) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < len1 {
            let mut j: u64 = 0;
            while j < len2 {
                if *(arr1 + i) == *(arr2 + j) {
                    count = count + 1;
                    break;
                }
                j = j + 1;
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    let arr1: ptr = [1, 2, 3, 4];
    let arr2: ptr = [3, 4, 5, 6];
    return array_intersection_count(arr1, 4, arr2, 4);  // Returns 2
}
```
</details>

### Exercise 5: Partition Array

Write a function that partitions an array around a pivot (all elements less than pivot come before it).

<details>
<summary>Solution</summary>

```lwanga
fn partition(arr: ptr, len: u64, pivot: u64) -> u64 {
    let mut left: u64 = 0;
    let mut right: u64 = len - 1;
    
    unsafe {
        while left < right {
            while left < len && *(arr + left) < pivot {
                left = left + 1;
            }
            
            while right > 0 && *(arr + right) >= pivot {
                right = right - 1;
            }
            
            if left < right {
                let temp: u64 = *(arr + left);
                *(arr + left) = *(arr + right);
                *(arr + right) = temp;
            }
        }
    }
    
    return left;
}

fn main() -> u64 {
    let numbers: ptr = [5, 2, 8, 1, 9, 3];
    let pivot_index: u64 = partition(numbers, 6, 5);
    return pivot_index;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to create and use array literals
- ✅ Accessing and modifying array elements
- ✅ Iterating through arrays
- ✅ Common array operations (sum, max, min, reverse)
- ✅ Searching arrays (linear search, binary search)
- ✅ Sorting arrays (bubble sort)
- ✅ Working with byte arrays
- ✅ Simulating multi-dimensional arrays
- ✅ Safe array access with bounds checking

---

## Next Lesson

Now that you understand arrays, let's learn about structs to create custom data types in [Lesson 13: Structs](13-structs.md).

---

**Navigation**: [← Previous: Strings](11-strings.md) | [Tutorial Home](README.md) | [Next: Structs →](13-structs.md)
