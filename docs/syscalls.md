# Lwanga Syscall Reference

This document provides a reference for Linux system calls on x86_64 and ARM64 architectures.

## Overview

Lwanga provides direct access to system calls through the `syscall()` function. System calls are the interface between user-space programs and the Linux kernel.

### Syntax

```lwanga
unsafe {
    let result: u64 = syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}
```

- `number`: Syscall number (differs between architectures)
- `arg1-arg6`: Up to 6 arguments (unused arguments should be 0)
- Returns: Result value (typically 0 on success, negative on error)

### Error Handling

Syscalls return negative values on error. To check for errors:

```lwanga
let result: u64 = syscall(1, 1, msg, len);
if (result > 18446744073709551615 - 4096) {
    // Error occurred (result is -errno cast to u64)
}
```

## Common Syscalls

### File I/O

#### read - Read from file descriptor
- **x86_64**: 0
- **ARM64**: 63
- **Arguments**: `(fd: u64, buf: ptr, count: u64)`
- **Returns**: Number of bytes read, or negative on error

```lwanga
fn read(fd: u64, buf: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(0, fd, buf, count);
    }
}
```

#### write - Write to file descriptor
- **x86_64**: 1
- **ARM64**: 64
- **Arguments**: `(fd: u64, buf: ptr, count: u64)`
- **Returns**: Number of bytes written, or negative on error

```lwanga
fn write(fd: u64, buf: ptr, count: u64) -> u64 {
    unsafe {
        return syscall(1, fd, buf, count);
    }
}
```

#### open - Open file
- **x86_64**: 2
- **ARM64**: 56 (use openat with AT_FDCWD)
- **Arguments**: `(filename: ptr, flags: u64, mode: u64)`
- **Returns**: File descriptor, or negative on error

```lwanga
fn open(filename: ptr, flags: u64, mode: u64) -> u64 {
    unsafe {
        return syscall(2, filename, flags, mode);
    }
}
```

#### close - Close file descriptor
- **x86_64**: 3
- **ARM64**: 57
- **Arguments**: `(fd: u64)`
- **Returns**: 0 on success, negative on error

```lwanga
fn close(fd: u64) -> u64 {
    unsafe {
        return syscall(3, fd, 0, 0);
    }
}
```

### Process Management

#### exit - Terminate process
- **x86_64**: 60
- **ARM64**: 93
- **Arguments**: `(status: u64)`
- **Returns**: Does not return

```lwanga
fn exit(code: u64) -> u64 {
    unsafe {
        return syscall(60, code, 0, 0);
    }
}
```

#### fork - Create child process
- **x86_64**: 57
- **ARM64**: 220
- **Arguments**: None
- **Returns**: 0 in child, child PID in parent, negative on error

```lwanga
fn fork() -> u64 {
    unsafe {
        return syscall(57, 0, 0, 0);
    }
}
```

#### execve - Execute program
- **x86_64**: 59
- **ARM64**: 221
- **Arguments**: `(filename: ptr, argv: ptr, envp: ptr)`
- **Returns**: Does not return on success, negative on error

```lwanga
fn execve(filename: ptr, argv: ptr, envp: ptr) -> u64 {
    unsafe {
        return syscall(59, filename, argv, envp);
    }
}
```

### Memory Management

#### mmap - Map memory
- **x86_64**: 9
- **ARM64**: 222
- **Arguments**: `(addr: ptr, length: u64, prot: u64, flags: u64, fd: u64, offset: u64)`
- **Returns**: Mapped address, or negative on error

```lwanga
fn mmap(addr: ptr, length: u64, prot: u64, flags: u64, fd: u64, offset: u64) -> ptr {
    unsafe {
        return syscall(9, addr, length, prot, flags, fd, offset) as ptr;
    }
}
```

#### munmap - Unmap memory
- **x86_64**: 11
- **ARM64**: 215
- **Arguments**: `(addr: ptr, length: u64)`
- **Returns**: 0 on success, negative on error

```lwanga
fn munmap(addr: ptr, length: u64) -> u64 {
    unsafe {
        return syscall(11, addr, length, 0);
    }
}
```

### Network Operations

#### socket - Create socket
- **x86_64**: 41
- **ARM64**: 198
- **Arguments**: `(domain: u64, type: u64, protocol: u64)`
- **Returns**: Socket file descriptor, or negative on error

```lwanga
fn socket(domain: u64, type: u64, protocol: u64) -> u64 {
    unsafe {
        return syscall(41, domain, type, protocol);
    }
}
```

#### bind - Bind socket to address
- **x86_64**: 49
- **ARM64**: 200
- **Arguments**: `(sockfd: u64, addr: ptr, addrlen: u64)`
- **Returns**: 0 on success, negative on error

