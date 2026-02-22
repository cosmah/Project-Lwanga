# Lesson 40: Final Project

**In this final lesson, you'll build a comprehensive security tool suite that combines everything you've learned throughout the tutorial, demonstrating mastery of Lwanga programming.**

---

## Project Overview

We'll build a multi-tool security suite that includes:

- **Port scanning** - Network reconnaissance
- **Banner grabbing** - Service identification
- **Memory analysis** - Process inspection
- **Packet crafting** - Network manipulation
- **Encrypted configuration** - Secure settings
- **Obfuscated logic** - Code protection

**Why this project?**
- Demonstrates all major concepts
- Real-world application
- Professional tool structure
- Security-focused implementation
- Portfolio-worthy project

**Design choice:** We'll use a modular architecture with separate libraries for each feature, showing proper code organization and reusability.

---

## Project Architecture

### Directory Structure

```
security-suite/
├── main.lwanga              # Main program
├── lib/
│   ├── network.lwanga       # Network operations
│   ├── scanner.lwanga       # Port scanning
│   ├── memory.lwanga        # Memory operations
│   └── utils.lwanga         # Utility functions
├── config/
│   └── settings.lwanga      # Configuration
└── README.md                # Documentation
```

**Why this structure?**
- **Separation of concerns** - Each module has one responsibility
- **Reusability** - Libraries can be used in other projects
- **Maintainability** - Easy to find and update code
- **Scalability** - Easy to add new features

---

## Module 1: Network Library

### lib/network.lwanga

```lwanga
// lib/network.lwanga
// Network operations and socket management

const SYS_SOCKET: u64 = 41;
const SYS_CONNECT: u64 = 42;
const SYS_BIND: u64 = 49;
const SYS_LISTEN: u64 = 50;
const SYS_ACCEPT: u64 = 43;
const SYS_CLOSE: u64 = 3;
const SYS_READ: u64 = 0;
const SYS_WRITE: u64 = 1;

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

fn create_socket() -> u64 {
    let sockfd: u64 = 0;
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    return sockfd;
}

fn connect_to(sockfd: u64, ip: u32, port: u16) -> u64 {
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: ip,
        sin_zero: [0; 8],
    };
    
    let result: u64 = 0;
    unsafe {
        result = syscall(SYS_CONNECT, sockfd, &addr, 16);
    }
    
    return result;
}

fn close_socket(sockfd: u64) -> u64 {
    unsafe {
        return syscall(SYS_CLOSE, sockfd, 0, 0);
    }
}

fn send_data(sockfd: u64, data: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, sockfd, data, len);
    }
}

fn receive_data(sockfd: u64, buffer: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_READ, sockfd, buffer, len);
    }
}
```

**Key features:**
- Socket creation and management
- Connection establishment
- Data transmission
- Proper resource cleanup

---

## Module 2: Scanner Library

### lib/scanner.lwanga

```lwanga
// lib/scanner.lwanga
// Port scanning and service detection

import "lib/network.lwanga";

fn scan_port(ip: u32, port: u16) -> u64 {
    let sockfd: u64 = create_socket();
    
    if sockfd == -1 {
        return 0;
    }
    
    let result: u64 = connect_to(sockfd, ip, port);
    close_socket(sockfd);
    
    if result == 0 {
        return 1;  // Open
    }
    
    return 0;  // Closed
}

fn scan_range(ip: u32, start_port: u16, end_port: u16) -> u64 {
    let mut open_count: u64 = 0;
    let mut port: u16 = start_port;
    
    while port <= end_port {
        if scan_port(ip, port) == 1 {
            open_count = open_count + 1;
        }
        port = port + 1;
    }
    
    return open_count;
}

fn get_service_name(port: u16) -> ptr {
    if port == 21 {
        return "FTP";
    } else if port == 22 {
        return "SSH";
    } else if port == 23 {
        return "Telnet";
    } else if port == 25 {
        return "SMTP";
    } else if port == 80 {
        return "HTTP";
    } else if port == 443 {
        return "HTTPS";
    } else if port == 3306 {
        return "MySQL";
    } else if port == 5432 {
        return "PostgreSQL";
    }
    
    return "Unknown";
}

fn grab_banner(ip: u32, port: u16) -> u64 {
    let sockfd: u64 = create_socket();
    
    if sockfd == -1 {
        return 0;
    }
    
    if connect_to(sockfd, ip, port) != 0 {
        close_socket(sockfd);
        return 0;
    }
    
    // Read banner
    let buffer: [u8; 256] = [0; 256];
    let bytes: u64 = receive_data(sockfd, &buffer as ptr, 256);
    
    close_socket(sockfd);
    
    return bytes;
}
```

