# Lesson 22: File Operations

**In this lesson, you'll learn how to work with files using system calls: opening, reading, writing, and closing files.**

---

## File Operations Overview

Working with files requires four main operations:

1. **Open** - Get a file descriptor
2. **Read/Write** - Transfer data
3. **Seek** - Move file position (optional)
4. **Close** - Release file descriptor

**Why use syscalls instead of libc?**
- Smaller binaries (no libc dependency)
- Full control over behavior
- Direct kernel interaction
- Better for security tools

---

## Opening Files

The `open` syscall creates a file descriptor:

```lwanga
const SYS_OPEN: u64 = 2;

// Open flags
const O_RDONLY: u64 = 0;      // Read only
const O_WRONLY: u64 = 1;      // Write only
const O_RDWR: u64 = 2;        // Read and write
const O_CREAT: u64 = 64;      // Create if doesn't exist
const O_TRUNC: u64 = 512;     // Truncate to zero length
const O_APPEND: u64 = 1024;   // Append mode

fn open_file(path: ptr, flags: u64, mode: u64) -> u64 {
    unsafe {
        return syscall(SYS_OPEN, path, flags, mode);
    }
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    let fd: u64 = open_file(path, O_RDONLY, 0);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;  // Error
    }
    
    return fd;
}
```

**Signature:**
```c
int open(const char *pathname, int flags, mode_t mode);
```

**Arguments:**
- `pathname`: Path to file
- `flags`: Open mode (O_RDONLY, O_WRONLY, etc.)
- `mode`: Permissions (when creating file)

**Returns:** File descriptor on success, -1 on error

### File Permissions

When creating files with O_CREAT, specify permissions:

```lwanga
const S_IRUSR: u64 = 0o400;  // User read
const S_IWUSR: u64 = 0o200;  // User write
const S_IXUSR: u64 = 0o100;  // User execute
const S_IRGRP: u64 = 0o040;  // Group read
const S_IWGRP: u64 = 0o020;  // Group write
const S_IROTH: u64 = 0o004;  // Other read

fn create_file(path: ptr) -> u64 {
    let flags: u64 = O_WRONLY | O_CREAT | O_TRUNC;
    let mode: u64 = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;  // 0644
    
    unsafe {
        return syscall(SYS_OPEN, path, flags, mode);
    }
}
```

---

## Reading Files

The `read` syscall reads data from a file descriptor:

```lwanga
const SYS_READ: u64 = 0;

fn read_file(fd: u64, buffer: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(SYS_READ, fd, buffer, count);
    }
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    let fd: u64 = open_file(path, O_RDONLY, 0);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;
    }
    
    let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    let bytes_read: u64 = read_file(fd, &buffer, 1024);
    
    return bytes_read;
}
```

**Signature:**
```c
ssize_t read(int fd, void *buf, size_t count);
```

**Returns:**
- Number of bytes read on success
- 0 on end of file (EOF)
- -1 on error

### Reading in a Loop

```lwanga
fn read_entire_file(fd: u64, buffer: ptr, buffer_size: u64) -> u64 {
    let mut total_read: u64 = 0;
    
    unsafe {
        while total_read < buffer_size {
            let buf_ptr: ptr = (buffer as u64 + total_read) as ptr;
            let remaining: u64 = buffer_size - total_read;
            
            let bytes: u64 = syscall(SYS_READ, fd, buf_ptr, remaining);
            
            if bytes == 0 {
                break;  // EOF
            }
            
            if bytes > 0xFFFFFFFFFFFF0000 {
                return 0;  // Error
            }
            
            total_read = total_read + bytes;
        }
    }
    
    return total_read;
}
```

---

## Writing Files

The `write` syscall writes data to a file descriptor:

```lwanga
const SYS_WRITE: u64 = 1;

fn write_file(fd: u64, data: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, fd, data, count);
    }
}

fn main() -> u64 {
    let path: ptr = "output.txt";
    let flags: u64 = O_WRONLY | O_CREAT | O_TRUNC;
    let mode: u64 = 0o644;
    
    let fd: u64 = open_file(path, flags, mode);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;
    }
    
    let data: ptr = "Hello, File!\n";
    let bytes_written: u64 = write_file(fd, data, 13);
    
    return bytes_written;
}
```

