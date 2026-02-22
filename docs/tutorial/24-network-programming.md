# Lesson 24: Network Programming

**In this lesson, you'll learn network programming with sockets: creating connections, building servers and clients, and understanding TCP/IP networking at a low level.**

---

## Understanding Sockets

A socket is an endpoint for network communication. Think of it as a phone - you need one to make or receive calls.

**Socket types:**
- **Stream sockets (TCP)** - Reliable, ordered, connection-oriented
- **Datagram sockets (UDP)** - Unreliable, unordered, connectionless
- **Raw sockets** - Direct access to IP layer

**Why use sockets?**
- Build network applications
- Create servers and clients
- Implement custom protocols
- Network security tools
- Penetration testing

---

## Socket System Calls

### Creating a Socket

```lwanga
const SYS_SOCKET: u64 = 41;

// Address families
const AF_INET: u64 = 2;   // IPv4
const AF_INET6: u64 = 10; // IPv6

// Socket types
const SOCK_STREAM: u64 = 1;  // TCP
const SOCK_DGRAM: u64 = 2;   // UDP
const SOCK_RAW: u64 = 3;     // Raw

fn create_tcp_socket() -> u64 {
    unsafe {
        return syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
}

fn create_udp_socket() -> u64 {
    unsafe {
        return syscall(SYS_SOCKET, AF_INET, SOCK_DGRAM, 0);
    }
}

fn main() -> u64 {
    let sockfd: u64 = create_tcp_socket();
    
    if sockfd > 0xFFFFFFFFFFFF0000 {
        return 1;  // Error
    }
    
    return sockfd;
}
```

**Signature:**
```c
int socket(int domain, int type, int protocol);
```

**Returns:** Socket file descriptor on success, -1 on error

---

## Socket Address Structure

Network addresses use the `sockaddr_in` structure:

```lwanga
packed struct sockaddr_in {
    sin_family: u16,    // Address family (AF_INET)
    sin_port: u16,      // Port number (network byte order)
    sin_addr: u32,      // IP address (network byte order)
    sin_zero: [u8; 8],  // Padding to match sockaddr size
}
```

**Why packed?**

The structure must match the kernel's exact layout for syscalls.

### Byte Order Conversion

Network byte order is big-endian. x86_64 is little-endian. We need to convert:

```lwanga
fn htons(value: u16) -> u16 {
    // Host to network short (16-bit)
    let low: u8 = (value & 0xFF) as u8;
    let high: u8 = ((value >> 8) & 0xFF) as u8;
    
    return ((low as u16) << 8) | (high as u16);
}

fn htonl(value: u32) -> u32 {
    // Host to network long (32-bit)
    let b0: u8 = (value & 0xFF) as u8;
    let b1: u8 = ((value >> 8) & 0xFF) as u8;
    let b2: u8 = ((value >> 16) & 0xFF) as u8;
    let b3: u8 = ((value >> 24) & 0xFF) as u8;
    
    return ((b0 as u32) << 24) | ((b1 as u32) << 16) | 
           ((b2 as u32) << 8) | (b3 as u32);
}

fn main() -> u64 {
    let port: u16 = 8080;
    let network_port: u16 = htons(port);
    
    return network_port as u64;
}
```

### IP Address Conversion

```lwanga
fn ip_to_u32(a: u8, b: u8, c: u8, d: u8) -> u32 {
    // Convert 192.168.1.1 to u32
    return ((a as u32) << 0) | ((b as u32) << 8) | 
           ((c as u32) << 16) | ((d as u32) << 24);
}

fn main() -> u64 {
    let localhost: u32 = ip_to_u32(127, 0, 0, 1);
    return localhost as u64;
}
```

---

## Building a TCP Server

### Step 1: Create Socket

```lwanga
const SYS_SOCKET: u64 = 41;
const AF_INET: u64 = 2;
const SOCK_STREAM: u64 = 1;

fn create_server_socket() -> u64 {
    unsafe {
        return syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
}
```