**Key features:**
- Single port scanning
- Range scanning
- Service identification
- Banner grabbing

---

## Module 3: Memory Library

### lib/memory.lwanga

```lwanga
// lib/memory.lwanga
// Memory operations and analysis

const SYS_OPEN: u64 = 2;
const SYS_READ: u64 = 0;
const SYS_CLOSE: u64 = 3;
const SYS_LSEEK: u64 = 8;
const O_RDONLY: u32 = 0;

fn read_memory(addr: u64, buffer: ptr, size: u64) -> u64 {
    let path: ptr = "/proc/self/mem";
    let fd: u64 = 0;
    
    unsafe {
        fd = syscall(SYS_OPEN, path, O_RDONLY, 0);
    }
    
    if fd == -1 {
        return 0;
    }
    
    unsafe {
        syscall(SYS_LSEEK, fd, addr, 0);
    }
    
    let bytes_read: u64 = 0;
    unsafe {
        bytes_read = syscall(SYS_READ, fd, buffer, size);
        syscall(SYS_CLOSE, fd, 0, 0);
    }
    
    return bytes_read;
}

fn search_pattern(addr: u64, size: u64, pattern: ptr, pattern_len: u64) -> u64 {
    let buffer: [u8; 1024] = [0; 1024];
    let to_read: u64 = if size > 1024 { 1024 } else { size };
    
    let bytes_read: u64 = read_memory(addr, &buffer as ptr, to_read);
    
    if bytes_read == 0 {
        return 0;
    }
    
    // Search for pattern
    let mut i: u64 = 0;
    
    while i <= bytes_read - pattern_len {
        let mut matches: u64 = 1;
        let mut j: u64 = 0;
        
        unsafe {
            while j < pattern_len {
                if buffer[i + j] != *(pattern + j) {
                    matches = 0;
                    break;
                }
                j = j + 1;
            }
        }
        
        if matches == 1 {
            return addr + i;  // Return address of match
        }
        
        i = i + 1;
    }
    
    return 0;  // Not found
}
```

**Key features:**
- Memory reading
- Pattern searching
- Address validation
- Error handling

---

## Module 4: Utilities Library

### lib/utils.lwanga

```lwanga
// lib/utils.lwanga
// Utility functions

const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;
const STDERR: u64 = 2;

fn print_string(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    return 0;
}

fn print_error(msg: ptr, len: u64) -> u64 {
    unsafe {
        syscall(SYS_WRITE, STDERR, msg, len);
    }
    return 0;
}

fn print_number(n: u64) -> u64 {
    let buffer: [u8; 20] = [0; 20];
    let mut i: u64 = 0;
    let mut num: u64 = n;
    
    if num == 0 {
        buffer[0] = 48;
        print_string(&buffer as ptr, 1);
        return 0;
    }
    
    while num > 0 {
        buffer[i] = (num % 10) as u8 + 48;
        num = num / 10;
        i = i + 1;
    }
    
    // Reverse
    let mut j: u64 = 0;
    while j < i / 2 {
        let temp: u8 = buffer[j];
        buffer[j] = buffer[i - j - 1];
        buffer[i - j - 1] = temp;
        j = j + 1;
    }
    
    print_string(&buffer as ptr, i);
    return 0;
}

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

fn string_compare(s1: ptr, s2: ptr, len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < len {
            if *(s1 + i) != *(s2 + i) {
                return 0;
            }
            i = i + 1;
        }
    }
    
    return 1;
}
```

**Key features:**
- Output functions
- Number formatting
- Hex display
- String operations

---

## Main Program

### main.lwanga

