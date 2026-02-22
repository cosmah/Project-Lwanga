# Lesson 8: Loops

**Learn how to repeat code using while loops.**

---

## While Loops

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    while counter < 10 {
        counter = counter + 1;
    }
    
    return counter;  // Returns 10
}
```

---

## Loop with Condition

```lwanga
fn main() -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while i <= 100 {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;  // Sum of 1 to 100
}
```

---

## Breaking Out of Loops

```lwanga
fn find_value(target: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < 1000 {
        if i == target {
            return i;  // Found it, exit loop
        }
        i = i + 1;
    }
    
    return 0;  // Not found
}
```

---

## Nested Loops

```lwanga
fn main() -> u64 {
    let mut i: u64 = 0;
    let mut count: u64 = 0;
    
    while i < 10 {
        let mut j: u64 = 0;
        while j < 10 {
            count = count + 1;
            j = j + 1;
        }
        i = i + 1;
    }
    
    return count;  // Returns 100
}
```

---

## Infinite Loops (Be Careful!)

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    while 1 == 1 {  // Always true
        counter = counter + 1;
        
        if counter > 100 {
            return counter;  // Must have exit condition!
        }
    }
}
```

---

**Navigation**: [← Previous: Conditionals](07-conditionals.md) | [Tutorial Home](README.md) | [Next: Operators →](09-operators.md)