### Step 2: Bind to Port

```lwanga
const SYS_BIND: u64 = 49;

fn bind_socket(sockfd: u64, port: u16) -> u64 {
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: 0,  // INADDR_ANY (0.0.0.0 - all interfaces)
        sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
    };
    
    unsafe {
        return syscall(SYS_BIND, sockfd, &addr, 16);
    }
}
```

**What is INADDR_ANY?**

`0.0.0.0` means "listen on all network interfaces". The server accepts connections from any IP address.

### Step 3: Listen for Connections

```lwanga
const SYS_LISTEN: u64 = 50;

fn listen_socket(sockfd: u64, backlog: u64) -> u64 {
    unsafe {
        return syscall(SYS_LISTEN, sockfd, backlog, 0);
    }
}
```

**Backlog:** Maximum number of pending connections in the queue.

### Step 4: Accept Connections

```lwanga
const SYS_ACCEPT: u64 = 43;

fn accept_connection(sockfd: u64) -> u64 {
    unsafe {
        return syscall(SYS_ACCEPT, sockfd, 0, 0);
    }
}
```

**Blocking:** `accept` blocks until a client connects.

### Complete TCP Server

```lwanga
const SYS_SOCKET: u64 = 41;
const SYS_BIND: u64 = 49;
const SYS_LISTEN: u64 = 50;
const SYS_ACCEPT: u64 = 43;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;
const SYS_CLOSE: u64 = 3;

const AF_INET: u64 = 2;
const SOCK_STREAM: u64 = 1;

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
    unsafe {
        // Create socket
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        // Bind to port
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: 0,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        let bind_result: u64 = syscall(SYS_BIND, sockfd, &addr, 16);
        
        if bind_result > 0xFFFFFFFFFFFF0000 {
            syscall(SYS_CLOSE, sockfd, 0, 0);
            return 0;
        }
        
        // Listen
        let listen_result: u64 = syscall(SYS_LISTEN, sockfd, 10);
        
        if listen_result > 0xFFFFFFFFFFFF0000 {
            syscall(SYS_CLOSE, sockfd, 0, 0);
            return 0;
        }
        
        return sockfd;
    }
}

fn handle_client(clientfd: u64) -> u64 {
    let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
    
    unsafe {
        // Read from client
        let bytes_read: u64 = syscall(SYS_READ, clientfd, &buffer, 1024);
        
        if bytes_read > 0 && bytes_read < 0xFFFFFFFFFFFF0000 {
            // Echo back
            syscall(SYS_WRITE, clientfd, &buffer, bytes_read);
        }
        
        // Close connection
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    // Accept loop
    unsafe {
        while 1 == 1 {
            let clientfd: u64 = syscall(SYS_ACCEPT, sockfd, 0, 0);
            
            if clientfd > 0 && clientfd < 0xFFFFFFFFFFFF0000 {
                handle_client(clientfd);
            }
        }
    }
    
    return 0;
}
```

---

## Building a TCP Client

### Connect to Server

```lwanga
const SYS_CONNECT: u64 = 42;

fn connect_to_server(ip: u32, port: u16) -> u64 {
    unsafe {
        // Create socket
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        // Setup address
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: ip,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        // Connect
        let result: u64 = syscall(SYS_CONNECT, sockfd, &addr, 16);
        
        if result > 0xFFFFFFFFFFFF0000 {
            syscall(SYS_CLOSE, sockfd, 0, 0);
            return 0;
        }
        
        return sockfd;
    }
}

fn main() -> u64 {
    let localhost: u32 = 0x0100007F;  // 127.0.0.1
    let sockfd: u64 = connect_to_server(localhost, 8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    // Send data
    let msg: ptr = "Hello, Server!\n";
    unsafe {
        syscall(SYS_WRITE, sockfd, msg, 15);
        
        // Read response
        let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        
        syscall(SYS_READ, sockfd, &buffer, 1024);
        
        // Close
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    
    return 0;
}
```