**Signature:**
```c
ssize_t write(int fd, const void *buf, size_t count);
```

**Returns:**
- Number of bytes written on success
- -1 on error

### Writing in a Loop

```lwanga
fn write_all(fd: u64, data: ptr, count: u64) -> u64 {
    let mut total_written: u64 = 0;
    
    unsafe {
        while total_written < count {
            let data_ptr: ptr = (data as u64 + total_written) as ptr;
            let remaining: u64 = count - total_written;
            
            let bytes: u64 = syscall(SYS_WRITE, fd, data_ptr, remaining);
            
            if bytes > 0xFFFFFFFFFFFF0000 {
                return 0;  // Error
            }
            
            total_written = total_written + bytes;
        }
    }
    
    return total_written;
}
```

---

## Closing Files

The `close` syscall releases a file descriptor:

```lwanga
const SYS_CLOSE: u64 = 3;

fn close_file(fd: u64) -> u64 {
    unsafe {
        return syscall(SYS_CLOSE, fd, 0, 0);
    }
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    let fd: u64 = open_file(path, O_RDONLY, 0);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;
    }
    
    // Use file...
    
    close_file(fd);
    return 0;
}
```

**Why close files?**
- Releases system resources
- Flushes buffered data
- Prevents file descriptor leaks
- Good practice

---

## Seeking in Files

The `lseek` syscall changes the file position:

```lwanga
const SYS_LSEEK: u64 = 8;

// Seek whence values
const SEEK_SET: u64 = 0;  // From beginning
const SEEK_CUR: u64 = 1;  // From current position
const SEEK_END: u64 = 2;  // From end

fn seek_file(fd: u64, offset: u64, whence: u64) -> u64 {
    unsafe {
        return syscall(SYS_LSEEK, fd, offset, whence);
    }
}

fn get_file_size(fd: u64) -> u64 {
    // Seek to end
    let size: u64 = seek_file(fd, 0, SEEK_END);
    
    // Seek back to beginning
    seek_file(fd, 0, SEEK_SET);
    
    return size;
}
```

---

## Practical Examples

### Example 1: Copy File

```lwanga
const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;
const O_RDONLY: u64 = 0;
const O_WRONLY: u64 = 1;
const O_CREAT: u64 = 64;
const O_TRUNC: u64 = 512;

fn copy_file(src_path: ptr, dst_path: ptr) -> u64 {
    unsafe {
        // Open source
        let src_fd: u64 = syscall(SYS_OPEN, src_path, O_RDONLY, 0);
        if src_fd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        // Open destination
        let dst_fd: u64 = syscall(SYS_OPEN, dst_path, 
                                   O_WRONLY | O_CREAT | O_TRUNC, 0o644);
        if dst_fd > 0xFFFFFFFFFFFF0000 {
            syscall(SYS_CLOSE, src_fd, 0, 0);
            return 2;
        }
        
        // Copy data
        let mut buffer: [u8; 4096] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        
        let mut total: u64 = 0;
        
        while 1 == 1 {
            let bytes_read: u64 = syscall(SYS_READ, src_fd, &buffer, 4096);
            
            if bytes_read == 0 {
                break;  // EOF
            }
            
            if bytes_read > 0xFFFFFFFFFFFF0000 {
                break;  // Error
            }
            
            syscall(SYS_WRITE, dst_fd, &buffer, bytes_read);
            total = total + bytes_read;
        }
        
        // Close files
        syscall(SYS_CLOSE, src_fd, 0, 0);
        syscall(SYS_CLOSE, dst_fd, 0, 0);
        
        return 0;
    }
}

fn main() -> u64 {
    let src: ptr = "source.txt";
    let dst: ptr = "destination.txt";
    return copy_file(src, dst);
}
```

### Example 2: Append to File

