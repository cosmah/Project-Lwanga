# Lesson 36: Debugging

**In this lesson, you'll learn essential debugging techniques to find and fix bugs in your Lwanga programs efficiently.**

---

## Why Debugging Matters

Debugging is a critical skill for every programmer:

- **Find bugs faster** - Systematic approach beats guessing
- **Understand code flow** - See what your program actually does
- **Verify assumptions** - Check if code behaves as expected
- **Learn from mistakes** - Understand why bugs happen
- **Build confidence** - Know your code works correctly

**Design choice:** Lwanga supports standard debugging tools like GDB, making it easy to debug with familiar workflows.

---

## Compiling for Debugging

### Debug vs Release Builds

```bash
# Debug build (with debug symbols)
lwangac program.lwanga -o program -g

# Release build (optimized, no debug info)
lwangac program.lwanga -o program -O2

# Debug build with some optimization
lwangac program.lwanga -o program -g -O1
```

**Debug builds include:**
- Symbol information (function names, variables)
- Line number information
- No optimization (easier to follow)
- Larger binary size

**When to use:**
- Development and testing
- Investigating bugs
- Learning how code works

---

## Using GDB (GNU Debugger)

### Basic GDB Commands

```bash
# Start GDB
gdb ./program

# Common commands:
(gdb) break main          # Set breakpoint at main
(gdb) break file.lwanga:42  # Breakpoint at line 42
(gdb) run                 # Start program
(gdb) step                # Execute one line (step into functions)
(gdb) next                # Execute one line (step over functions)
(gdb) continue            # Continue until next breakpoint
(gdb) print x             # Print variable x
(gdb) print *ptr          # Print value at pointer
(gdb) backtrace           # Show call stack
(gdb) info locals         # Show local variables
(gdb) quit                # Exit GDB
```

### Example GDB Session

**Program:**
```lwanga
fn calculate(a: u64, b: u64) -> u64 {
    let sum: u64 = a + b;
    let product: u64 = a * b;
    return sum + product;
}

fn main() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    let result: u64 = calculate(x, y);
    return result;
}
```

**Debug session:**
```bash
$ lwangac program.lwanga -o program -g
$ gdb ./program

(gdb) break main
Breakpoint 1 at 0x401000: file program.lwanga, line 7.

(gdb) run
Starting program: ./program
Breakpoint 1, main () at program.lwanga:7

(gdb) print x
$1 = 5

(gdb) print y
$2 = 10

(gdb) step
calculate (a=5, b=10) at program.lwanga:2

(gdb) print sum
$3 = 15

(gdb) print product
$4 = 50

(gdb) continue
Program exited with code 65.
```

---

## Print Debugging

### Simple Print Function

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn debug_print_string(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    return 0;
}

fn debug_print_number(n: u64) -> u64 {
    let mut buffer: [u8; 20] = [0; 20];
    let mut i: u64 = 0;
    let mut num: u64 = n;
    
    if num == 0 {
        buffer[0] = 48;  // '0'
        debug_print_string(&buffer as ptr, 1);
        return 0;
    }
    
    // Convert to string (reversed)
    while num > 0 {
        buffer[i] = (num % 10) as u8 + 48;
        num = num / 10;
        i = i + 1;
    }
    
    // Reverse
    let mut j: u64 = 0;
    while j < i / 2 {
        let temp: u8 = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
        j = j + 1;
    }
    
    debug_print_string(&buffer as ptr, i);
    return 0;
}

fn main() -> u64 {
    debug_print_string("Starting program\n", 17);
    
    let x: u64 = 42;
    debug_print_string("x = ", 4);
    debug_print_number(x);
    debug_print_string("\n", 1);
    
    debug_print_string("Program complete\n", 17);
    
    return 0;
}
```

### Debug Macros with Preprocessor

```lwanga
#if DEBUG
fn debug_log(msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    unsafe {
        syscall(SYS_WRITE, 1, msg, len);
    }
    return 0;
}
#else
fn debug_log(msg: ptr, len: u64) -> u64 {
    return 0;  // No-op in release
}
#endif

fn process_data(data: u64) -> u64 {
    debug_log("Processing data\n", 16);
    
    let result: u64 = data * 2;
    
    debug_log("Processing complete\n", 20);
    
    return result;
}

