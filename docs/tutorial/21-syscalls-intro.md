# Lesson 21: System Calls Introduction

**In this lesson, you'll learn what system calls are, how they work, and how to use them to interact with the operating system.**

---

## What are System Calls?

System calls are the interface between your program and the operating system kernel. They allow your program to request services from the OS.

**Why system calls?**

Your program runs in user space with limited privileges. To do anything that requires OS resources, you must ask the kernel:
- Read/write files
- Allocate memory
- Create network connections
- Manage processes
- Access hardware

**How they work:**

1. Your program prepares arguments
2. Executes a special CPU instruction (syscall)
3. CPU switches to kernel mode
4. Kernel performs the operation
5. Kernel returns result
6. CPU switches back to user mode
7. Your program continues

---

## The syscall Function

Lwanga provides a built-in `syscall` function:

```lwanga
unsafe {
    let result: u64 = syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}
```

**Parameters:**
- `number`: System call number (identifies which syscall)
- `arg1-arg6`: Arguments (meaning depends on the syscall)

**Returns:** Result value (meaning depends on the syscall)

**Why unsafe?**

System calls bypass all safety checks and can:
- Access any memory
- Crash the program
- Perform privileged operations
- Have side effects

---

## System Call Numbers (Linux x86_64)

Each system call has a unique number:

| Number | Name | Purpose |
|--------|------|---------|
| 0 | read | Read from file descriptor |
| 1 | write | Write to file descriptor |
| 2 | open | Open file |
| 3 | close | Close file descriptor |
| 4 | stat | Get file status |
| 5 | fstat | Get file status by fd |
| 8 | lseek | Reposition file offset |
| 9 | mmap | Map memory |
| 10 | mprotect | Set memory protection |
| 11 | munmap | Unmap memory |
| 12 | brk | Change data segment size |
| 39 | getpid | Get process ID |
| 41 | socket | Create socket |
| 42 | connect | Connect socket |
| 43 | accept | Accept connection |
| 57 | fork | Create child process |
| 59 | execve | Execute program |
| 60 | exit | Exit program |

**Design choice:** Lwanga uses direct syscall numbers instead of libc wrappers. This creates smaller binaries and gives you full control.

---

## File Descriptors

Many syscalls work with file descriptors (fd):

```lwanga
const STDIN: u64 = 0;   // Standard input
const STDOUT: u64 = 1;  // Standard output
const STDERR: u64 = 2;  // Standard error
```

**What is a file descriptor?**

A small integer that represents an open file, socket, pipe, or other I/O resource. The kernel maintains a table of open files for each process.

---

## The Write System Call

The most common syscall - write data to a file descriptor:

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 14);
    }
    
    return 0;
}
```

**Signature:**
```c
ssize_t write(int fd, const void *buf, size_t count);
```

**Arguments:**
- `fd`: File descriptor to write to
- `buf`: Pointer to data
- `count`: Number of bytes to write

**Returns:** Number of bytes written, or -1 on error

### Writing to Different Outputs

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;

fn write_stdout(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}

fn write_stderr(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDERR, msg, len);
    }
}

fn main() -> u64 {
    let normal: ptr = "Normal message\n";
    let error: ptr = "Error message\n";
    
    write_stdout(normal, 15);
    write_stderr(error, 14);
    
    return 0;
}
```

---

## The Read System Call

Read data from a file descriptor:

```lwanga
const SYS_READ: u64 = 0;
const STDIN: u64 = 0;

fn main() -> u64 {
    let mut buffer: [u8; 100] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    unsafe {
        let bytes_read: u64 = syscall(SYS_READ, STDIN, &buffer, 100);
        return bytes_read;
    }
}
```

**Signature:**
```c
ssize_t read(int fd, void *buf, size_t count);
```

**Arguments:**
- `fd`: File descriptor to read from
- `buf`: Buffer to store data
- `count`: Maximum bytes to read

**Returns:** Number of bytes read, 0 on EOF, -1 on error

---

## The Exit System Call

