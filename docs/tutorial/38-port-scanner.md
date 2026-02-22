# Lesson 38: Port Scanner

**In this lesson, you'll build a network port scanner to detect open ports on target systems, learning about network reconnaissance and security scanning.**

---

## Project Overview

We'll build a port scanner that:

- **Scans port ranges** - Check multiple ports efficiently
- **Detects open ports** - Identify listening services
- **Reports results** - Display findings clearly
- **Handles errors** - Deal with timeouts and failures
- **Optimizes performance** - Scan quickly

**Why build a port scanner?**
- Essential security tool
- Learn network reconnaissance
- Understand service detection
- Practice socket programming
- Build security awareness

**Design choice:** We'll use TCP connect scanning, which is reliable and doesn't require special privileges (unlike SYN scanning).

---

## Understanding Port Scanning

### What is Port Scanning?

Port scanning is the process of checking which network ports are open on a target system:

- **Open port** - Service is listening, accepts connections
- **Closed port** - No service listening, connection refused
- **Filtered port** - Firewall blocks connection, timeout

### Common Port Ranges

```
Well-known ports: 1-1023
- 21: FTP
- 22: SSH
- 23: Telnet
- 25: SMTP
- 53: DNS
- 80: HTTP
- 443: HTTPS
- 3306: MySQL
- 5432: PostgreSQL
- 8080: HTTP alternate

Registered ports: 1024-49151
- 3000: Node.js dev server
- 5000: Flask dev server
- 8000: Python HTTP server

Dynamic ports: 49152-65535
- Temporary/ephemeral ports
```

### TCP Connect Scan

Our scanner uses TCP connect scanning:

1. Create socket
2. Try to connect to port
3. If connection succeeds → port is open
4. If connection fails → port is closed
5. Close socket

**Advantages:**
- Works without root privileges
- Reliable detection
- Simple to implement

**Disadvantages:**
- Slower than SYN scan
- Leaves logs on target system
- Easy to detect

---

## System Call Constants

```lwanga
const SYS_SOCKET: u64 = 41;
const SYS_CONNECT: u64 = 42;
const SYS_CLOSE: u64 = 3;
const SYS_WRITE: u64 = 1;

const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;
const STDOUT: u64 = 1;
```

---

## Socket Address Structure

```lwanga
packed struct sockaddr_in {
    sin_family: u16,
    sin_port: u16,
    sin_addr: u32,
    sin_zero: [u8; 8],
}

fn htons(port: u16) -> u16 {
    return ((port & 0xFF) << 8) | ((port >> 8) & 0xFF);
}
```

---

## Core Scanning Function

### scan_port() - Check Single Port

```lwanga
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
```

**How it works:**

1. **Create socket** - Get a new socket file descriptor
2. **Setup address** - Configure target IP and port
3. **Try connect** - Attempt TCP connection
4. **Check result** - 0 = success (open), -1 = failure (closed)
5. **Close socket** - Clean up resources

---

## Output Functions

### print_number() - Display Port Numbers

```lwanga
fn print_number(n: u64) -> u64 {
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
    
    // Convert to digits (reversed)
    while num > 0 {
        buffer[i] = (num % 10) as u8 + 48;
        num = num / 10;
        i = i + 1;
    }
    
    // Reverse digits
    let mut j: u64 = 0;
    while j < i / 2 {
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
```

**Algorithm:**
1. Handle zero case
2. Extract digits using modulo 10
3. Digits are in reverse order
4. Reverse the buffer
5. Print the result

---

## Complete Scanner

```lwanga
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

## Understanding IP Addresses

### IP Address Format

```lwanga
// 127.0.0.1 in different formats:

// Dotted decimal (human-readable)
// 127.0.0.1

// Hexadecimal
// 0x7F000001

// Little-endian (x86)
// 0x0100007F

// Bytes
// [127, 0, 0, 1] → stored as [1, 0, 0, 127]
```

**Why 0x0100007F?**

IP addresses are stored in network byte order (big-endian), but x86 is little-endian:
- 127.0.0.1 = 0x7F 00 00 01 (big-endian)
- Stored as: 0x01 00 00 7F (little-endian)

### Common IP Addresses

```lwanga
const LOCALHOST: u32 = 0x0100007F;      // 127.0.0.1
const BROADCAST: u32 = 0xFFFFFFFF;      // 255.255.255.255
const ANY: u32 = 0x00000000;            // 0.0.0.0

