# Lesson 37: Building a TCP Server

**Complete project: Build a working TCP server.**

---

## Project Overview

We'll build a simple TCP server that:
- Listens on port 8080
- Accepts connections
- Echoes back received data
- Handles multiple clients

---

## Complete Code

```lwanga
const SYS_SOCKET: u64 = 41;
const SYS_BIND: u64 = 49;
const SYS_LISTEN: u64 = 50;
const SYS_ACCEPT: u64 = 43;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;

const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;

packed struct sockaddr_in {
    sin_family: u16,
    sin_port: u16,
    sin_addr: u32,
    sin_zero: [u8; 8],
}

fn htons(port: u16) -> u16 {
    return ((port & 0xFF) << 8) | ((port >> 8) & 0xFF);
}

fn create_server(port: u16) -> u64 {
    // Create socket
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    
    if sockfd == -1 {
        return 0;
    }
    
    // Bind to port
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: 0,
        sin_zero: [0; 8],
    };
    
    unsafe {
        let result: u64 = syscall(SYS_BIND, sockfd, &addr, 16);
        if result == -1 {
            return 0;
        }
    }
    
    // Listen
    unsafe {
        let result: u64 = syscall(SYS_LISTEN, sockfd, 10);
        if result == -1 {
            return 0;
        }
    }
    
    return sockfd;
}

fn handle_client(clientfd: u64) -> u64 {
    let buffer: [u8; 1024] = [0; 1024];
    
    // Read data
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, clientfd, &buffer, 1024);
    }
    
    if bytes_read > 0 {
        // Echo back
        unsafe {
            syscall(SYS_WRITE, clientfd, &buffer, bytes_read);
        }
    }
    
    // Close connection
    unsafe {
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;  // Error
    }
    
    // Accept loop
    while 1 == 1 {
        let clientfd: u64 = 0;
        unsafe {
            clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
        }
        
        if clientfd != -1 {
            handle_client(clientfd);
        }
    }
    
    return 0;
}
```

---

## Testing

```bash
# Compile
lwangac tcp_server.lwanga -o tcp_server

# Run server
./tcp_server

# In another terminal, test with netcat
echo "Hello" | nc localhost 8080
```

---

**Navigation**: [← Previous: Debugging](36-debugging.md) | [Tutorial Home](README.md) | [Next: Port Scanner →](38-port-scanner.md)