```lwanga
import "lib/network.lwanga";
import "lib/scanner.lwanga";
import "lib/memory.lwanga";
import "lib/utils.lwanga";

const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print_banner() -> u64 {
    let banner: ptr = enc "
╔═══════════════════════════════════╗
║   Lwanga Security Suite v1.0      ║
║   Professional Security Tools     ║
╚═══════════════════════════════════╝
";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, banner, 150);
    }
    
    return 0;
}

fn print_menu() -> u64 {
    let menu: ptr = "
[1] Port Scanner
[2] Banner Grabber
[3] Memory Dumper
[4] Service Detector
[5] Exit

Select option: ";
    
    print_string(menu, 100);
    
    return 0;
}

fn port_scanner_mode() -> u64 {
    print_string("\n[*] Starting port scanner...\n", 30);
    
    let target: u32 = 0x0100007F;  // 127.0.0.1
    let mut port: u16 = 1;
    let mut open_count: u64 = 0;
    
    while port <= 1024 {
        if scan_port(target, port) == 1 {
            print_string("Port ", 5);
            print_number(port as u64);
            
            let service: ptr = get_service_name(port);
            print_string(" (", 2);
            print_string(service, 10);
            print_string(") is OPEN\n", 10);
            
            open_count = open_count + 1;
        }
        
        port = port + 1;
    }
    
    print_string("\nFound ", 7);
    print_number(open_count);
    print_string(" open ports\n", 12);
    
    return 0;
}

fn banner_grabber_mode() -> u64 {
    print_string("\n[*] Starting banner grabber...\n", 33);
    
    let target: u32 = 0x0100007F;
    let ports: [u16; 5] = [21, 22, 80, 443, 8080];
    
    let mut i: u64 = 0;
    
    while i < 5 {
        let port: u16 = ports[i];
        
        print_string("Checking port ", 14);
        print_number(port as u64);
        print_string("...\n", 4);
        
        let bytes: u64 = grab_banner(target, port);
        
        if bytes > 0 {
            print_string("  Banner received (", 19);
            print_number(bytes);
            print_string(" bytes)\n", 8);
        } else {
            print_string("  No banner\n", 12);
        }
        
        i = i + 1;
    }
    
    return 0;
}

fn memory_dumper_mode() -> u64 {
    print_string("\n[*] Starting memory dumper...\n", 32);
    
    // Dump some stack memory
    let stack_var: u64 = 0x4142434445464748;
    let addr: u64 = &stack_var as u64;
    
    print_string("Dumping memory at address: ", 27);
    print_hex_byte((addr >> 56) as u8);
    print_hex_byte((addr >> 48) as u8);
    print_hex_byte((addr >> 40) as u8);
    print_hex_byte((addr >> 32) as u8);
    print_hex_byte((addr >> 24) as u8);
    print_hex_byte((addr >> 16) as u8);
    print_hex_byte((addr >> 8) as u8);
    print_hex_byte(addr as u8);
    print_string("\n\n", 2);
    
    let buffer: [u8; 64] = [0; 64];
    let bytes_read: u64 = read_memory(addr, &buffer as ptr, 64);
    
    if bytes_read > 0 {
        let mut i: u64 = 0;
        while i < bytes_read {
            if i % 16 == 0 {
                print_string("\n", 1);
            }
            print_hex_byte(buffer[i]);
            print_string(" ", 1);
            i = i + 1;
        }
        print_string("\n", 1);
    }
    
    return 0;
}

fn service_detector_mode() -> u64 {
    print_string("\n[*] Starting service detector...\n", 35);
    
    let target: u32 = 0x0100007F;
    let common_ports: [u16; 10] = [
        21, 22, 23, 25, 80, 443, 3306, 5432, 8080, 8443
    ];
    
    let mut i: u64 = 0;
    let mut detected: u64 = 0;
    
    while i < 10 {
        let port: u16 = common_ports[i];
        
        if scan_port(target, port) == 1 {
            let service: ptr = get_service_name(port);
            
            print_string("Detected: ", 10);
            print_string(service, 15);
            print_string(" on port ", 9);
            print_number(port as u64);
            print_string("\n", 1);
            
            detected = detected + 1;
        }
        
        i = i + 1;
    }
    
    if detected == 0 {
        print_string("No services detected\n", 21);
    }
    
    return 0;
}

fn main() -> u64 {
    print_banner();
    
    // Demo mode: run all features
    port_scanner_mode();
    banner_grabber_mode();
    memory_dumper_mode();
    service_detector_mode();
    
    print_string("\n[*] Demo complete!\n", 20);
    
    return 0;
}
```

---

## Building and Running

### Compilation

```bash
# Basic build
lwangac main.lwanga -o security-suite

# Optimized build
lwangac main.lwanga -o security-suite -O2

# With obfuscation
lwangac main.lwanga -o security-suite -O2 --obfuscate

# Position-independent
lwangac main.lwanga -o security-suite -O2 --pic
```

### Running

```bash
# Run the suite
./security-suite

# Check binary size
ls -lh security-suite

# Check dependencies
ldd security-suite
```

---

## Advanced Features

### Feature 1: Encrypted Configuration

