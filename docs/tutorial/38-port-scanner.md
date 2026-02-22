# Lesson 38: Port Scanner

**Complete project: Build a network port scanner.**

---

## Project Overview

Build a port scanner that:
- Scans a range of ports
- Detects open ports
- Reports results
- Handles timeouts

---

## Complete Code

```lwanga
const SYS_SOCKET: u64 = 41;
const SYS_CONNECT: u64 = 42;
const SYS_CLOSE: u64 = 3;
const SYS_WRITE: u64 = 1;

const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;
const STDOUT: u64 = 1;

packed struct sockaddr_in {
    sin_family: u16,
    sin_port: u16,
    sin_addr: u32,
    sin_zero: [u8; 8],
}

fn htons(port: u16) -> u16 {
    return ((port & 0xFF) << 8) | ((port >> 8) & 0xFF);
}

fn scan_port(ip: u32, port: u16) -> u64 {
    // Create socket
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    
    if sockfd == -1 {
        return 0;  // Failed to create socket
    }
    
    // Setup address
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: ip,
        sin_zero: [0; 8],
    };
    
    // Try to connect
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_CONNECT, sockfd, &addr, 16);
    }
    
    // Close socket
    unsafe {
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    
    if result == 0 {
        return 1;  // Port is open
    }
    
    return 0;  // Port is closed
}

fn print_number(n: u64) -> u64 {
    // Simple number to string conversion
    let buffer: [u8; 20] = [0; 20];
    let mut i: u64 = 0;
    let mut num: u64 = n;
    
    if num == 0 {
        buffer[0] = 48;  // '0'
        unsafe {
            syscall(SYS_WRITE, STDOUT, &buffer, 1);
        }
        return 0;
    }
    
    while num > 0 {
        buffer[i] = (num % 10) as u8 + 48;
        num = num / 10;
        i = i + 1;
    }
    
    // Reverse and print
    let mut j: u64 = 0;
    while j < i {
        let temp: u8 = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
        j = j + 1;
    }
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, &buffer, i);
    }
    
    return 0;
}

fn main() -> u64 {
    let target_ip: u32 = 0x0100007F;  // 127.0.0.1 (localhost)
    let start_port: u16 = 1;
    let end_port: u16 = 1024;
    
    let msg_scanning: ptr = "Scanning ports...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg_scanning, 18);
    }
    
    let mut port: u16 = start_port;
    while port <= end_port {
        let is_open: u64 = scan_port(target_ip, port);
        
        if is_open == 1 {
            let msg_open: ptr = "Port ";
            unsafe {
                syscall(SYS_WRITE, STDOUT, msg_open, 5);
            }
            
            print_number(port as u64);
            
            let msg_is_open: ptr = " is OPEN\n";
            unsafe {
                syscall(SYS_WRITE, STDOUT, msg_is_open, 9);
            }
        }
        
        port = port + 1;
    }
    
    let msg_done: ptr = "Scan complete!\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg_done, 15);
    }
    
    return 0;
}
```

---

## Usage

```bash
# Compile
lwangac port_scanner.lwanga -o port_scanner

# Run
./port_scanner
```

---

**Navigation**: [← Previous: TCP Server](37-tcp-server.md) | [Tutorial Home](README.md) | [Next: Memory Dumper →](39-memory-dumper.md)
