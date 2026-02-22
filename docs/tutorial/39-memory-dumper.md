# Lesson 39: Memory Dumper

**Complete project: Build a process memory dumper.**

---

## Project Overview

Build a tool that:
- Reads process memory
- Dumps memory regions
- Formats output
- Handles errors

---

## Complete Code

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