fn main() -> u64 {
    let result: u64 = process_data(21);
    return result;
}
```

---

## Return Code Debugging

### Using Exit Codes

```lwanga
fn validate_input(value: u64) -> u64 {
    if value == 0 {
        return 1;  // Error: zero value
    }
    
    if value > 1000 {
        return 2;  // Error: value too large
    }
    
    return 0;  // Success
}

fn main() -> u64 {
    let input: u64 = 42;
    let result: u64 = validate_input(input);
    
    if result == 1 {
        return 10;  // Exit code 10: zero value error
    }
    
    if result == 2 {
        return 20;  // Exit code 20: value too large error
    }
    
    return 0;  // Success
}
```

**Check exit code:**
```bash
$ ./program
$ echo $?
0
```

### Error Code Constants

```lwanga
const ERR_SUCCESS: u64 = 0;
const ERR_INVALID_INPUT: u64 = 1;
const ERR_FILE_NOT_FOUND: u64 = 2;
const ERR_PERMISSION_DENIED: u64 = 3;
const ERR_OUT_OF_MEMORY: u64 = 4;

fn open_file(path: ptr) -> u64 {
    const SYS_OPEN: u64 = 2;
    const O_RDONLY: u32 = 0;
    
    let fd: u64 = 0;
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return ERR_FILE_NOT_FOUND;
    }
    
    return ERR_SUCCESS;
}

fn main() -> u64 {
    let result: u64 = open_file("/tmp/test.txt");
    
    if result != ERR_SUCCESS {
        return result;  // Return error code
    }
    
    return ERR_SUCCESS;
}
```

---

## Common Bugs and Fixes

### Bug 1: Null Pointer Dereference

**Problem:**
```lwanga
fn buggy_function(ptr: ptr) -> u64 {
    // CRASH if ptr is null!
    let value: u64 = 0;
    unsafe {
        value = *ptr;
    }
    return value;
}

fn main() -> u64 {
    let null_ptr: ptr = 0;
    let result: u64 = buggy_function(null_ptr);  // CRASH!
    return result;
}
```

**Fix:**
```lwanga
fn safe_function(ptr: ptr) -> u64 {
    // Check for null first
    if ptr == 0 {
        return 0;  // Or handle error appropriately
    }
    
    let value: u64 = 0;
    unsafe {
        value = *ptr;
    }
    return value;
}

fn main() -> u64 {
    let null_ptr: ptr = 0;
    let result: u64 = safe_function(null_ptr);  // Safe
    return result;
}
```

### Bug 2: Buffer Overflow

**Problem:**
```lwanga
fn buggy_copy(dest: ptr, src: ptr, len: u64) -> u64 {
    let buffer: [u8; 10] = [0; 10];
    
    // CRASH if len > 10!
    let mut i: u64 = 0;
    unsafe {
        while i < len {
            buffer[i] = *(src + i);
            i = i + 1;
        }
    }
    
    return 0;
}
```

**Fix:**
```lwanga
fn safe_copy(dest: ptr, src: ptr, len: u64) -> u64 {
    let buffer: [u8; 10] = [0; 10];
    let buffer_size: u64 = 10;
    
    // Limit to buffer size
    let copy_len: u64 = if len > buffer_size {
        buffer_size
    } else {
        len
    };
    
    let mut i: u64 = 0;
    unsafe {
        while i < copy_len {
            buffer[i] = *(src + i);
            i = i + 1;
        }
    }
    
    return 0;
}
```

### Bug 3: Off-by-One Error

**Problem:**
```lwanga
fn buggy_loop(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    // BUG: i <= len accesses one past end!
    unsafe {
        while i <= len {
            *(arr + i * 8) = 0;
            i = i + 1;
        }
    }
    
    return 0;
}
```

**Fix:**
```lwanga
fn fixed_loop(arr: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    // CORRECT: i < len
    unsafe {
        while i < len {
            *(arr + i * 8) = 0;
            i = i + 1;
        }
    }
    
    return 0;
}
```

### Bug 4: Integer Overflow

**Problem:**
```lwanga
fn buggy_add(a: u8, b: u8) -> u8 {
    // BUG: Can overflow!
    return a + b;  // 200 + 100 = 44 (wraps around)
}

fn main() -> u64 {
    let result: u8 = buggy_add(200, 100);
    return result as u64;  // Returns 44, not 300!
}
```

**Fix:**
```lwanga
fn safe_add(a: u8, b: u8) -> u64 {
    // Use larger type to prevent overflow
    let result: u64 = a as u64 + b as u64;
    return result;
}

