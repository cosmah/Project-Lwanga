# Lesson 39: Memory Dumper

**In this lesson, you'll build a process memory dumper to inspect and analyze memory contents, learning about process memory layout and forensic techniques.**

---

## Project Overview

We'll build a memory dumper that:

- **Reads process memory** - Access memory regions
- **Dumps memory regions** - Extract memory contents
- **Formats output** - Display as hex dump
- **Handles errors** - Deal with access violations
- **Analyzes data** - Search for patterns

**Why build a memory dumper?**
- Essential for debugging and forensics
- Understand process memory layout
- Learn memory access techniques
- Analyze running processes
- Detect malware and rootkits

**Design choice:** We'll use `/proc/self/mem` to read our own process memory, demonstrating memory introspection without requiring special privileges.

---

## Understanding Process Memory

### Memory Layout

A typical process memory layout:

```
High addresses
┌─────────────────┐
│  Kernel Space   │  (not accessible)
├─────────────────┤
│     Stack       │  ← grows down
│       ↓         │
│                 │
│   (unmapped)    │
│                 │
│       ↑         │
│     Heap        │  ← grows up
├─────────────────┤
│      BSS        │  (uninitialized data)
├─────────────────┤
│     Data        │  (initialized data)
├─────────────────┤
│     Text        │  (code)
└─────────────────┘
Low addresses
```

**Sections explained:**
- **Text** - Executable code (read-only)
- **Data** - Initialized global variables
- **BSS** - Uninitialized global variables
- **Heap** - Dynamic allocations (malloc, etc.)
- **Stack** - Local variables, function calls

### /proc/self/mem

Linux provides `/proc/self/mem` to access process memory:
- Special file representing process memory
- Can read/write (with proper permissions)
- Requires seeking to specific addresses
- Useful for introspection and debugging

---

## System Call Constants

```lwanga
const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;
const SYS_LSEEK: u64 = 8;

const O_RDONLY: u32 = 0;
const STDOUT: u64 = 1;
```

**Syscalls used:**
- `open()` - Open /proc/self/mem
- `lseek()` - Seek to memory address
- `read()` - Read memory contents
- `write()` - Output to stdout
- `close()` - Close file

---

## Hex Dump Functions

### print_hex_byte() - Display Byte as Hex

```lwanga
fn print_hex_byte(byte: u8) -> u64 {
    let hex_chars: ptr = "0123456789abcdef";
    let high: u8 = (byte >> 4) & 0x0F;
    let low: u8 = byte & 0x0F;
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, hex_chars + high, 1);
        syscall(SYS_WRITE, STDOUT, hex_chars + low, 1);
    }
    
    return 0;
}
```

**How it works:**
1. Split byte into high and low nibbles
2. High nibble: shift right 4 bits
3. Low nibble: mask with 0x0F
4. Use nibble as index into hex character string
5. Print both characters

**Example:**
- Byte: 0xA5
- High: 0xA (10) → 'a'
- Low: 0x5 (5) → '5'
- Output: "a5"

---

## Core Memory Dumping

### dump_memory() - Read and Display Memory

```lwanga
fn dump_memory(addr: u64, size: u64) -> u64 {
    // Open /proc/self/mem
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        let msg: ptr = "Failed to open memory\n";
        unsafe {
            syscall(SYS_WRITE, STDOUT, msg, 22);
        }
        return 1;
    }
    
    // Seek to address
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    // Read memory
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    // Print hex dump
    let mut i: u64 = 0;
    while i < bytes_read {
        if i % 16 == 0 {
            let newline: ptr = "\n";
            unsafe {
                syscall(SYS_WRITE, STDOUT, newline, 1);
            }
        }
        
        print_hex_byte(buffer[i]);
        
        let space: ptr = " ";
        unsafe {
            syscall(SYS_WRITE, STDOUT, space, 1);
        }
        
        i = i + 1;
    }
    
    let newline: ptr = "\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, newline, 1);
    }
    
    // Close file
    unsafe {
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return 0;
}
```

**Process:**
1. Open `/proc/self/mem` for reading
2. Seek to target memory address
3. Read memory into buffer
4. Format as hex dump (16 bytes per line)
5. Close file descriptor

---

## Complete Memory Dumper

```lwanga
fn main() -> u64 {
    let msg: ptr = "Memory Dumper\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 14);
    }
    
    // Dump some memory (stack)
    let stack_var: u64 = 0x4142434445464748;
    let addr: u64 = &stack_var as u64;
    
    dump_memory(addr, 64);
    
    return 0;
}
```