// Private networks
const PRIVATE_10: u32 = 0x0A000000;     // 10.0.0.0
const PRIVATE_172: u32 = 0xAC100000;    // 172.16.0.0
const PRIVATE_192: u32 = 0xC0A80000;    // 192.168.0.0
```

---

## Enhanced Features

### Feature 1: Progress Indicator

```lwanga
fn scan_with_progress(ip: u32, start: u16, end: u16) -> u64 {
    let total: u16 = end - start + 1;
    let mut scanned: u16 = 0;
    let mut port: u16 = start;
    
    while port <= end {
        let is_open: u64 = scan_port(ip, port);
        
        if is_open == 1 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Port ", 5);
            }
            print_number(port as u64);
            unsafe {
                syscall(SYS_WRITE, STDOUT, " is OPEN\n", 9);
            }
        }
        
        scanned = scanned + 1;
        
        // Print progress every 100 ports
        if scanned % 100 == 0 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Scanned ", 8);
            }
            print_number(scanned as u64);
            unsafe {
                syscall(SYS_WRITE, STDOUT, " ports\n", 7);
            }
        }
        
        port = port + 1;
    }
    
    return 0;
}
```

### Feature 2: Service Detection

```lwanga
fn get_service_name(port: u16) -> ptr {
    if port == 21 {
        return "FTP";
    } else if port == 22 {
        return "SSH";
    } else if port == 23 {
        return "Telnet";
    } else if port == 25 {
        return "SMTP";
    } else if port == 53 {
        return "DNS";
    } else if port == 80 {
        return "HTTP";
    } else if port == 443 {
        return "HTTPS";
    } else if port == 3306 {
        return "MySQL";
    } else if port == 5432 {
        return "PostgreSQL";
    } else if port == 8080 {
        return "HTTP-Alt";
    }
    
    return "Unknown";
}

fn scan_with_service(ip: u32, port: u16) -> u64 {
    let is_open: u64 = scan_port(ip, port);
    
    if is_open == 1 {
        unsafe {
            syscall(SYS_WRITE, STDOUT, "Port ", 5);
        }
        print_number(port as u64);
        
        let service: ptr = get_service_name(port);
        unsafe {
            syscall(SYS_WRITE, STDOUT, " (", 2);
            syscall(SYS_WRITE, STDOUT, service, 10);  // Max service name length
            syscall(SYS_WRITE, STDOUT, ") is OPEN\n", 10);
        }
    }
    
    return 0;
}
```

### Feature 3: Multiple Targets

```lwanga
fn scan_multiple_hosts() -> u64 {
    let targets: [u32; 3] = [
        0x0100007F,  // 127.0.0.1
        0x0100A8C0,  // 192.168.0.1
        0x0101A8C0,  // 192.168.1.1
    ];
    
    let mut i: u64 = 0;
    
    while i < 3 {
        unsafe {
            syscall(SYS_WRITE, STDOUT, "\nScanning target ", 17);
        }
        print_number(i + 1);
        unsafe {
            syscall(SYS_WRITE, STDOUT, "\n", 1);
        }
        
        let mut port: u16 = 1;
        while port <= 1024 {
            scan_port(targets[i], port);
            port = port + 1;
        }
        
        i = i + 1;
    }
    
    return 0;
}
```

---

## Usage Examples

### Basic Scan

```bash
# Compile
lwangac port_scanner.lwanga -o port_scanner