```lwanga
const O_APPEND: u64 = 1024;

fn append_to_file(path: ptr, data: ptr, len: u64) -> u64 {
    unsafe {
        let flags: u64 = O_WRONLY | O_CREAT | O_APPEND;
        let fd: u64 = syscall(SYS_OPEN, path, flags, 0o644);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        syscall(SYS_WRITE, fd, data, len);
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return 0;
    }
}

fn main() -> u64 {
    let path: ptr = "log.txt";
    let entry: ptr = "Log entry\n";
    
    append_to_file(path, entry, 10);
    append_to_file(path, entry, 10);
    append_to_file(path, entry, 10);
    
    return 0;
}
```

### Example 3: Read File Line by Line

```lwanga
fn read_line(fd: u64, buffer: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < max_len - 1 {
            let ch_ptr: ptr = (buffer as u64 + i) as ptr;
            let bytes: u64 = syscall(SYS_READ, fd, ch_ptr, 1);
            
            if bytes == 0 {
                break;  // EOF
            }
            
            let ch: u8 = *(ch_ptr as ptr);
            
            if ch == 10 {  // Newline
                i = i + 1;
                break;
            }
            
            i = i + 1;
        }
        
        // Null terminate
        let null_ptr: ptr = (buffer as u64 + i) as ptr;
        *(null_ptr as ptr) = 0;
    }
    
    return i;
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    
    unsafe {
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        let mut buffer: [u8; 256] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                      0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        
        let len: u64 = read_line(fd, &buffer, 256);
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return len;
    }
}
```

### Example 4: Write Formatted Data

```lwanga
fn write_number(fd: u64, n: u64) -> u64 {
    if n == 0 {
        let zero: u8 = 48;
        unsafe {
            return syscall(SYS_WRITE, fd, &zero, 1);
        }
    }
    
    let mut digits: [u8; 20] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let mut count: u64 = 0;
    let mut num: u64 = n;
    
    while num > 0 {
        digits[count] = ((num % 10) + 48) as u8;
        num = num / 10;
        count = count + 1;
    }
    
    // Write in reverse
    let mut i: u64 = count;
    unsafe {
        while i > 0 {
            i = i - 1;
            syscall(SYS_WRITE, fd, &digits[i], 1);
        }
    }
    
    return count;
}

fn main() -> u64 {
    let path: ptr = "numbers.txt";
    
    unsafe {
        let flags: u64 = O_WRONLY | O_CREAT | O_TRUNC;
        let fd: u64 = syscall(SYS_OPEN, path, flags, 0o644);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        write_number(fd, 12345);
        
        let newline: u8 = 10;
        syscall(SYS_WRITE, fd, &newline, 1);
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return 0;
    }
}
```

---

## Common Mistakes

### Mistake 1: Not Checking Open Errors

```lwanga
// WRONG: Not checking if open succeeded
let fd: u64 = open_file(path, O_RDONLY, 0);
read_file(fd, buffer, 1024);  // Might fail!
```

**Fix:**
```lwanga
let fd: u64 = open_file(path, O_RDONLY, 0);

if fd > 0xFFFFFFFFFFFF0000 {
    return 1;  // Handle error
}

read_file(fd, buffer, 1024);
```

### Mistake 2: Forgetting to Close Files

```lwanga
// WRONG: File descriptor leak
fn read_data(path: ptr) -> u64 {
    let fd: u64 = open_file(path, O_RDONLY, 0);
    // ... read data ...
    return 0;  // fd never closed!
}
```

**Fix:**
```lwanga
fn read_data(path: ptr) -> u64 {
    let fd: u64 = open_file(path, O_RDONLY, 0);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;
    }
    
    // ... read data ...
    
    close_file(fd);  // Always close
    return 0;
}
```

### Mistake 3: Buffer Too Small

```lwanga
// WRONG: Buffer might be too small
let mut buffer: [u8; 10] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
read_file(fd, &buffer, 1024);  // Reading 1024 into 10-byte buffer!
```

**Fix:**
```lwanga
let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
read_file(fd, &buffer, 1024);  // Buffer size matches
```

---

## Exercises

### Exercise 1: Count File Lines

Write a function that counts the number of lines in a file.

