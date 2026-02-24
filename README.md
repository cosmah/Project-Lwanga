<div align="center">

<img src="lwanga.png" alt="Lwanga Logo" width="200"/>

# Lwanga Programming Language

**Security-Focused Systems Programming for the Modern Era**

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Version](https://img.shields.io/badge/version-1.0.2-blue.svg)](CHANGELOG.md)
[![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20macOS%20%7C%20Windows-lightgrey.svg)](#installation)

[Features](#features) • [Installation](#installation) • [Quick Start](#quick-start) • [Documentation](#documentation) • [Examples](#examples) • [Contributing](#contributing)

</div>

---

## Overview

Lwanga is a low-level, compiled systems programming language designed for **security research**, **penetration testing**, and **shellcode development**. Built on LLVM, it provides direct system access, minimal runtime overhead, and powerful features for creating security tools and exploits.

### Why Lwanga?

- 🔒 **Security-First**: Encrypted strings, position-independent code, direct syscalls
- ⚡ **Minimal Footprint**: Hello World in under 10KB, no libc dependencies
- 🎯 **Low-Level Control**: Inline assembly, naked functions, register access
- 🚀 **Modern Tooling**: LLVM backend, code formatter, syntax highlighter
- 🌐 **Cross-Platform**: x86_64 and ARM64 support on Linux, macOS, and Windows

---

## Features

### Core Language Features

| Feature | Description |
|---------|-------------|
| **Direct Syscalls** | Make system calls without libc overhead |
| **Inline Assembly** | Intel and AT&T syntax support |
| **Naked Functions** | Functions without prologue/epilogue for shellcode |
| **Encrypted Strings** | Compile-time string encryption with `enc` keyword |
| **Position-Independent Code** | Built-in PIC generation for shellcode |
| **Unsafe Blocks** | Explicit low-level operations |
| **Zero Runtime** | No standard library, minimal dependencies |

### Type System

```lwanga
u8, u16, u32, u64    // Unsigned integers
ptr                   // Raw pointers
register              // Register type for inline assembly
struct                // Composite types
packed struct         // Memory-layout controlled structs
```

### Tooling

- **lwangac**: LLVM-based compiler with optimization levels
- **lwangafmt**: Code formatter with logo headers and TOML configuration
- **lwangacat**: Syntax highlighter with 4 color themes
- **VS Code Extension**: Full IDE support with snippets and formatting
- **Vim Plugin**: Syntax highlighting and commands

---

## Installation

### Quick Install

<details>
<summary><b>Linux (Ubuntu/Debian)</b></summary>

```bash
# Download .deb package
wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.2/lwanga_1.0.2-1_amd64.deb

# Install
sudo dpkg -i lwanga_1.0.2-1_amd64.deb
sudo apt-get install -f  # Fix dependencies
```
</details>

<details>
<summary><b>Linux (Arch)</b></summary>

```bash
# From AUR
yay -S lwanga
# or
paru -S lwanga
```
</details>

<details>
<summary><b>macOS</b></summary>

```bash
# Homebrew (when available)
brew install lwanga

# Or build from source
git clone https://github.com/cosmah/Project-Lwanga.git
cd Project-Lwanga/compiler
mkdir build && cd build
cmake .. && make -j$(nproc)
sudo make install
```
</details>

<details>
<summary><b>Windows</b></summary>

```powershell
# Download and run installer
Invoke-WebRequest -Uri "https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.2/lwanga-1.0.2-windows-x64-installer.exe" -OutFile "lwanga-installer.exe"
.\lwanga-installer.exe

# Or via Chocolatey
choco install lwanga
```
</details>

<details>
<summary><b>Docker</b></summary>

```bash
docker pull cosmctechnologies/lwanga:latest
docker run -it --rm -v $(pwd):/workspace cosmctechnologies/lwanga:latest
```
</details>

**📖 Detailed installation instructions**: [INSTALL.md](INSTALL.md)

---

## Quick Start

### Hello World

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);  // write(stdout, msg, len)
    }
    return 0;
}
```

**Compile and run:**
```bash
lwangac hello.lwanga -o hello
./hello
```

### Inline Assembly

```lwanga
naked fn shellcode() -> u64 {
    asm {
        mov rax, 60      // sys_exit
        xor rdi, rdi     // status = 0
        syscall
    }
}
```

### Encrypted Strings

```lwanga
fn main() -> u64 {
    let secret: ptr = enc "sensitive data";
    // String is encrypted at compile-time
    return 0;
}
```

**🎓 More examples**: [examples/](examples/) | [Tutorial](docs/tutorial.md)

---

## Documentation

### 📚 Core Documentation

| Document | Description |
|----------|-------------|
| [**Tutorial Series**](docs/tutorial/README.md) | 40-lesson comprehensive learning path |
| [**Language Reference**](docs/language-reference.md) | Complete language specification |
| [**Installation Guide**](INSTALL.md) | Platform-specific installation |
| [**Syscall Reference**](docs/syscalls.md) | System call documentation |
| [**Style Guide**](docs/style-guide.md) | Coding conventions |
| [**Cheat Sheet**](docs/cheat-sheet.md) | Quick syntax reference |

### 🛠️ Tools Documentation

| Tool | Description | Documentation |
|------|-------------|---------------|
| **lwangac** | Compiler | [Compiler Guide](docs/compiler.md) |
| **lwangafmt** | Formatter | [Formatter Guide](docs/formatter.md) |
| **lwangacat** | Highlighter | [Highlighter Guide](docs/highlighter.md) |

### 🎨 IDE Integration

| Editor | Status | Documentation |
|--------|--------|---------------|
| **VS Code** | ✅ Full Support | [VS Code Extension](extensions/vscode/README.md) |
| **Vim/Neovim** | ✅ Full Support | [Vim Plugin](extensions/vim/README.md) |
| **Emacs** | 🔄 Planned | - |
| **Sublime** | 🔄 Planned | - |

### 📦 Packaging

| Platform | Package Type | Documentation |
|----------|--------------|---------------|
| **Debian/Ubuntu** | .deb | [Debian Package](packaging/debian/) |
| **Arch Linux** | PKGBUILD | [Arch Package](packaging/arch/) |
| **Windows** | .exe Installer | [Windows Guide](packaging/windows/README.md) |
| **Docker** | Container | [Docker Guide](packaging/docker/README.md) |

---

## Examples

### Basic Programs

- [**Hello World**](examples/hello_world.lwanga) - Simple syscall-based output
- [**Hello World with Logo**](examples/hello_world_with_logo.lwanga) - With file header
- [**TCP Server**](examples/tcp_server.lwanga) - Network programming
- [**Encrypted Strings**](examples/encrypted_strings.lwanga) - String encryption

### Security Tools

- [**Shellcode**](examples/shellcode.lwanga) - Position-independent shellcode
- [**Shellcode with Banner**](examples/shellcode_with_banner.lwanga) - With branding

### Advanced Examples

- [**Test Errors**](examples/test_errors.lwanga) - Error handling demonstration
- [**Inline Assembly**](examples/test_asm_simple.lwanga) - Assembly integration
- [**Naked Functions**](examples/test_naked.lwanga) - Shellcode techniques

**📁 Browse all examples**: [examples/](examples/)

---

## Project Structure

```
lwanga/
├── compiler/              # Compiler implementation (C++ + LLVM)
│   ├── include/          # Header files
│   ├── src/              # Source code
│   │   ├── lexer/       # Lexical analysis
│   │   ├── parser/      # Syntax analysis
│   │   ├── types/       # Type checking
│   │   ├── codegen/     # LLVM IR generation
│   │   ├── backend/     # Code generation
│   │   └── driver/      # Compiler driver
│   ├── tests/            # Test suite (20+ tests)
│   └── CMakeLists.txt    # Build configuration
│
├── tools/                 # Additional tooling
│   ├── formatter/        # lwangafmt code formatter
│   └── highlighter/      # lwangacat syntax highlighter
│
├── extensions/            # IDE extensions
│   ├── vscode/           # VS Code extension
│   └── vim/              # Vim plugin
│
├── docs/                  # Documentation
│   ├── language-reference.md
│   ├── tutorial.md
│   ├── syscalls.md
│   └── api/              # API documentation
│
├── examples/              # Example programs
│   ├── hello_world.lwanga
│   ├── shellcode.lwanga
│   └── ...
│
├── packaging/             # Distribution packages
│   ├── debian/           # Debian/Ubuntu packages
│   ├── arch/             # Arch Linux packages
│   ├── windows/          # Windows installer
│   └── docker/           # Docker images
│
├── assets/                # Branding assets
│   └── icons/            # File type icons
│
├── README.md              # This file
├── INSTALL.md             # Installation guide
├── CHANGELOG.md           # Version history
└── LICENSE                # MIT License
```

---

## Development

### Building from Source

```bash
# Clone repository
git clone https://github.com/cosmah/Project-Lwanga.git
cd Project-Lwanga