---

## Practical Examples

### Example 1: HTTP Client

```lwanga
fn http_get(host_ip: u32, port: u16, path: ptr) -> u64 {
    unsafe {
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 1;
        }
        
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: host_ip,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        syscall(SYS_CONNECT, sockfd, &addr, 16);
        
        // Send HTTP request
        let request: ptr = "GET / HTTP/1.0\r\n\r\n";
        syscall(SYS_WRITE, sockfd, request, 18);
        
        // Read response
        let mut buffer: [u8; 4096] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                       0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
        
        let bytes: u64 = syscall(SYS_READ, sockfd, &buffer, 4096);
        
        // Print response
        syscall(SYS_WRITE, 1, &buffer, bytes);
        
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let localhost: u32 = 0x0100007F;
    return http_get(localhost, 80, "/");
}
```

### Example 2: Port Scanner

```lwanga
fn scan_port(ip: u32, port: u16) -> u64 {
    unsafe {
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: ip,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        let result: u64 = syscall(SYS_CONNECT, sockfd, &addr, 16);
        
        syscall(SYS_CLOSE, sockfd, 0, 0);
        
        if result == 0 {
            return 1;  // Port open
        }
        
        return 0;  // Port closed
    }
}

fn main() -> u64 {
    let target: u32 = 0x0100007F;
    let mut port: u16 = 1;
    let mut open_count: u64 = 0;
    
    while port <= 1024 {
        if scan_port(target, port) == 1 {
            open_count = open_count + 1;
        }
        port = port + 1;
    }
    
    return open_count;
}
```

### Example 3: Chat Server

```lwanga
fn broadcast_message(clients: ptr, count: u64, msg: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < count {
            let client_ptr: ptr = (clients as u64 + i * 8) as ptr;
            let clientfd: u64 = *(client_ptr as ptr);
            
            syscall(SYS_WRITE, clientfd, msg, len);
            
            i = i + 1;
        }
    }
    
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Forgetting Byte Order Conversion

```lwanga
// WRONG: Using host byte order
let addr: sockaddr_in = sockaddr_in {
    sin_port: 8080,  // Wrong!
    ...
};
```

**Fix:**
```lwanga
let addr: sockaddr_in = sockaddr_in {
    sin_port: htons(8080),  // Correct
    ...
};
```

### Mistake 2: Not Checking Socket Creation

```lwanga
// WRONG: Not checking if socket creation succeeded
let sockfd: u64 = create_socket();
bind_socket(sockfd, 8080);  // Might fail!
```

**Fix:**
```lwanga
let sockfd: u64 = create_socket();

if sockfd > 0xFFFFFFFFFFFF0000 {
    return 1;  // Handle error
}

bind_socket(sockfd, 8080);
```

### Mistake 3: Not Closing Sockets

```lwanga
// WRONG: Socket leak
fn connect_and_send() -> u64 {
    let sockfd: u64 = create_socket();
    // ... use socket ...
    return 0;  // Socket never closed!
}
```

**Fix:**
```lwanga
fn connect_and_send() -> u64 {
    let sockfd: u64 = create_socket();
    // ... use socket ...
    unsafe {
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    return 0;
}
```

---

## Exercises

### Exercise 1: Echo Server

Build a server that echoes back whatever it receives.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    unsafe {
        while 1 == 1 {
            let clientfd: u64 = syscall(SYS_ACCEPT, sockfd, 0, 0);
            
            if clientfd > 0 && clientfd < 0xFFFFFFFFFFFF0000 {
                let mut buffer: [u8; 1024] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                               0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
                
                let bytes: u64 = syscall(SYS_READ, clientfd, &buffer, 1024);
                
                if bytes > 0 {
                    syscall(SYS_WRITE, clientfd, &buffer, bytes);
                }
                
                syscall(SYS_CLOSE, clientfd, 0, 0);
            }
        }
    }
    
    return 0;
}
```
</details>

### Exercise 2: Simple HTTP Server

Create a server that responds with "HTTP/1.0 200 OK\r\n\r\nHello!".

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    let response: ptr = "HTTP/1.0 200 OK\r\n\r\nHello!";
    
    unsafe {
        while 1 == 1 {
            let clientfd: u64 = syscall(SYS_ACCEPT, sockfd, 0, 0);
            
            if clientfd > 0 && clientfd < 0xFFFFFFFFFFFF0000 {
                syscall(SYS_WRITE, clientfd, response, 29);
                syscall(SYS_CLOSE, clientfd, 0, 0);
            }
        }
    }
    
    return 0;
}
```
</details>

### Exercise 3: Port Availability Checker

Write a function that checks if a port is available (not in use).

<details>
<summary>Solution</summary>

```lwanga
fn is_port_available(port: u16) -> u64 {
    unsafe {
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: 0,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        let result: u64 = syscall(SYS_BIND, sockfd, &addr, 16);
        
        syscall(SYS_CLOSE, sockfd, 0, 0);
        
        if result == 0 {
            return 1;  // Available
        }
        
        return 0;  // In use
    }
}