**Output example:**
```
Memory Dumper

48 47 46 45 44 43 42 41 00 00 00 00 00 00 00 00
...
```

---

## Enhanced Features

### Feature 1: Address Display

```lwanga
fn print_address(addr: u64) -> u64 {
    let hex_chars: ptr = "0123456789abcdef";
    let mut i: u64 = 0;
    
    // Print "0x"
    unsafe {
        syscall(SYS_WRITE, STDOUT, "0x", 2);
    }
    
    // Print address (16 hex digits)
    while i < 16 {
        let shift: u64 = 60 - (i * 4);
        let nibble: u8 = ((addr >> shift) & 0x0F) as u8;
        
        unsafe {
            syscall(SYS_WRITE, STDOUT, hex_chars + nibble, 1);
        }
        
        i = i + 1;
    }
    
    return 0;
}

fn dump_memory_with_addresses(addr: u64, size: u64) -> u64 {
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return 1;
    }
    
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    let mut i: u64 = 0;
    while i < bytes_read {
        if i % 16 == 0 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "\n", 1);
            }
            print_address(addr + i);
            unsafe {
                syscall(SYS_WRITE, STDOUT, ": ", 2);
            }
        }
        
        print_hex_byte(buffer[i]);
        unsafe {
            syscall(SYS_WRITE, STDOUT, " ", 1);
        }
        
        i = i + 1;
    }
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\n", 1);
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return 0;
}
```

**Output:**
```
0x00007ffd12345678: 48 47 46 45 44 43 42 41 00 00 00 00 00 00 00 00
0x00007ffd12345688: ...
```

### Feature 2: ASCII Display

```lwanga
fn is_printable(c: u8) -> u64 {
    if c >= 32 && c <= 126 {
        return 1;
    }
    return 0;
}

fn dump_memory_with_ascii(addr: u64, size: u64) -> u64 {
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return 1;
    }
    
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    let mut i: u64 = 0;
    while i < bytes_read {
        if i % 16 == 0 {
            if i > 0 {
                // Print ASCII representation
                unsafe {
                    syscall(SYS_WRITE, STDOUT, "  |", 3);
                }
                
                let mut j: u64 = i - 16;
                while j < i {
                    if is_printable(buffer[j]) == 1 {
                        unsafe {
                            syscall(SYS_WRITE, STDOUT, &buffer[j], 1);
                        }
                    } else {
                        unsafe {
                            syscall(SYS_WRITE, STDOUT, ".", 1);
                        }
                    }
                    j = j + 1;
                }
                
                unsafe {
                    syscall(SYS_WRITE, STDOUT, "|\n", 2);
                }
            }
            
            print_address(addr + i);
            unsafe {
                syscall(SYS_WRITE, STDOUT, ": ", 2);
            }
        }
        
        print_hex_byte(buffer[i]);
        unsafe {
            syscall(SYS_WRITE, STDOUT, " ", 1);
        }
        
        i = i + 1;
    }
    
    // Print final ASCII
    let remaining: u64 = bytes_read % 16;
    if remaining > 0 {
        let padding: u64 = 16 - remaining;
        let mut p: u64 = 0;
        while p < padding {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "   ", 3);
            }
            p = p + 1;
        }
        
        unsafe {
            syscall(SYS_WRITE, STDOUT, "  |", 3);
        }
        
        let start: u64 = bytes_read - remaining;
        let mut j: u64 = start;
        while j < bytes_read {
            if is_printable(buffer[j]) == 1 {
                unsafe {
                    syscall(SYS_WRITE, STDOUT, &buffer[j], 1);
                }
            } else {
                unsafe {
                    syscall(SYS_WRITE, STDOUT, ".", 1);
                }
            }
            j = j + 1;
        }
        
        unsafe {
            syscall(SYS_WRITE, STDOUT, "|\n", 2);
        }
    }
    
    unsafe {
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return 0;
}
```

**Output:**
```
0x00007ffd12345678: 48 65 6c 6c 6f 20 57 6f 72 6c 64 21 00 00 00 00  |Hello World!....|
```

### Feature 3: Pattern Search