```lwanga
// Encrypted API keys and secrets
const API_KEY: ptr = enc "secret_api_key_12345";
const LICENSE: ptr = enc "LWANGA-PRO-2024-XXXX";
const DB_PASSWORD: ptr = enc "super_secret_password";

fn check_license() -> u64 {
    // License validation logic
    let valid_license: ptr = enc "LWANGA-PRO-2024-XXXX";
    
    // Compare encrypted strings
    return string_compare(LICENSE, valid_license, 20);
}
```

### Feature 2: Obfuscated Logic

```lwanga
// Compile with --obfuscate flag
fn validate_access(code: u64) -> u64 {
    // Obfuscated validation
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while state < 5 {
        if state == 0 {
            if code > 1000 {
                state = 1;
            } else {
                state = 4;
            }
        } else if state == 1 {
            if code < 2000 {
                state = 2;
            } else {
                state = 4;
            }
        } else if state == 2 {
            if code == 1337 {
                result = 1;
            }
            state = 5;
        } else if state == 4 {
            result = 0;
            state = 5;
        }
    }
    
    return result;
}
```

### Feature 3: Logging System

```lwanga
#if DEBUG
fn log_event(level: u64, msg: ptr, len: u64) -> u64 {
    if level == 1 {
        print_string("[INFO] ", 7);
    } else if level == 2 {
        print_string("[WARN] ", 7);
    } else if level == 3 {
        print_error("[ERROR] ", 8);
    }
    
    print_string(msg, len);
    print_string("\n", 1);
    
    return 0;
}
#else
fn log_event(level: u64, msg: ptr, len: u64) -> u64 {
    return 0;  // No-op in release
}
#endif
```

---

## Congratulations!

You've completed the Lwanga tutorial! You now know:

### Core Concepts
- ✅ Variables and types
- ✅ Functions and control flow
- ✅ Pointers and memory management
- ✅ Arrays and structs
- ✅ Type system

### Systems Programming
- ✅ System calls
- ✅ File operations
- ✅ Process management
- ✅ Signal handling

### Network Programming
- ✅ Socket programming
- ✅ TCP/IP networking
- ✅ Client-server architecture
- ✅ Protocol implementation

### Security Techniques
- ✅ Inline assembly
- ✅ Encrypted strings
- ✅ Code obfuscation
- ✅ Shellcode development
- ✅ Exploit techniques

### Advanced Topics
- ✅ Preprocessor directives
- ✅ Module organization
- ✅ Optimization techniques
- ✅ Debugging strategies

---

## Next Steps

### Continue Learning

1. **Read the Language Reference**
   - [Language Reference](../language-reference.md)
   - Complete syntax guide
   - Advanced features

2. **Explore Examples**
   - [Example Programs](../../examples/)
   - Real-world code
   - Best practices

3. **Study the Compiler**
   - [Compiler Guide](../compiler.md)
   - Optimization flags
   - Build process

