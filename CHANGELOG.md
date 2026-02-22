# Changelog

All notable changes to the Lwanga programming language will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.0] - 2026-02-22

### Added
- Initial public release of Lwanga programming language
- Full compiler with LLVM 18 backend
- Support for x86_64 and ARM64 architectures
- Direct syscall interface for system programming
- Inline assembly support (Intel and AT&T syntax)
- Naked functions for shellcode development
- Position-independent code (PIC) generation
- Encrypted string literals (`enc` keyword)
- Unsafe blocks for low-level operations
- Module system with import statements
- Preprocessor directives (#if, #else, #endif, #define)
- Packed structs for memory layout control
- Register type for inline assembly
- Array literals support

#### Compiler Features
- LLVM-based code generation
- Optimization levels (-O0 to -O3)
- Cross-compilation support
- Debug information generation (-g)
- Code obfuscation (--obfuscate)
- JIT execution mode (--jit)
- Verbose output mode (-v)

#### Tools
- **lwangafmt**: Code formatter with logo headers
  - Multiple logo styles (minimal, block, banner)
  - TOML configuration support
  - Format on save capability
  - Recursive directory formatting
  - Check mode for CI/CD
- **lwangacat**: Syntax highlighter for terminal
  - 4 color themes (default, dark, light, monokai)
  - Line numbers with highlighting
  - Context display for errors
  - Pipe support

#### IDE Extensions
- **VS Code Extension**:
  - Full syntax highlighting with TextMate grammar
  - 18 code snippets
  - Document formatting integration
  - Compilation integration with output channel
  - Format on save
  - Logo header insertion
  - Theme-aware file icons
- **Vim Plugin**:
  - Complete syntax highlighting
  - Filetype detection
  - Smart indentation
  - Custom commands (format, compile, add header)
  - Keybindings

#### Type System
- Unsigned integers: u8, u16, u32, u64
- Raw pointers: ptr
- Register type for inline assembly
- Struct types with packed option
- Array types
- Function types

#### Standard Library
- Syscall interface
- Basic I/O operations
- String manipulation
- Memory operations

#### Testing
- 20+ comprehensive test suites:
  - Lexer tests
  - Parser tests
  - Type system tests
  - IR generation tests
  - Backend tests
  - Integration tests
  - End-to-end tests
  - Cross-compilation tests
  - Determinism tests
  - Obfuscation tests

#### Documentation
- Complete language reference
- Installation guide
- API documentation
- Syscall reference
- 10+ example programs
- Formatter and branding guide
- IDE extension documentation

#### Packaging
- Debian/Ubuntu .deb package
- Arch Linux PKGBUILD
- Docker image
- Windows installer (.exe with NSIS)
- Chocolatey package for Windows
- Source tarballs
- MIME type registration
- File type icons (16px to 256px)

### Performance
- Hello World binary: 8.9KB (under 10KB target)
- Compilation speed: ~75ms for simple programs
- Zero runtime overhead for syscalls
- Minimal standard library footprint

### Security Features
- Encrypted string literals
- Position-independent code generation
- Naked functions for shellcode
- Direct syscall interface (no libc)
- Unsafe blocks for explicit low-level operations

### Compatibility
- LLVM 18
- C++17 compiler required
- CMake 3.15+
- Linux (Ubuntu 20.04+, Arch, Debian 11+)
- macOS 11+
- Windows 10/11 (with Visual Studio 2019+)
- Docker support

### Known Limitations
- No garbage collection (manual memory management)
- Limited standard library (by design)
- No dynamic linking support yet
- Single-threaded compilation

## [Unreleased]

### Planned Features
- Language Server Protocol (LSP) implementation
- Debugger integration (GDB/LLDB/WinDbg)
- Package manager
- More standard library functions
- Native Windows syscall support
- WebAssembly target
- Emacs mode
- Sublime Text syntax
- Scoop package for Windows
- WinGet package for Windows

---

## Release Notes

### v1.0.0 - Initial Release

This is the first stable release of Lwanga, suitable for:
- Security research and penetration testing
- Shellcode development
- Low-level systems programming
- Educational purposes
- Embedded systems development

**Not recommended for**:
- Production web services
- Applications requiring garbage collection
- Projects needing extensive standard library

### Breaking Changes
None (initial release)

### Migration Guide
None (initial release)

### Deprecations
None (initial release)

---

**Cosmc Technologies** | https://github.com/cosmah/Project-Lwanga

[1.0.0]: https://github.com/cosmah/Project-Lwanga/releases/tag/v1.0.0
[Unreleased]: https://github.com/cosmah/Project-Lwanga/compare/v1.0.0...HEAD
