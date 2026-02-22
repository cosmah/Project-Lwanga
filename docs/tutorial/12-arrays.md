# Lesson 12: Arrays

**Learn how to work with arrays and collections of data.**

---

## Array Literals

```lwanga
fn main() -> u64 {
    let numbers: ptr = [1, 2, 3, 4, 5];
    let bytes: ptr = [0x48, 0x65, 0x6c, 0x6c, 0x6f];
    
    return 0;
}
```

---

## Accessing Array Elements

```lwanga
fn main() -> u64 {
    let arr: ptr = [10, 20, 30, 40, 50];
    
    unsafe {
        let first: u64 = *arr;          // 10
        let second: u64 = *(arr + 1);   // 20
        let third: u64 = *(arr + 2);    // 30
        
        return first;
    }
}
```

---

## Iterating Over Arrays

```lwanga
fn sum_array(arr: ptr, len: u64) -> u64 {
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
    return sum_array(numbers, 5);  // Returns 15
}
```

---

## Modifying Array Elements

```lwanga
fn main() -> u64 {
    let arr: ptr = [1, 2, 3];
    
    unsafe {
        *(arr + 1) = 99;  // Change second element
        return *(arr + 1);  // Returns 99
    }
}
```

---

## Byte Arrays

```lwanga
fn main() -> u64 {
    // Array of bytes (u8)
    let data: ptr = [0xFF, 0xFE, 0xFD, 0xFC];
    
    unsafe {
        let first_byte: u8 = *data;
        return first_byte as u64;  // Returns 255
    }
}
```

---

**Navigation**: [← Previous: Strings](11-strings.md) | [Tutorial Home](README.md) | [Next: Structs →](13-structs.md)
