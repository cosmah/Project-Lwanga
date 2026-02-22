# Lesson 22: File Operations

**Learn how to read and write files using system calls.**

---

## Opening Files

```lwanga
const SYS_OPEN: u64 = 2;
const O_RDONLY: u32 = 0;
const O_WRONLY: u32 = 1;
const O_RDWR: u32 = 2;

fn open_file(path: ptr, flags: u32) -> u64 {
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, flags, 0);
    }
    
    return fd;
}
```

---

## Reading Files

```lwanga
const SYS_READ: u64 = 0;

fn read_file(fd: u64, buffer: ptr, count: u64) -> u64 {
    let bytes_read: u64 = 0;
    
    unsafe {
        bytes_read = syscall(SYS_READ, fd, buffer, count);
    }
    
    return bytes_read;
}
```

---

## Writing Files

```lwanga
const SYS_WRITE: u64 = 1;

fn write_file(fd: u64, data: ptr, count: u64) -> u64 {
    let bytes_written: u64 = 0;
    
    unsafe {
        bytes_written = syscall(SYS_WRITE, fd, data, count);
    }
    
    return bytes_written;
}
```

---

## Closing Files

```lwanga
const SYS_CLOSE: u64 = 3;

fn close_file(fd: u64) -> u64 {
    unsafe {
        return syscall(SYS_CLOSE, fd, 0, 0);
    }
}
```

---

## Complete Example

```lwanga
fn main() -> u64 {
    let path: ptr = "test.txt";
    let fd: u64 = open_file(path, O_RDONLY);
    
    if fd == -1 {
        return 1;  // Error
    }
    
    let buffer: [u8; 1024] = [0; 1024];
    let bytes: u64 = read_file(fd, &buffer, 1024);
    
    close_file(fd);
    
    return bytes;
}
```

---

**Navigation**: [← Previous: Syscalls Intro](21-syscalls-intro.md) | [Tutorial Home](README.md) | [Next: Process Management →](23-process-management.md)