# Build compiler
cd compiler
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)

# Run tests
ctest --output-on-failure

# Install
sudo make install
```

### Running Tests

```bash
cd compiler/build
ctest --output-on-failure -j$(nproc)
```

**Test Coverage**: 20+ comprehensive test suites covering lexer, parser, type system, IR generation, backend, and end-to-end compilation.

### Contributing

We welcome contributions! Please see:
- [Contributing Guidelines](CONTRIBUTING.md)
- [Code of Conduct](CODE_OF_CONDUCT.md)
- [Style Guide](docs/style-guide.md)

---

## Roadmap

### v1.0.2 (Current) ✅
- Full compiler with LLVM backend
- Code formatter and syntax highlighter
- VS Code and Vim extensions
- Comprehensive documentation
- Multi-platform support

### v1.1.0 (Planned)
- Language Server Protocol (LSP)
- Debugger integration (GDB/LLDB/WinDbg)
- Package manager
- More standard library functions

### v2.0.0 (Future)
- WebAssembly target
- Native Windows syscall support
- Advanced optimization passes
- Garbage collection (optional)

---

## Community

- **GitHub Issues**: [Report bugs or request features](https://github.com/cosmah/Project-Lwanga/issues)
- **Discussions**: [Ask questions and share ideas](https://github.com/cosmah/Project-Lwanga/discussions)
- **Email**: cosmahke4@gmail.com

---

## License

Lwanga is released under the [MIT License](LICENSE).

```
Copyright (c) 2024-2026 Cosmc Technologies

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.
```

---

## Acknowledgments

- **LLVM Project**: For the excellent compiler infrastructure
- **Security Community**: For inspiration and use cases
- **Contributors**: Everyone who has contributed to this project

---

## Citation

If you use Lwanga in your research or project, please cite:

```bibtex
@software{lwanga2026,
  title = {Lwanga: Security-Focused Systems Programming Language},
  author = {Cosmc Technologies},
  year = {2026},
  url = {https://github.com/cosmah/Project-Lwanga},
  version = {1.0.2}
}
```

---

<div align="center">

**Made by [Cosmc Technologies](https://github.com/cosmah)**

[⬆ Back to Top](#lwanga-programming-language)

</div>