fn main() -> u64 {
    let result: u64 = safe_add(200, 100);
    return result;  // Returns 300
}
```

### Bug 5: Uninitialized Variable

**Problem:**
```lwanga
fn buggy_function() -> u64 {
    let x: u64;  // Uninitialized!
    
    if some_condition() == 1 {
        x = 10;
    }
    
    // BUG: x might not be initialized!
    return x;
}
```

**Fix:**
```lwanga
fn fixed_function() -> u64 {
    let mut x: u64 = 0;  // Initialize to default
    
    if some_condition() == 1 {
        x = 10;
    }
    
    return x;  // Always initialized
}
```

---

## Debugging Strategies

### Strategy 1: Binary Search

When you have a large program with a bug:

1. Comment out half the code
2. If bug persists, it's in the remaining half
3. If bug disappears, it's in the commented half
4. Repeat until you find the bug

### Strategy 2: Rubber Duck Debugging

Explain your code line-by-line to someone (or a rubber duck):
- Forces you to think through logic
- Often reveals the bug while explaining
- No actual duck required!

### Strategy 3: Minimal Reproduction

Create the smallest program that shows the bug:
- Easier to understand
- Easier to debug
- Easier to ask for help

**Example:**
```lwanga
// Original: 500 lines with bug somewhere

// Minimal reproduction: 20 lines
fn main() -> u64 {
    let x: u64 = buggy_calculation(42);
    return x;  // Wrong result
}
```

### Strategy 4: Add Assertions

```lwanga
#if DEBUG
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    if condition == 0 {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, msg, len);
            syscall(60, 1, 0, 0);  // Exit with error
        }
    }
    return 0;
}
#else
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    return 0;
}
#endif

fn divide(a: u64, b: u64) -> u64 {
    assert(b != 0, "Division by zero!\n", 19);
    return a / b;
}

fn main() -> u64 {
    let result: u64 = divide(10, 0);  // Assertion fails
    return result;
}
```

---

## Practical Examples

### Example 1: Debug Logger

```lwanga
const SYS_WRITE: u64 = 1;
const STDERR: u64 = 2;

#if DEBUG
const DEBUG_ENABLED: u64 = 1;
#else
const DEBUG_ENABLED: u64 = 0;
#endif

fn log_debug(msg: ptr, len: u64) -> u64 {
    if DEBUG_ENABLED == 1 {
        unsafe {
            syscall(SYS_WRITE, STDERR, "[DEBUG] ", 8);
            syscall(SYS_WRITE, STDERR, msg, len);
            syscall(SYS_WRITE, STDERR, "\n", 1);
        }
    }
    return 0;
}

fn log_error(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDERR, "[ERROR] ", 8);
        syscall(SYS_WRITE, STDERR, msg, len);
        syscall(SYS_WRITE, STDERR, "\n", 1);
    }
    return 0;
}

fn process_data(value: u64) -> u64 {
    log_debug("Entering process_data", 22);
    
    if value == 0 {
        log_error("Invalid value: zero", 19);
        return 1;
    }
    
    let result: u64 = value * 2;
    log_debug("Processing complete", 19);
    
    return result;
}

fn main() -> u64 {
    log_debug("Program started", 15);
    
    let result: u64 = process_data(42);
    
    log_debug("Program complete", 16);
    
    return 0;
}
```

### Example 2: Memory Debugger

```lwanga
#if DEBUG
fn check_pointer(ptr: ptr, name: ptr, name_len: u64) -> u64 {
    if ptr == 0 {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, "NULL POINTER: ", 14);
            syscall(SYS_WRITE, 2, name, name_len);
            syscall(SYS_WRITE, 2, "\n", 1);
        }
        return 1;
    }
    return 0;
}

fn check_bounds(index: u64, size: u64, name: ptr, name_len: u64) -> u64 {
    if index >= size {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, "OUT OF BOUNDS: ", 15);
            syscall(SYS_WRITE, 2, name, name_len);
            syscall(SYS_WRITE, 2, "\n", 1);
        }
        return 1;
    }
    return 0;
}
#else
fn check_pointer(ptr: ptr, name: ptr, name_len: u64) -> u64 {
    return 0;
}

fn check_bounds(index: u64, size: u64, name: ptr, name_len: u64) -> u64 {
    return 0;
}
#endif

fn safe_access(arr: ptr, index: u64, size: u64) -> u64 {
    check_pointer(arr, "array", 5);
    check_bounds(index, size, "array index", 11);
    
    let value: u64 = 0;
    unsafe {
        value = *(arr + index * 8);
    }
    
    return value;
}