fn main() -> u64 {
    return is_port_available(8080);
}
```
</details>

### Exercise 4: TCP Client with Timeout

Create a client that connects with a timeout (non-blocking).

<details>
<summary>Solution</summary>

```lwanga
const SYS_FCNTL: u64 = 72;
const F_SETFL: u64 = 4;
const O_NONBLOCK: u64 = 2048;

fn connect_with_timeout(ip: u32, port: u16) -> u64 {
    unsafe {
        let sockfd: u64 = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
        
        if sockfd > 0xFFFFFFFFFFFF0000 {
            return 0;
        }
        
        // Set non-blocking
        syscall(SYS_FCNTL, sockfd, F_SETFL, O_NONBLOCK);
        
        let addr: sockaddr_in = sockaddr_in {
            sin_family: AF_INET as u16,
            sin_port: htons(port),
            sin_addr: ip,
            sin_zero: [0, 0, 0, 0, 0, 0, 0, 0],
        };
        
        syscall(SYS_CONNECT, sockfd, &addr, 16);
        
        // In real implementation, would use select/poll to wait
        
        return sockfd;
    }
}

fn main() -> u64 {
    let localhost: u32 = 0x0100007F;
    return connect_with_timeout(localhost, 8080);
}
```
</details>

### Exercise 5: Multi-Client Server

Build a server that can handle multiple clients simultaneously using fork.

<details>
<summary>Solution</summary>

```lwanga
const SYS_FORK: u64 = 57;
const SYS_EXIT: u64 = 60;

fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    unsafe {
        while 1 == 1 {
            let clientfd: u64 = syscall(SYS_ACCEPT, sockfd, 0, 0);
            
            if clientfd > 0 && clientfd < 0xFFFFFFFFFFFF0000 {
                let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
                
                if pid == 0 {
                    // Child process handles client
                    syscall(SYS_CLOSE, sockfd, 0, 0);
                    handle_client(clientfd);
                    syscall(SYS_EXIT, 0, 0, 0);
                } else {
                    // Parent closes client socket
                    syscall(SYS_CLOSE, clientfd, 0, 0);
                }
            }
        }
    }
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Socket concepts and types (TCP, UDP, raw)
- ✅ Creating sockets with socket()
- ✅ Binding to ports with bind()
- ✅ Listening for connections with listen()
- ✅ Accepting connections with accept()
- ✅ Connecting to servers with connect()
- ✅ Byte order conversion (htons, htonl)
- ✅ Building complete TCP servers and clients
- ✅ Practical examples: HTTP client, port scanner
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand network programming, let's learn about signal handling in [Lesson 25: Signal Handling](25-signals.md).

---

**Navigation**: [← Previous: Process Management](23-process-management.md) | [Tutorial Home](README.md) | [Next: Signals →](25-signals.md)