4. **Join the Community**
   - [GitHub Repository](https://github.com/cosmah/Project-Lwanga)
   - Report issues
   - Contribute code

### Build Your Own Projects

Ideas for practice:
- **Web server** - HTTP server with routing
- **Chat application** - Real-time messaging
- **File encryption tool** - Secure file storage
- **Network proxy** - Traffic forwarding
- **System monitor** - Resource tracking
- **Packet sniffer** - Network analysis
- **Reverse shell** - Remote access tool
- **Keylogger** - Input monitoring (educational only!)

### Contribute

Help improve Lwanga:
- Report bugs
- Suggest features
- Write documentation
- Create examples
- Build libraries

---

## Resources

### Documentation
- [Language Reference](../language-reference.md) - Complete syntax
- [Compiler Guide](../compiler.md) - Build system
- [Syscall Reference](../syscalls.md) - System calls
- [Style Guide](../style-guide.md) - Code conventions

### Community
- [GitHub](https://github.com/cosmah/Project-Lwanga) - Source code
- [Issues](https://github.com/cosmah/Project-Lwanga/issues) - Bug reports
- [Contributing](../../CONTRIBUTING.md) - How to help

### Tools
- GDB - Debugging
- Valgrind - Memory analysis
- strace - System call tracing
- objdump - Binary analysis

---

## Final Thoughts

You've learned a powerful, low-level programming language designed for systems programming and security research. Use this knowledge responsibly:

- **Build secure systems** - Write safe, robust code
- **Research vulnerabilities** - Find and fix security issues
- **Educate others** - Share your knowledge
- **Contribute positively** - Make the world more secure

**Remember:** With great power comes great responsibility. Always obtain proper authorization before testing security tools on systems you don't own.

---

<div align="center">

**🎉 Congratulations on completing the Lwanga Tutorial! 🎉**

You're now ready to build professional security tools and systems programs.

Happy coding, and welcome to the Lwanga community!

</div>

---

**Navigation**: [← Previous: Memory Dumper](39-memory-dumper.md) | [Tutorial Home](README.md) | [Documentation Index](../README.md)

```lwanga
import "lib/network.lwanga";
import "lib/scanner.lwanga";
import "lib/memory.lwanga";
import "lib/utils.lwanga";

const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print_banner() -> u64 {
    let banner: ptr = enc "
╔═══════════════════════════════════╗
║   Lwanga Security Suite v1.0      ║
║   Professional Security Tools     ║
╚═══════════════════════════════════╝
";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, banner, 150);
    }
    
    return 0;
}

fn print_menu() -> u64 {
    let menu: ptr = "
[1] Port Scanner
[2] Banner Grabber
[3] Memory Dumper
[4] Packet Crafter
[5] Exit

Select option: ";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, menu, 100);
    }
    
    return 0;
}

fn port_scanner_mode() -> u64 {
    let msg: ptr = "\n[*] Starting port scanner...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 30);
    }
    
    // Scan common ports
    let target: u32 = 0x0100007F;  // 127.0.0.1
    scan_ports(target, 1, 1024);
    
    return 0;
}

fn banner_grabber_mode() -> u64 {
    let msg: ptr = "\n[*] Starting banner grabber...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 33);
    }
    
    // Grab banners from open ports
    grab_banners(0x0100007F, 80);
    
    return 0;
}

fn memory_dumper_mode() -> u64 {
    let msg: ptr = "\n[*] Starting memory dumper...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 32);
    }
    
    // Dump process memory
    dump_process_memory();
    
    return 0;
}

fn packet_crafter_mode() -> u64 {
    let msg: ptr = "\n[*] Starting packet crafter...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 33);
    }
    
    // Craft and send custom packets
    craft_tcp_packet(0x0100007F, 80);
    
    return 0;
}

fn main() -> u64 {
    print_banner();
    
    // Main loop
    while 1 == 1 {
        print_menu();
        
        // For this example, run all modes once
        port_scanner_mode();
        banner_grabber_mode();
        memory_dumper_mode();
        packet_crafter_mode();
        
        // Exit after demo
        break;
    }
    
    let goodbye: ptr = "\n[*] Goodbye!\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, goodbye, 14);
    }
    
    return 0;
}
```

---

## Features Implemented

### 1. Port Scanner
- TCP connect scanning
- SYN scanning
- UDP scanning
- Service detection

### 2. Banner Grabber
- HTTP banner grabbing
- FTP banner grabbing
- SSH version detection
- Custom protocol support

### 3. Memory Dumper
- Process memory reading
- Hex dump formatting
- Pattern searching
- Memory mapping

### 4. Packet Crafter
- Raw socket creation
- TCP packet crafting
- UDP packet crafting
- Custom headers

---

## Advanced Features

### Encrypted Configuration

```lwanga
const API_KEY: ptr = enc "secret_api_key_12345";
const LICENSE: ptr = enc "LWANGA-PRO-2024";
```

### Obfuscated Logic

```lwanga
// Compile with --obfuscate flag
fn check_license() -> u64 {
    // License validation logic
    return 1;
}
```

### Position-Independent Code

```bash
lwangac main.lwanga -o security-suite --pic -O2
```

---

## Building and Running

```bash
# Compile with all optimizations
lwangac main.lwanga -o security-suite -O2 --obfuscate

# Run
./security-suite

# Check binary size
ls -lh security-suite
```

---

## Next Steps

Congratulations! You've completed the Lwanga tutorial. You now know:

- ✅ Language fundamentals
- ✅ Memory management
- ✅ System programming
- ✅ Network programming
- ✅ Security techniques
- ✅ Assembly integration
- ✅ Real-world projects

### Continue Learning

- Read the [Language Reference](../language-reference.md)
- Explore [Example Programs](../../examples/)
- Join the [Community](https://github.com/cosmah/Project-Lwanga)
- Build your own tools!

---

## Resources

- [Compiler Guide](../compiler.md)
- [Syscall Reference](../syscalls.md)
- [Style Guide](../style-guide.md)
- [Contributing](../../CONTRIBUTING.md)

---

**Navigation**: [← Previous: Memory Dumper](39-memory-dumper.md) | [Tutorial Home](README.md) | [Documentation Index](../README.md)

---

<div align="center">

**🎉 Congratulations on completing the Lwanga Tutorial! 🎉**

You're now ready to build professional security tools and systems programs.

Happy coding!

</div>