```lwanga
fn bind(sockfd: u64, addr: ptr, addrlen: u64) -> u64 {
    unsafe {
        return syscall(49, sockfd, addr, addrlen);
    }
}
```

#### listen - Listen for connections
- **x86_64**: 50
- **ARM64**: 201
- **Arguments**: `(sockfd: u64, backlog: u64)`
- **Returns**: 0 on success, negative on error

```lwanga
fn listen(sockfd: u64, backlog: u64) -> u64 {
    unsafe {
        return syscall(50, sockfd, backlog, 0);
    }
}
```

#### accept - Accept connection
- **x86_64**: 43
- **ARM64**: 202
- **Arguments**: `(sockfd: u64, addr: ptr, addrlen: ptr)`
- **Returns**: New socket file descriptor, or negative on error

```lwanga
fn accept(sockfd: u64, addr: ptr, addrlen: ptr) -> u64 {
    unsafe {
        return syscall(43, sockfd, addr, addrlen);
    }
}
```

#### connect - Connect to address
- **x86_64**: 42
- **ARM64**: 203
- **Arguments**: `(sockfd: u64, addr: ptr, addrlen: u64)`
- **Returns**: 0 on success, negative on error

```lwanga
fn connect(sockfd: u64, addr: ptr, addrlen: u64) -> u64 {
    unsafe {
        return syscall(42, sockfd, addr, addrlen);
    }
}
```

## Constants

### File Flags (for open)
```lwanga
const O_RDONLY: u64 = 0;
const O_WRONLY: u64 = 1;
const O_RDWR: u64 = 2;
const O_CREAT: u64 = 64;
const O_TRUNC: u64 = 512;
const O_APPEND: u64 = 1024;
```

### Socket Domains
```lwanga
const AF_INET: u64 = 2;      // IPv4
const AF_INET6: u64 = 10;    // IPv6
```

### Socket Types
```lwanga
const SOCK_STREAM: u64 = 1;  // TCP
const SOCK_DGRAM: u64 = 2;   // UDP
const SOCK_RAW: u64 = 3;     // Raw socket
```

### Memory Protection (for mmap)
```lwanga
const PROT_READ: u64 = 1;
const PROT_WRITE: u64 = 2;
const PROT_EXEC: u64 = 4;
const PROT_NONE: u64 = 0;
```

### Memory Mapping Flags (for mmap)
```lwanga
const MAP_SHARED: u64 = 1;
const MAP_PRIVATE: u64 = 2;
const MAP_ANONYMOUS: u64 = 32;
const MAP_FIXED: u64 = 16;
```

## Complete Syscall Tables

### x86_64 Linux Syscalls

| Number | Name | Arguments |
|--------|------|-----------|
| 0 | read | fd, buf, count |
| 1 | write | fd, buf, count |
| 2 | open | filename, flags, mode |
| 3 | close | fd |
| 9 | mmap | addr, length, prot, flags, fd, offset |
| 11 | munmap | addr, length |
| 41 | socket | domain, type, protocol |
| 42 | connect | sockfd, addr, addrlen |
| 43 | accept | sockfd, addr, addrlen |
| 49 | bind | sockfd, addr, addrlen |
| 50 | listen | sockfd, backlog |
| 57 | fork | - |
| 59 | execve | filename, argv, envp |
| 60 | exit | status |

### ARM64 Linux Syscalls

| Number | Name | Arguments |
|--------|------|-----------|
| 56 | openat | dirfd, filename, flags, mode |
| 57 | close | fd |
| 63 | read | fd, buf, count |
| 64 | write | fd, buf, count |
| 93 | exit | status |
| 198 | socket | domain, type, protocol |
| 200 | bind | sockfd, addr, addrlen |
| 201 | listen | sockfd, backlog |
| 202 | accept | sockfd, addr, addrlen |
| 203 | connect | sockfd, addr, addrlen |
| 215 | munmap | addr, length |
| 220 | fork | - |
| 221 | execve | filename, argv, envp |
| 222 | mmap | addr, length, prot, flags, fd, offset |

## Platform-Specific Code

Use preprocessor directives to write platform-specific code:

```lwanga
#if ARCH_X86_64
fn exit(code: u64) -> u64 {
    unsafe {
        return syscall(60, code, 0, 0);
    }
}
#endif

#if ARCH_ARM64
fn exit(code: u64) -> u64 {
    unsafe {
        return syscall(93, code, 0, 0);
    }
}
#endif
```

## Resources

- [Linux Syscall Reference (x86_64)](https://filippo.io/linux-syscall-table/)
- [Linux Syscall Reference (ARM64)](https://arm64.syscall.sh/)
- [Linux man pages](https://man7.org/linux/man-pages/)