```lwanga
fn search_memory(addr: u64, size: u64, pattern: ptr, pattern_len: u64) -> u64 {
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return 0;
    }
    
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    let buffer: [u8; 1024] = [0; 1024];
    let to_read: u64 = if size > 1024 { 1024 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    // Search for pattern
    let mut i: u64 = 0;
    let mut found_count: u64 = 0;
    
    while i <= bytes_read - pattern_len {
        let mut matches: u64 = 1;
        let mut j: u64 = 0;
        
        unsafe {
            while j < pattern_len {
                if buffer[i + j] != *(pattern + j) {
                    matches = 0;
                    break;
                }
                j = j + 1;
            }
        }
        
        if matches == 1 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Found at ", 9);
            }
            print_address(addr + i);
            unsafe {
                syscall(SYS_WRITE, STDOUT, "\n", 1);
            }
            found_count = found_count + 1;
        }
        
        i = i + 1;
    }
    
    unsafe {
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return found_count;
}
```

---

## Practical Examples

### Example 1: Dump Stack Variables

```lwanga
fn dump_stack_example() -> u64 {
    let var1: u64 = 0x1111111111111111;
    let var2: u64 = 0x2222222222222222;
    let var3: u64 = 0x3333333333333333;
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, "Stack variables:\n", 17);
    }
    
    let addr: u64 = &var1 as u64;
    dump_memory_with_addresses(addr, 24);
    
    return 0;
}
```

### Example 2: Dump String Contents

```lwanga
fn dump_string_example() -> u64 {
    let message: ptr = "Hello, Memory Dumper!";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, "String contents:\n", 17);
    }
    
    let addr: u64 = message as u64;
    dump_memory_with_ascii(addr, 32);
    
    return 0;
}
```

### Example 3: Search for Value

```lwanga
fn search_example() -> u64 {
    let data: [u64; 10] = [1, 2, 3, 42, 5, 6, 42, 8, 9, 10];
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, "Searching for 42:\n", 18);
    }
    
    let pattern: [u8; 8] = [42, 0, 0, 0, 0, 0, 0, 0];
    let addr: u64 = &data as u64;
    
    let found: u64 = search_memory(addr, 80, &pattern as ptr, 8);
    
    return found;
}
```

---

## Common Mistakes

### Mistake 1: Invalid Address

```lwanga
// WRONG: Trying to read invalid address
let invalid_addr: u64 = 0;
dump_memory(invalid_addr, 64);  // Will fail!
```

**Fix:** Always validate addresses before reading.

### Mistake 2: Reading Too Much

```lwanga
// WRONG: Reading huge amount
dump_memory(addr, 1000000000);  // Very slow!
```

**Fix:** Limit read sizes to reasonable amounts.

### Mistake 3: Not Checking Errors

```lwanga
// WRONG: Ignoring open failure
let fd: u64 = 0;
unsafe {
    fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
}
// What if fd == -1?
```

**Fix:** Always check return values.

### Mistake 4: Buffer Overflow

```lwanga
// WRONG: Reading more than buffer size
let buffer: [u8; 64] = [0; 64];
unsafe {
    syscall(SYS_READ, fd, &buffer, 1024);  // Overflow!
}
```

**Fix:** Never read more than buffer size.

---

## Exercises

### Exercise 1: Add Byte Count

Display total bytes dumped at the end.

<details>
<summary>Solution</summary>

```lwanga
fn dump_with_count(addr: u64, size: u64) -> u64 {
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return 0;
    }
    
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    // Dump memory...
    let mut i: u64 = 0;
    while i < bytes_read {
        if i % 16 == 0 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "\n", 1);
            }
        }
        print_hex_byte(buffer[i]);
        unsafe {
            syscall(SYS_WRITE, STDOUT, " ", 1);
        }
        i = i + 1;
    }
    
    // Print count
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\n\nTotal bytes: ", 14);
    }
    print_number(bytes_read);
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\n", 1);
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return bytes_read;
}
```
</details>

### Exercise 2: Dump Multiple Regions

Create a function that dumps multiple memory regions.

<details>
<summary>Solution</summary>

```lwanga
fn dump_multiple_regions() -> u64 {
    let var1: u64 = 0xAAAAAAAAAAAAAAAA;
    let var2: u64 = 0xBBBBBBBBBBBBBBBB;
    let var3: u64 = 0xCCCCCCCCCCCCCCCC;
    
    let regions: [u64; 3] = [
        &var1 as u64,
        &var2 as u64,
        &var3 as u64,
    ];
    
    let mut i: u64 = 0;
    
    while i < 3 {
        unsafe {
            syscall(SYS_WRITE, STDOUT, "\nRegion ", 8);
        }
        print_number(i + 1);
        unsafe {
            syscall(SYS_WRITE, STDOUT, ":\n", 2);
        }
        
        dump_memory_with_addresses(regions[i], 8);
        
        i = i + 1;
    }
    
    return 0;
}
```
</details>

