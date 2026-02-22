# Lesson 37: Building a TCP Server

**In this lesson, you'll build a complete, working TCP server from scratch, learning network programming concepts and practical server implementation.**

---

## Project Overview

We'll build a TCP echo server that demonstrates:

- **Socket creation** - Creating network endpoints
- **Binding** - Associating socket with address and port
- **Listening** - Accepting incoming connections
- **Client handling** - Processing client requests
- **Data transfer** - Reading and writing network data
- **Resource management** - Properly closing connections

**Why build a TCP server?**
- Fundamental network programming skill
- Foundation for web servers, APIs, chat servers
- Understand client-server architecture
- Learn socket programming concepts
- Build real-world applications

**Design choice:** We'll use raw syscalls for maximum control and minimal dependencies, showing exactly how network programming works at the system level.

---

## Understanding TCP Sockets

### What is a Socket?

A socket is an endpoint for network communication:
- **IP Address** - Identifies the machine (e.g., 127.0.0.1)
- **Port** - Identifies the application (e.g., 8080)
- **Protocol** - TCP (reliable, ordered) or UDP (fast, unordered)

### TCP Server Lifecycle

```
1. socket()  - Create socket
2. bind()    - Bind to address:port
3. listen()  - Mark as passive socket
4. accept()  - Wait for client connection
5. read()    - Receive data from client
6. write()   - Send data to client
7. close()   - Close connection
```

---

## System Call Constants

### Socket Syscalls

```lwanga
const SYS_SOCKET: u64 = 41;   // Create socket
const SYS_BIND: u64 = 49;     // Bind to address
const SYS_LISTEN: u64 = 50;   // Listen for connections
const SYS_ACCEPT: u64 = 43;   // Accept connection
const SYS_READ: u64 = 0;      // Read data
const SYS_WRITE: u64 = 1;     // Write data
const SYS_CLOSE: u64 = 3;     // Close socket
```

**Why these numbers?**
These are Linux x86_64 syscall numbers. Different architectures may use different numbers.

### Socket Constants

```lwanga
const AF_INET: u32 = 2;        // IPv4 address family
const SOCK_STREAM: u32 = 1;    // TCP socket type
```

**Address families:**
- `AF_INET` - IPv4 (e.g., 192.168.1.1)
- `AF_INET6` - IPv6 (e.g., ::1)
- `AF_UNIX` - Unix domain sockets

**Socket types:**
- `SOCK_STREAM` - TCP (connection-oriented, reliable)
- `SOCK_DGRAM` - UDP (connectionless, fast)

---

## Socket Address Structure

### sockaddr_in Structure

```lwanga
packed struct sockaddr_in {
    sin_family: u16,      // Address family (AF_INET)
    sin_port: u16,        // Port number (network byte order)
    sin_addr: u32,        // IP address (network byte order)
    sin_zero: [u8; 8],    // Padding to match sockaddr size
}
```

**Why packed?**
The structure must match the exact layout expected by the kernel, with no padding.

**Fields explained:**
- `sin_family` - Always `AF_INET` for IPv4
- `sin_port` - Port in network byte order (big-endian)
- `sin_addr` - IP address (0 = INADDR_ANY, listen on all interfaces)
- `sin_zero` - Padding bytes, must be zero

---

## Byte Order Conversion

### Network vs Host Byte Order

```lwanga
fn htons(port: u16) -> u16 {
    // Host to Network Short
    // Convert from little-endian to big-endian
    return ((port & 0xFF) << 8) | ((port >> 8) & 0xFF);
}
```

**Why is this needed?**

Different CPUs store multi-byte numbers differently:
- **Little-endian** (x86): Least significant byte first
  - 0x1234 stored as: 34 12
- **Big-endian** (network): Most significant byte first
  - 0x1234 stored as: 12 34

Network protocols use big-endian, so we must convert.

**Example:**
```lwanga
let port: u16 = 8080;           // 0x1F90 in hex
let network_port: u16 = htons(port);  // 0x901F in hex
```

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