<details>
<summary>Solution</summary>

```lwanga
const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_CLOSE: u64 = 3;
const O_RDONLY: u64 = 0;

fn count_lines(path: ptr) -> u64 {
    unsafe {
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        let mut count: u64 = 0;
        let mut ch: u8 = 0;
        
        while 1 == 1 {
            let bytes: u64 = syscall(SYS_READ, fd, &ch, 1);
            
            if bytes == 0 {
                break;
            }
            
            if ch == 10 {  // Newline
                count = count + 1;
            }
        }
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return count;
    }
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    return count_lines(path);
}
```
</details>

### Exercise 2: Write Array to File

Write a function that writes an array of numbers to a file.

<details>
<summary>Solution</summary>

```lwanga
const SYS_OPEN: u64 = 2;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;
const O_WRONLY: u64 = 1;
const O_CREAT: u64 = 64;
const O_TRUNC: u64 = 512;

fn write_array(path: ptr, arr: ptr, len: u64) -> u64 {
    unsafe {
        let flags: u64 = O_WRONLY | O_CREAT | O_TRUNC;
        let fd: u64 = syscall(SYS_OPEN, path, flags, 0o644);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        let mut i: u64 = 0;
        while i < len {
            let elem_ptr: ptr = (arr as u64 + i * 8) as ptr;
            let value: u64 = *(elem_ptr as ptr);
            
            // Write value as bytes
            syscall(SYS_WRITE, fd, elem_ptr, 8);
            
            i = i + 1;
        }
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return 0;
    }
}

fn main() -> u64 {
    let arr: [u64; 5] = [10, 20, 30, 40, 50];
    let path: ptr = "numbers.bin";
    return write_array(path, &arr, 5);
}
```
</details>

### Exercise 3: Read File into Buffer

Write a safe function that reads an entire file into a buffer with size checking.

<details>
<summary>Solution</summary>

```lwanga
fn safe_read_file(path: ptr, buffer: ptr, buffer_size: u64) -> u64 {
    if path as u64 == 0 || buffer as u64 == 0 || buffer_size == 0 {
        return 0;
    }
    
    unsafe {
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        let bytes_read: u64 = syscall(SYS_READ, fd, buffer, buffer_size);
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        if bytes_read > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        return bytes_read;
    }
}

fn main() -> u64 {
    let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let path: ptr = "test.txt";
    return safe_read_file(path, &buffer, 1024);
}
```
</details>

### Exercise 4: File Exists Check

Write a function that checks if a file exists.

<details>
<summary>Solution</summary>

```lwanga
fn file_exists(path: ptr) -> u64 {
    unsafe {
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 0;  // Doesn't exist or can't open
        }
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return 1;  // Exists
    }
}

fn main() -> u64 {
    let path1: ptr = "existing.txt";
    let path2: ptr = "nonexistent.txt";
    
    let exists1: u64 = file_exists(path1);
    let exists2: u64 = file_exists(path2);
    
    return exists1;
}
```
</details>

### Exercise 5: Truncate File

Write a function that truncates a file to zero length.

<details>
<summary>Solution</summary>

```lwanga
fn truncate_file(path: ptr) -> u64 {
    unsafe {
        let flags: u64 = O_WRONLY | O_TRUNC;
        let fd: u64 = syscall(SYS_OPEN, path, flags, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;  // Error
        }
        
        syscall(SYS_CLOSE, fd, 0, 0);
        
        return 0;  // Success
    }
}

fn main() -> u64 {
    let path: ptr = "file_to_truncate.txt";
    return truncate_file(path);
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to open files with different modes (read, write, create)
- ✅ File permissions and flags
- ✅ Reading data from files
- ✅ Writing data to files
- ✅ Closing files to release resources
- ✅ Seeking within files
- ✅ Practical examples: copy, append, line-by-line reading
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you can work with files, let's learn about process management in [Lesson 23: Process Management](23-process-management.md).

---

**Navigation**: [← Previous: Syscalls Intro](21-syscalls-intro.md) | [Tutorial Home](README.md) | [Next: Process Management →](23-process-management.md)
