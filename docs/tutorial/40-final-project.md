# Lesson 40: Final Project

**Comprehensive security tool combining everything you've learned.**

---

## Project: Multi-Tool Security Suite

Build a comprehensive tool that includes:
- Port scanning
- Banner grabbing
- Service detection
- Memory analysis
- Network packet crafting

---

## Project Structure

```
security-suite/
├── main.lwanga
├── lib/
│   ├── network.lwanga
│   ├── scanner.lwanga
│   ├── memory.lwanga
│   └── utils.lwanga
└── README.md
```

---

## Main Program

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