Terminate the program:

```lwanga
const SYS_EXIT: u64 = 60;

fn exit_program(code: u64) -> u64 {
    unsafe {
        syscall(SYS_EXIT, code, 0, 0);
    }
    return 0;  // Never reached
}

fn main() -> u64 {
    let msg: ptr = "Exiting...\n";
    
    unsafe {
        syscall(1, 1, msg, 11);
    }
    
    exit_program(0);
    return 0;  // Never reached
}
```

**Signature:**
```c
void exit(int status);
```

**Arguments:**
- `status`: Exit code (0 = success, non-zero = error)

**Returns:** Never returns (terminates process)

---

## Error Handling

System calls return -1 on error and set errno. In Lwanga, you check the return value:

```lwanga
const SYS_OPEN: u64 = 2;
const O_RDONLY: u64 = 0;

fn safe_open(path: ptr) -> u64 {
    unsafe {
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        // Check for error (fd will be -1 cast to u64)
        if fd > 0xFFFFFFFFFFFF0000 {  // Negative value
            return 0;  // Error
        }
        
        return fd;
    }
}

fn main() -> u64 {
    let path: ptr = "nonexistent.txt";
    let fd: u64 = safe_open(path);
    
    if fd == 0 {
        let err: ptr = "Failed to open file\n";
        unsafe {
            syscall(1, 2, err, 20);  // Write to stderr
        }
        return 1;
    }
    
    return 0;
}
```

---

## Practical Examples

### Example 1: Echo Program

```lwanga
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const STDIN: u64 = 0;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    unsafe {
        // Read from stdin
        let bytes_read: u64 = syscall(SYS_READ, STDIN, &buffer, 1024);
        
        if bytes_read > 0 {
            // Echo to stdout
            syscall(SYS_WRITE, STDOUT, &buffer, bytes_read);
        }
    }
    
    return 0;
}
```

### Example 2: Print Numbers

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn print_digit(digit: u8) -> u64 {
    let ch: u8 = digit + 48;  // Convert to ASCII
    
    unsafe {
        return syscall(SYS_WRITE, STDOUT, &ch, 1);
    }
}

fn print_number(n: u64) -> u64 {
    if n == 0 {
        return print_digit(0);
    }
    
    // Convert to digits
    let mut num: u64 = n;
    let mut digits: [u8; 20] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    let mut count: u64 = 0;
    
    while num > 0 {
        digits[count] = (num % 10) as u8;
        num = num / 10;
        count = count + 1;
    }
    
    // Print in reverse order
    let mut i: u64 = count;
    while i > 0 {
        i = i - 1;
        print_digit(digits[i]);
    }
    
    return count;
}

fn main() -> u64 {
    print_number(12345);
    
    let newline: u8 = 10;
    unsafe {
        syscall(SYS_WRITE, STDOUT, &newline, 1);
    }
    
    return 0;
}
```

### Example 3: Simple Cat

```lwanga
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_OPEN: u64 = 2;
const SYS_CLOSE: u64 = 3;
const O_RDONLY: u64 = 0;
const STDOUT: u64 = 1;