fn main() -> u64 {
    let array: [u64; 10] = [0; 10];
    let value: u64 = safe_access(&array as ptr, 5, 10);
    return value;
}
```

---

## Common Mistakes

### Mistake 1: Not Checking Return Values

```lwanga
// WRONG: Ignoring errors
fn buggy_code() -> u64 {
    let fd: u64 = open_file("/tmp/test.txt");
    // What if open failed?
    write_to_file(fd, "data", 4);  // Crash!
    return 0;
}
```

**Fix:** Always check return values.

### Mistake 2: Debugging Optimized Code

```lwanga
// Compiled with -O3
// Variables might be optimized away
// Code might be reordered
// Hard to follow in debugger
```

**Fix:** Use `-g -O0` for debugging.

### Mistake 3: Forgetting to Remove Debug Code

```lwanga
fn production_code() -> u64 {
    debug_print("TODO: remove this", 18);  // Oops!
    return 0;
}
```

**Fix:** Use preprocessor conditionals.

---

## Exercises

### Exercise 1: Add Debug Logging

Add debug logging to this function:

```lwanga
fn calculate(a: u64, b: u64) -> u64 {
    let sum: u64 = a + b;
    let product: u64 = a * b;
    return sum + product;
}
```

<details>
<summary>Solution</summary>

```lwanga
#if DEBUG
fn debug_log(msg: ptr, len: u64) -> u64 {
    const SYS_WRITE: u64 = 1;
    unsafe {
        syscall(SYS_WRITE, 2, msg, len);
    }
    return 0;
}
#else
fn debug_log(msg: ptr, len: u64) -> u64 {
    return 0;
}
#endif

fn calculate(a: u64, b: u64) -> u64 {
    debug_log("Entering calculate\n", 19);
    
    let sum: u64 = a + b;
    debug_log("Calculated sum\n", 15);
    
    let product: u64 = a * b;
    debug_log("Calculated product\n", 19);
    
    let result: u64 = sum + product;
    debug_log("Returning result\n", 17);
    
    return result;
}
```
</details>

### Exercise 2: Fix the Bug

Find and fix the bug in this code:

```lwanga
fn copy_string(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i <= max_len {
            *(dest + i) = *(src + i);
            if *(src + i) == 0 {
                break;
            }
            i = i + 1;
        }
    }
    
    return i;
}
```

<details>
<summary>Solution</summary>

```lwanga
// Bug: i <= max_len should be i < max_len
// This causes an off-by-one error

fn copy_string_fixed(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < max_len {  // Fixed: < instead of <=
            *(dest + i) = *(src + i);
            if *(src + i) == 0 {
                break;
            }
            i = i + 1;
        }
    }
    
    return i;
}
```
</details>

### Exercise 3: Add Assertions

Add assertions to validate inputs:

```lwanga
fn divide_and_multiply(a: u64, b: u64, c: u64) -> u64 {
    let result: u64 = (a / b) * c;
    return result;
}
```

<details>
<summary>Solution</summary>

```lwanga
#if DEBUG
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    if condition == 0 {
        const SYS_WRITE: u64 = 1;
        unsafe {
            syscall(SYS_WRITE, 2, msg, len);
            syscall(60, 1, 0, 0);
        }
    }
    return 0;
}
#else
fn assert(condition: u64, msg: ptr, len: u64) -> u64 {
    return 0;
}
#endif

fn divide_and_multiply(a: u64, b: u64, c: u64) -> u64 {
    assert(b != 0, "Division by zero!\n", 19);
    assert(a < 1000000, "Value too large!\n", 17);
    
    let result: u64 = (a / b) * c;
    
    assert(result >= a / b, "Overflow detected!\n", 19);
    
    return result;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to compile with debug information
- ✅ Using GDB to debug programs
- ✅ Print debugging techniques
- ✅ Return code debugging
- ✅ Common bugs and how to fix them
- ✅ Debugging strategies
- ✅ Creating debug logging systems
- ✅ Using assertions effectively

**Remember:** Good debugging skills save hours of frustration. Learn to use tools effectively and develop systematic debugging habits.

---

## Next Lesson

Now that you know how to debug, let's build a complete TCP server project in [Lesson 37: TCP Server](37-tcp-server.md).

---

**Navigation**: [← Previous: Optimization](35-optimization.md) | [Tutorial Home](README.md) | [Next: TCP Server →](37-tcp-server.md)