```

**Code walkthrough:**

1. **create_server()** - Sets up the server socket
   - Creates socket with `socket()`
   - Binds to port with `bind()`
   - Starts listening with `listen()`

2. **handle_client()** - Processes one client
   - Reads data from client
   - Echoes it back
   - Closes connection

3. **main()** - Server loop
   - Creates server socket
   - Accepts connections in loop
   - Handles each client

---

## Step-by-Step Explanation

### Step 1: Create Socket

```lwanga
let sockfd: u64 = 0;
unsafe {
    sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
}
```

**What this does:**
- Creates a new socket
- `AF_INET` - Use IPv4
- `SOCK_STREAM` - Use TCP
- Returns file descriptor (or -1 on error)

### Step 2: Bind to Port

```lwanga
let addr: sockaddr_in = sockaddr_in {
    sin_family: AF_INET as u16,
    sin_port: htons(port),
    sin_addr: 0,  // INADDR_ANY (all interfaces)
    sin_zero: [0; 8],
};

unsafe {
    let result: u64 = syscall(SYS_BIND, sockfd, &addr, 16);
}
```

**What this does:**
- Associates socket with address and port
- `sin_addr: 0` means listen on all network interfaces
- Size is 16 bytes (sizeof sockaddr_in)

### Step 3: Listen for Connections

```lwanga
unsafe {
    let result: u64 = syscall(SYS_LISTEN, sockfd, 10);
}
```

**What this does:**
- Marks socket as passive (accepting connections)
- `10` is the backlog (max queued connections)

### Step 4: Accept Connections

```lwanga
let clientfd: u64 = 0;
unsafe {
    clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
}
```

**What this does:**
- Blocks until a client connects
- Returns new socket for the client
- Original socket continues listening

### Step 5: Read and Write Data

```lwanga
// Read from client
unsafe {
    bytes_read = syscall(SYS_READ, clientfd, &buffer, 1024);
}

// Echo back to client
unsafe {
    syscall(SYS_WRITE, clientfd, &buffer, bytes_read);
}
```

---

## Testing the Server

### Compile and Run

```bash
# Compile
lwangac tcp_server.lwanga -o tcp_server

# Run server
./tcp_server
```

The server is now listening on port 8080.

### Test with netcat

In another terminal:

```bash
# Send a message
echo "Hello, Server!" | nc localhost 8080

# Interactive session
nc localhost 8080
Hello
Hello
World
World
^C
```

### Test with telnet

```bash
telnet localhost 8080
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Test message
Test message
```

### Test with curl

```bash
curl http://localhost:8080
```

---

## Enhanced Server Features

### Feature 1: Multiple Clients (Sequential)

```lwanga
fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    let mut client_count: u64 = 0;
    
    // Accept loop - handles clients one at a time
    while 1 == 1 {
        let clientfd: u64 = 0;
        unsafe {
            clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
        }
        
        if clientfd != -1 {
            client_count = client_count + 1;
            handle_client(clientfd);
        }
    }
    
    return 0;
}
```

### Feature 2: Logging

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn log_message(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    return 0;
}

fn handle_client_with_logging(clientfd: u64) -> u64 {
    log_message("Client connected\n", 17);
    
    let buffer: [u8; 1024] = [0; 1024];
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, clientfd, &buffer, 1024);
    }
    
    if bytes_read > 0 {
        log_message("Received data\n", 14);
        
        unsafe {
            syscall(SYS_WRITE, clientfd, &buffer, bytes_read);
        }
        
        log_message("Sent response\n", 14);
    }
    
    unsafe {
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    log_message("Client disconnected\n", 20);
    
    return 0;
}
```

### Feature 3: Custom Responses

```lwanga
fn handle_http_client(clientfd: u64) -> u64 {
    let buffer: [u8; 1024] = [0; 1024];
    
    // Read request
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, clientfd, &buffer, 1024);
    }
    
    // Send HTTP response
    let response: ptr = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nHello from Lwanga!\n";
    let response_len: u64 = 68;
    
    unsafe {
        syscall(SYS_WRITE, clientfd, response, response_len);
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    return 0;
}
```

Test with browser: `http://localhost:8080`

---

## Common Mistakes

### Mistake 1: Forgetting Byte Order Conversion

```lwanga
// WRONG: Port in host byte order
let addr: sockaddr_in = sockaddr_in {
    sin_family: AF_INET as u16,
    sin_port: 8080,  // Wrong! Should be htons(8080)
    sin_addr: 0,
    sin_zero: [0; 8],
};
```

**Fix:** Always use `htons()` for port numbers.

