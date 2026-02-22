# Lwanga Documentation

**Complete documentation for the Lwanga programming language**

<div align="center">

[Getting Started](#getting-started) • [Language](#language) • [Tools](#tools) • [IDE Support](#ide-support) • [Examples](#examples)

</div>

---

## Getting Started

### New to Lwanga?

1. **[Installation Guide](../INSTALL.md)** - Install Lwanga on your system
2. **[Tutorial](tutorial.md)** - Learn Lwanga step-by-step
3. **[Quick Start](../README.md#quick-start)** - Your first program in 5 minutes

### Quick Links

- 📖 [Language Reference](language-reference.md) - Complete language specification
- 🎓 [Tutorial](tutorial.md) - Interactive learning guide
- 💻 [Examples](../examples/) - Sample programs
- 🔧 [Compiler Guide](compiler.md) - Using `lwangac`

---

## Language

### Core Documentation

| Document | Description | Level |
|----------|-------------|-------|
| [**Tutorial**](tutorial.md) | Step-by-step learning guide | Beginner |
| [**Language Reference**](language-reference.md) | Complete language specification | All Levels |
| [**Syscall Reference**](syscalls.md) | System call documentation | Intermediate |
| [**Type System**](type-system.md) | Types and type checking | Intermediate |
| [**Memory Model**](memory-model.md) | Memory management | Advanced |

### Language Features

- **[Variables and Types](tutorial.md#variables-and-types)** - Basic data types
- **[Functions](tutorial.md#functions)** - Function definition and calling
- **[Control Flow](tutorial.md#control-flow)** - If, while, loops
- **[Pointers](tutorial.md#pointers-and-memory)** - Memory and pointers
- **[Structs](tutorial.md#structs)** - Composite types
- **[Unsafe Operations](tutorial.md#unsafe-operations)** - Low-level access
- **[Inline Assembly](tutorial.md#inline-assembly)** - Assembly integration

---

## Tools

### Compiler

| Document | Description |
|----------|-------------|
| [**Compiler Guide**](compiler.md) | Complete `lwangac` reference |
| [**Optimization**](optimization.md) | Performance tuning |
| [**Cross-Compilation**](compiler.md#cross-compilation) | Building for other platforms |
| [**Debugging**](debugging.md) | Debugging Lwanga programs |

### Code Formatting

| Document | Description |
|----------|-------------|
| [**Formatter Guide**](formatter.md) | Using `lwangafmt` |
| [**Style Guide**](style-guide.md) | Coding conventions |
| [**Configuration**](formatter.md#configuration) | `.lwangafmt.toml` reference |

### Syntax Highlighting

| Document | Description |
|----------|-------------|
| [**Highlighter Guide**](highlighter.md) | Using `lwangacat` |
| [**Color Themes**](highlighter.md#color-themes) | Available themes |
| [**Integration**](highlighter.md#integration) | Editor and tool integration |

---

## IDE Support

### Editors

| Editor | Status | Documentation |
|--------|--------|---------------|
| **VS Code** | ✅ Full Support | [VS Code Extension](../extensions/vscode/README.md) |
| **Vim/Neovim** | ✅ Full Support | [Vim Plugin](../extensions/vim/README.md) |
| **Emacs** | 🔄 Planned | - |
| **Sublime Text** | 🔄 Planned | - |

### Features

- Syntax highlighting
- Code snippets
- Auto-formatting
- Compilation integration
- Error highlighting

---

## Examples

### Basic Programs

- [Hello World](../examples/hello_world.lwanga) - Simple output
- [Hello World with Logo](../examples/hello_world_with_logo.lwanga) - With branding
- [Variables](../examples/variables.lwanga) - Variable usage
- [Functions](../examples/functions.lwanga) - Function examples

### System Programming

- [TCP Server](../examples/tcp_server.lwanga) - Network programming
- [File Operations](../examples/file_ops.lwanga) - File I/O
- [Process Management](../examples/process.lwanga) - Fork and exec

### Security Tools

- [Shellcode](../examples/shellcode.lwanga) - Position-independent code
- [Encrypted Strings](../examples/encrypted_strings.lwanga) - String encryption
- [Memory Scanner](../examples/memory_scan.lwanga) - Memory analysis

**Browse all examples**: [examples/](../examples/)

---

## API Reference

### Standard Library

| Module | Description | Documentation |
|--------|-------------|---------------|
| `std/io` | Input/Output operations | [IO Module](api/io.md) |
| `std/mem` | Memory operations | [Memory Module](api/mem.md) |
| `std/str` | String operations | [String Module](api/str.md) |
| `std/sys` | System calls | [System Module](api/sys.md) |

### Built-in Functions

- `syscall()` - Make system calls
- `asm {}` - Inline assembly
- `enc ""` - Encrypted strings

---

## Advanced Topics

### Security

- [**Security Patterns**](security-patterns.md) - Best practices
- [**Shellcode Development**](shellcode.md) - Creating shellcode
- [**Obfuscation**](obfuscation.md) - Code obfuscation techniques
- [**Exploit Development**](exploits.md) - Writing exploits

### Performance

- [**Optimization Guide**](optimization.md) - Performance tuning
- [**Profiling**](profiling.md) - Performance analysis
- [**Benchmarking**](benchmarking.md) - Measuring performance

### Internals

- [**Compiler Architecture**](compiler-internals.md) - How it works
- [**LLVM Integration**](llvm-integration.md) - LLVM backend
- [**Type System**](type-system.md) - Type checking
- [**IR Generation**](ir-generation.md) - Code generation

---

## Packaging and Distribution

### Building Packages

| Platform | Documentation |
|----------|---------------|
| **Debian/Ubuntu** | [Debian Package](../packaging/debian/) |
| **Arch Linux** | [Arch Package](../packaging/arch/) |
| **Windows** | [Windows Installer](../packaging/windows/README.md) |
| **Docker** | [Docker Image](../packaging/docker/README.md) |

### Release Process

- [**Release Guide**](../packaging/RELEASE.md) - Publishing releases
- [**Changelog**](../CHANGELOG.md) - Version history
- [**Contributing**](../CONTRIBUTING.md) - How to contribute

---

## Reference

### Quick Reference

- [**Syntax Cheat Sheet**](cheat-sheet.md) - Quick syntax reference
- [**Syscall Table**](syscalls.md) - System call numbers
- [**Style Guide**](style-guide.md) - Coding conventions

### Specifications

- [**Language Specification**](language-reference.md) - Formal specification
- [**ABI Specification**](abi.md) - Application Binary Interface
- [**Calling Convention**](calling-convention.md) - Function calls

---

## Community

### Getting Help

- **GitHub Issues**: [Report bugs](https://github.com/cosmah/Project-Lwanga/issues)
- **Discussions**: [Ask questions](https://github.com/cosmah/Project-Lwanga/discussions)
- **Email**: cosmahke4@gmail.com

### Contributing

- [**Contributing Guide**](../CONTRIBUTING.md) - How to contribute
- [**Code of Conduct**](../CODE_OF_CONDUCT.md) - Community guidelines
- [**Style Guide**](style-guide.md) - Coding conventions

---

## Additional Resources

### External Links

- [LLVM Documentation](https://llvm.org/docs/) - LLVM reference
- [Linux Syscalls](https://man7.org/linux/man-pages/man2/syscalls.2.html) - Syscall reference
- [x86_64 Assembly](https://www.felixcloutier.com/x86/) - x86 instruction reference

### Related Projects

- [Rust](https://www.rust-lang.org/) - Systems programming language
- [Zig](https://ziglang.org/) - General-purpose programming language
- [C](https://en.cppreference.com/) - Systems programming language

---

## Documentation Status

| Document | Status | Last Updated |
|----------|--------|--------------|
| Tutorial | ✅ Complete | 2026-02-22 |
| Language Reference | ✅ Complete | 2026-02-22 |
| Compiler Guide | ✅ Complete | 2026-02-22 |
| Formatter Guide | ✅ Complete | 2026-02-22 |
| Highlighter Guide | ✅ Complete | 2026-02-22 |
| Syscall Reference | ✅ Complete | 2026-02-22 |
| Style Guide | ✅ Complete | 2026-02-22 |
| Cheat Sheet | ✅ Complete | 2026-02-22 |
| Contributing Guide | ✅ Complete | 2026-02-22 |
| Code of Conduct | ✅ Complete | 2026-02-22 |

---

## License

All documentation is released under the [MIT License](../LICENSE).

---

<div align="center">

**[⬆ Back to Top](#lwanga-documentation)**

Made with ❤️ by [Cosmc Technologies](https://github.com/cosmah)

</div>