# Scan localhost ports 1-1024
./port_scanner
```

**Output:**
```
Scanning ports...
Port 22 is OPEN
Port 80 is OPEN
Port 443 is OPEN
Scan complete!
```

### Scan Specific Range

Modify the code to scan different ranges:

```lwanga
let start_port: u16 = 8000;
let end_port: u16 = 9000;
```

### Scan Different Target

```lwanga
// Scan 192.168.1.1
let target_ip: u32 = 0x0101A8C0;
```

---

## Common Mistakes

### Mistake 1: Wrong Byte Order

```lwanga
// WRONG: Port not converted
let addr: sockaddr_in = sockaddr_in {
    sin_family: AF_INET as u16,
    sin_port: 80,  // Should be htons(80)!
    sin_addr: ip,
    sin_zero: [0; 8],
};
```

**Fix:** Always use `htons()` for ports.

### Mistake 2: Not Closing Sockets

```lwanga
// WRONG: Socket leak
fn scan_port_buggy(ip: u32, port: u16) -> u64 {
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    
    // ... connect ...
    
    return result;  // Forgot to close socket!
}
```

**Fix:** Always close sockets, even on error.

### Mistake 3: Scanning Too Fast

```lwanga
// WRONG: May overwhelm network or trigger IDS
while port <= 65535 {
    scan_port(ip, port);
    port = port + 1;
}
```

**Fix:** Add delays or limit concurrent scans.

### Mistake 4: Ignoring Errors

```lwanga
// WRONG: Not checking socket creation
let sockfd: u64 = 0;
unsafe {
    sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
}
// What if sockfd == -1?
```

**Fix:** Always check return values.

---

## Exercises

### Exercise 1: Add Scan Statistics

Add counters for open, closed, and total ports scanned.

<details>
<summary>Solution</summary>

```lwanga
fn scan_with_stats(ip: u32, start: u16, end: u16) -> u64 {
    let mut total: u64 = 0;
    let mut open: u64 = 0;
    let mut closed: u64 = 0;
    
    let mut port: u16 = start;
    
    while port <= end {
        let is_open: u64 = scan_port(ip, port);
        
        total = total + 1;
        
        if is_open == 1 {
            open = open + 1;
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Port ", 5);
            }
            print_number(port as u64);
            unsafe {
                syscall(SYS_WRITE, STDOUT, " is OPEN\n", 9);
            }
        } else {
            closed = closed + 1;
        }
        
        port = port + 1;
    }
    
    // Print statistics
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\nStatistics:\n", 13);
        syscall(SYS_WRITE, STDOUT, "Total: ", 7);
    }
    print_number(total);
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\nOpen: ", 7);
    }
    print_number(open);
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\nClosed: ", 9);
    }
    print_number(closed);
    unsafe {
        syscall(SYS_WRITE, STDOUT, "\n", 1);
    }
    
    return 0;
}
```
</details>

### Exercise 2: Scan Common Ports Only

Create a scanner that only checks well-known ports.

<details>
<summary>Solution</summary>

```lwanga
fn scan_common_ports(ip: u32) -> u64 {
    let common_ports: [u16; 10] = [
        21,    // FTP
        22,    // SSH
        23,    // Telnet
        25,    // SMTP
        80,    // HTTP
        443,   // HTTPS
        3306,  // MySQL
        5432,  // PostgreSQL
        8080,  // HTTP-Alt
        8443,  // HTTPS-Alt
    ];
    
    let mut i: u64 = 0;
    
    while i < 10 {
        let port: u16 = common_ports[i];
        let is_open: u64 = scan_port(ip, port);
        
        if is_open == 1 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Port ", 5);
            }
            print_number(port as u64);
            
            let service: ptr = get_service_name(port);
            unsafe {
                syscall(SYS_WRITE, STDOUT, " (", 2);
                syscall(SYS_WRITE, STDOUT, service, 10);
                syscall(SYS_WRITE, STDOUT, ") is OPEN\n", 10);
            }
        }
        
        i = i + 1;
    }
    
    return 0;
}
```
</details>

### Exercise 3: Add Timeout Support

Implement connection timeout to avoid hanging on filtered ports.

<details>
<summary>Solution</summary>

```lwanga
const SYS_SETSOCKOPT: u64 = 54;
const SOL_SOCKET: u32 = 1;
const SO_SNDTIMEO: u32 = 21;

struct timeval {
    tv_sec: u64,
    tv_usec: u64,
}

fn scan_port_with_timeout(ip: u32, port: u16, timeout_sec: u64) -> u64 {
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    
    if sockfd == -1 {
        return 0;
    }
    
    // Set timeout
    let timeout: timeval = timeval {
        tv_sec: timeout_sec,
        tv_usec: 0,
    };
    
    unsafe {
        syscall(SYS_SETSOCKOPT, sockfd, SOL_SOCKET, SO_SNDTIMEO, &timeout, 16);
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
    
    unsafe {
        syscall(SYS_CLOSE, sockfd, 0, 0);
    }
    
    if result == 0 {
        return 1;
    }
    
    return 0;
}

fn main() -> u64 {
    let target_ip: u32 = 0x0100007F;
    let mut port: u16 = 1;
    
    while port <= 1024 {
        let is_open: u64 = scan_port_with_timeout(target_ip, port, 2);
        
        if is_open == 1 {
            unsafe {
                syscall(SYS_WRITE, STDOUT, "Port ", 5);
            }
            print_number(port as u64);
            unsafe {
                syscall(SYS_WRITE, STDOUT, " is OPEN\n", 9);
            }
        }
        
        port = port + 1;
    }
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Network port scanning fundamentals
- ✅ TCP connect scan implementation
- ✅ Socket programming for reconnaissance
- ✅ IP address and byte order handling
- ✅ Service detection techniques
- ✅ Error handling in network code
- ✅ Building practical security tools

**Remember:** Port scanning can be detected and may be illegal without authorization. Only scan systems you own or have permission to test.

---

## Next Lesson

Now that you can scan ports, let's build a memory dumper in [Lesson 39: Memory Dumper](39-memory-dumper.md).

---

**Navigation**: [← Previous: TCP Server](37-tcp-server.md) | [Tutorial Home](README.md) | [Next: Memory Dumper →](39-memory-dumper.md)

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