### Mistake 2: Not Checking Return Values

```lwanga
// WRONG: Ignoring errors
let sockfd: u64 = 0;
unsafe {
    sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
}
// What if socket creation failed?
```

**Fix:** Always check for -1 (error).

### Mistake 3: Forgetting to Close Sockets

```lwanga
// WRONG: Socket leak
fn handle_client(clientfd: u64) -> u64 {
    // Process client
    return 0;  // Forgot to close!
}
```

**Fix:** Always close sockets when done.

### Mistake 4: Buffer Overflow

```lwanga
// WRONG: No bounds checking
let buffer: [u8; 64] = [0; 64];
unsafe {
    syscall(SYS_READ, clientfd, &buffer, 1024);  // Can overflow!
}
```

**Fix:** Read at most buffer size.

---

## Exercises

### Exercise 1: Add Connection Limit

Modify the server to accept only 10 connections, then exit.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    let mut count: u64 = 0;
    let max_connections: u64 = 10;
    
    while count < max_connections {
        let clientfd: u64 = 0;
        unsafe {
            clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
        }
        
        if clientfd != -1 {
            handle_client(clientfd);
            count = count + 1;
        }
    }
    
    unsafe {
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    
    return 0;
}
```
</details>

### Exercise 2: Add Timeout

Modify the server to close connections that don't send data within 5 seconds.

<details>
<summary>Solution</summary>

```lwanga
const SYS_SETSOCKOPT: u64 = 54;
const SOL_SOCKET: u32 = 1;
const SO_RCVTIMEO: u32 = 20;

struct timeval {
    tv_sec: u64,
    tv_usec: u64,
}

fn set_timeout(sockfd: u64, seconds: u64) -> u64 {
    let timeout: timeval = timeval {
        tv_sec: seconds,
        tv_usec: 0,
    };
    
    unsafe {
        syscall(SYS_SETSOCKOPT, sockfd, SOL_SOCKET, SO_RCVTIMEO, &timeout, 16);
    }
    
    return 0;
}

fn handle_client_with_timeout(clientfd: u64) -> u64 {
    set_timeout(clientfd, 5);
    
    let buffer: [u8; 1024] = [0; 1024];
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, clientfd, &buffer, 1024);
    }
    
    if bytes_read > 0 {
        unsafe {
            syscall(SYS_WRITE, clientfd, &buffer, bytes_read);
        }
    }
    
    unsafe {
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    return 0;
}
```
</details>

### Exercise 3: Build HTTP Server

Create a simple HTTP server that serves a static page.

<details>
<summary>Solution</summary>

```lwanga
fn handle_http_request(clientfd: u64) -> u64 {
    let buffer: [u8; 1024] = [0; 1024];
    
    // Read HTTP request
    unsafe {
        syscall(SYS_READ, clientfd, &buffer, 1024);
    }
    
    // Build HTTP response
    let header: ptr = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n";
    let body: ptr = "<html><body><h1>Hello from Lwanga!</h1></body></html>";
    
    // Send response
    unsafe {
        syscall(SYS_WRITE, clientfd, header, 45);
        syscall(SYS_WRITE, clientfd, body, 54);
        syscall(SYS_CLOSE, clientfd, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    let sockfd: u64 = create_server(8080);
    
    if sockfd == 0 {
        return 1;
    }
    
    while 1 == 1 {
        let clientfd: u64 = 0;
        unsafe {
            clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
        }
        
        if clientfd != -1 {
            handle_http_request(clientfd);
        }
    }
    
    return 0;
}
```

Test: Open `http://localhost:8080` in your browser.
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ TCP socket programming fundamentals
- ✅ Creating and configuring server sockets
- ✅ Accepting and handling client connections
- ✅ Reading and writing network data
- ✅ Byte order conversion (htons)
- ✅ Proper resource management
- ✅ Testing network applications
- ✅ Building real-world servers

**Remember:** Network programming requires careful error handling and resource management. Always check return values and close sockets properly.

---

## Next Lesson

Now that you can build a TCP server, let's create a port scanner in [Lesson 38: Port Scanner](38-port-scanner.md).

---

**Navigation**: [← Previous: Debugging](36-debugging.md) | [Tutorial Home](README.md) | [Next: Port Scanner →](38-port-scanner.md)
