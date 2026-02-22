# Lesson 36: Debugging

**Learn how to find and fix bugs in your programs.**

---

## Compile with Debug Info

```bash
lwangac program.lwanga -o program -g
```

---

## Using GDB

```bash
gdb ./program

(gdb) break main
(gdb) run
(gdb) step
(gdb) print x
(gdb) continue
(gdb) backtrace
```

---

## Common Debugging Techniques

### Print Debugging

```lwanga
fn debug_print(value: u64) -> u64 {
    // Convert number to string and print
    return 0;
}

fn main() -> u64 {
    let x: u64 = 42;
    debug_print(x);  // Print for debugging
    return x;
}
```

### Return Code Debugging

```lwanga
fn main() -> u64 {
    let result: u64 = some_function();
    
    if result == 0 {
        return 1;  // Error code 1
    }
    
    return 0;  // Success
}
```

---

## Common Bugs

### Null Pointer Dereference

```lwanga
// Bug
let ptr: ptr = 0;
let value: u64 = *ptr;  // CRASH!

// Fix
if ptr != 0 {
    let value: u64 = *ptr;
}
```

### Buffer Overflow

```lwanga
// Bug
let buffer: [u8; 10] = [0; 10];
buffer[20] = 42;  // Out of bounds!

// Fix
if index < 10 {
    buffer[index] = 42;
}
```

---

**Navigation**: [← Previous: Optimization](35-optimization.md) | [Tutorial Home](README.md) | [Next: TCP Server →](37-tcp-server.md)