### Exercise 3: Export to File

Modify the dumper to write output to a file instead of stdout.

<details>
<summary>Solution</summary>

```lwanga
const O_WRONLY: u32 = 1;
const O_CREAT: u32 = 64;
const O_TRUNC: u32 = 512;

fn dump_to_file(addr: u64, size: u64, output_path: ptr) -> u64 {
    // Open memory
    let mem_fd: u64 = 0;
    unsafe {
        mem_fd = syscall(SYS_OPEN, "/proc/self/mem", O_RDONLY, 0);
    }
    
    if mem_fd == -1 {
        return 1;
    }
    
    // Open output file
    let out_fd: u64 = 0;
    unsafe {
        out_fd = syscall(SYS_OPEN, output_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    }
    
    if out_fd == -1 {
        unsafe {
            syscall(SYS_CLOSE, mem_fd, 0, 0);
        }
        return 1;
    }
    
    // Read and write
    unsafe {
        syscall(SYS_LSEEK, mem_fd, addr, 0);
    }
    
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, mem_fd, &buffer, to_read);
    }
    
    unsafe {
        syscall(SYS_WRITE, out_fd, &buffer, bytes_read);
        syscall(SYS_CLOSE, mem_fd, 0, 0);
        syscall(SYS_CLOSE, out_fd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let data: u64 = 0x1234567890ABCDEF;
    let addr: u64 = &data as u64;
    
    dump_to_file(addr, 8, "/tmp/memory_dump.bin");
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Process memory layout and organization
- ✅ Reading process memory via /proc/self/mem
- ✅ Formatting hex dumps
- ✅ Displaying ASCII representations
- ✅ Searching memory for patterns
- ✅ Memory forensics techniques
- ✅ Building practical analysis tools

**Remember:** Memory dumping is powerful but requires careful handling. Always validate addresses and handle errors properly.

---

## Next Lesson

Now that you can dump memory, let's build a comprehensive final project in [Lesson 40: Final Project](40-final-project.md).

---

**Navigation**: [← Previous: Port Scanner](38-port-scanner.md) | [Tutorial Home](README.md) | [Next: Final Project →](40-final-project.md)

```lwanga
const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;
const SYS_LSEEK: u64 = 8;

const O_RDONLY: u32 = 0;
const STDOUT: u64 = 1;

fn print_hex_byte(byte: u8) -> u64 {
    let hex_chars: ptr = "0123456789abcdef";
    let high: u8 = (byte >> 4) & 0x0F;
    let low: u8 = byte & 0x0F;
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, hex_chars + high, 1);
        syscall(SYS_WRITE, STDOUT, hex_chars + low, 1);
    }
    
    return 0;
}

fn dump_memory(addr: u64, size: u64) -> u64 {
    // Open /proc/self/mem
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        let msg: ptr = "Failed to open memory\n";
        unsafe {
            syscall(SYS_WRITE, STDOUT, msg, 22);
        }
        return 1;
    }
    
    // Seek to address
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    // Read memory
    let buffer: [u8; 256] = [0; 256];
    let to_read: u64 = if size > 256 { 256 } else { size };
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, &buffer, to_read);
    }
    
    // Print hex dump
    let mut i: u64 = 0;
    while i < bytes_read {
        if i % 16 == 0 {
            let newline: ptr = "\n";
            unsafe {
                syscall(SYS_WRITE, STDOUT, newline, 1);
            }
        }
        
        print_hex_byte(buffer[i]);
        
        let space: ptr = " ";
        unsafe {
            syscall(SYS_WRITE, STDOUT, space, 1);
        }
        
        i = i + 1;
    }
    
    let newline: ptr = "\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, newline, 1);
    }
    
    // Close file
    unsafe {
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let msg: ptr = "Memory Dumper\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 14);
    }
    
    // Dump some memory (stack)
    let stack_var: u64 = 0x4142434445464748;
    let addr: u64 = &stack_var as u64;
    
    dump_memory(addr, 64);
    
    return 0;
}
```

---

## Usage

```bash
# Compile
lwangac memory_dumper.lwanga -o memory_dumper

# Run
./memory_dumper
```

---

**Navigation**: [← Previous: Port Scanner](38-port-scanner.md) | [Tutorial Home](README.md) | [Next: Final Project →](40-final-project.md)
