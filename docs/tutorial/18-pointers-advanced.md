# Lesson 18: Advanced Pointers

**Master pointer arithmetic, casting, and advanced techniques.**

---

## Pointer Arithmetic

```lwanga
fn main() -> u64 {
    let arr: ptr = [10, 20, 30, 40, 50];
    
    unsafe {
        let p1: ptr = arr;
        let p2: ptr = arr + 2;  // Skip 2 elements
        
        let val1: u64 = *p1;  // 10
        let val2: u64 = *p2;  // 30
        
        return val2;
    }
}
```

---

## Pointer to Pointer

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    let p1: ptr = &x;
    let p2: ptr = &p1;  // Pointer to pointer
    
    return 0;
}
```

---

## Void Pointers (Generic Pointers)

```lwanga
fn copy_memory(dest: ptr, src: ptr, n: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < n {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
    }
    
    return n;
}
```

---

## Pointer Comparison

```lwanga
fn main() -> u64 {
    let arr: ptr = [1, 2, 3];
    let p1: ptr = arr;
    let p2: ptr = arr + 1;
    
    if p1 < p2 {
        return 1;  // p1 is before p2
    }
    
    return 0;
}
```

---

**Navigation**: [← Previous: Memory Model](17-memory-model.md) | [Tutorial Home](README.md) | [Next: Unsafe Blocks →](19-unsafe-blocks.md)