fn cat_file(path: ptr) -> u64 {
    unsafe {
        // Open file
        let fd: u64 = syscall(SYS_OPEN, path, O_RDONLY, 0);
        
        if fd > 0xFFFFFFFFFFFF0000 {
            return 1;  // Error opening
        }
        
        // Read and print
        let mut buffer: [u8; 4096] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        
        let bytes_read: u64 = syscall(SYS_READ, fd, &buffer, 4096);
        
        if bytes_read > 0 {
            syscall(SYS_WRITE, STDOUT, &buffer, bytes_read);
        }
        
        // Close file
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let path: ptr = "test.txt";
    return cat_file(path);
}
```

---

## Common Mistakes

### Mistake 1: Wrong Argument Count

```lwanga
// WRONG: Not enough arguments
unsafe {
    syscall(1, 1);  // Missing buffer and count
}
```

**Fix:**
```lwanga
unsafe {
    syscall(1, 1, msg, len);  // All arguments provided
}
```

### Mistake 2: Forgetting Unsafe

```lwanga
// WRONG: syscall requires unsafe
fn main() -> u64 {
    syscall(60, 0, 0, 0);  // ERROR
    return 0;
}
```

**Fix:**
```lwanga
fn main() -> u64 {
    unsafe {
        syscall(60, 0, 0, 0);
    }
    return 0;
}
```

### Mistake 3: Not Checking Errors

```lwanga
// WRONG: Ignoring errors
unsafe {
    let fd: u64 = syscall(2, path, 0, 0);
    // Use fd without checking if open succeeded
}
```

**Fix:**
```lwanga
unsafe {
    let fd: u64 = syscall(2, path, 0, 0);
    
    if fd > 0xFFFFFFFFFFFF0000 {
        return 1;  // Handle error
    }
    
    // Safe to use fd
}
```

---

## Exercises

### Exercise 1: Write to Stderr

Write a function that prints an error message to stderr.

<details>
<summary>Solution</summary>

```lwanga
const SYS_WRITE: u64 = 1;
const STDERR: u64 = 2;

fn print_error(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDERR, msg, len);
    }
}

fn main() -> u64 {
    let err: ptr = "Error: Something went wrong\n";
    print_error(err, 28);
    return 1;
}
```
</details>

### Exercise 2: Read One Character

Write a function that reads a single character from stdin.

<details>
<summary>Solution</summary>

```lwanga
const SYS_READ: u64 = 0;
const STDIN: u64 = 0;

fn read_char() -> u8 {
    let mut ch: u8 = 0;
    
    unsafe {
        syscall(SYS_READ, STDIN, &ch, 1);
    }
    
    return ch;
}

fn main() -> u64 {
    let c: u8 = read_char();
    return c as u64;
}
```
</details>

### Exercise 3: Print Multiple Lines

Write a function that prints multiple lines using separate syscalls.

<details>
<summary>Solution</summary>

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn println(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
        
        let newline: u8 = 10;
        syscall(SYS_WRITE, STDOUT, &newline, 1);
    }
    
    return 0;
}

fn main() -> u64 {
    println("Line 1", 6);
    println("Line 2", 6);
    println("Line 3", 6);
    return 0;
}
```
</details>

### Exercise 4: Count Bytes Read

Write a program that reads from stdin and reports how many bytes were read.

<details>
<summary>Solution</summary>

```lwanga
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const STDIN: u64 = 0;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let mut buffer: [u8; 100] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                  0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    unsafe {
        let bytes: u64 = syscall(SYS_READ, STDIN, &buffer, 100);
        
        let msg: ptr = "Read bytes: ";
        syscall(SYS_WRITE, STDOUT, msg, 12);
        
        return bytes;
    }
}
```
</details>

### Exercise 5: Safe Exit Wrapper

Create a safe wrapper for the exit syscall that validates the exit code.

<details>
<summary>Solution</summary>

```lwanga
const SYS_EXIT: u64 = 60;

fn safe_exit(code: u64) -> u64 {
    // Validate exit code (0-255)
    let exit_code: u64 = if code > 255 {
        1  // Use generic error code
    } else {
        code
    };
    
    unsafe {
        syscall(SYS_EXIT, exit_code, 0, 0);
    }
    
    return 0;  // Never reached
}

fn main() -> u64 {
    safe_exit(0);
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What system calls are and why they're needed
- ✅ How system calls work (user space → kernel space)
- ✅ The syscall function and its parameters
- ✅ Common system call numbers (read, write, open, close, exit)
- ✅ File descriptors (stdin, stdout, stderr)
- ✅ Error handling with system calls
- ✅ Practical examples: echo, cat, print numbers
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand system calls, let's learn about file operations in [Lesson 22: File Operations](22-file-operations.md).

---

**Navigation**: [← Previous: Memory Safety](20-memory-safety.md) | [Tutorial Home](README.md) | [Next: File Operations →](22-file-operations.md)
