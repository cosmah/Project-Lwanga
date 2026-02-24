# Changelog

All notable changes to the Lwanga programming language will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.2/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.0.2] - 2026-02-24

### Added
- Support for `*Type` pointer syntax (e.g., `let p: *u64`)
- Enhanced preprocessor with macro substitution support (`#define VAL 100`)
- Improved parser robustness with statement-level error recovery
- More descriptive error messages for expression and type parsing

### Fixed
- **Array Indexing**: Resolved critical LLVM verification error when indexing stack-allocated arrays
- **Opaque Pointers**: Fixed element size calculation for pointer indexing in LLVM 18+
- **Compiler Hangs**: Fixed infinite loops in Preprocessor and Parser during error states
- **Control Flow**: Fixed "Terminator in middle of block" error by skipping dead code after returns
- **Debian Package**: Added missing `binutils` and `build-essential` dependencies

### Changed
- Replaced removed LLVM pass `createDemoteRegisterToMemoryPass` with manual PHI node demotion
- Updated all internal versioning and documentation to 1.0.2

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

[1.0.2]: https://github.com/cosmah/Project-Lwanga/releases/tag/v1.0.2
[1.0.0]: https://github.com/cosmah/Project-Lwanga/releases/tag/v1.0.0
[Unreleased]: https://github.com/cosmah/Project-Lwanga/compare/v1.0.2...HEAD
