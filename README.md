# Lwanga Programming Language

Lwanga is a low-level, compiled programming language specifically designed for security researchers and hacking tool builders. The language prioritizes minimal binary size, direct system access, and features that enable the creation of security tools, exploits, and position-independent code.

## Features

- **Low-level control**: Direct memory access, pointer arithmetic, inline assembly
- **Minimal binaries**: No standard library dependencies, tiny executables (<10KB)
- **Direct syscalls**: Make system calls without libc
- **Security-focused**: Encrypted strings, naked functions, obfuscation support
- **Cross-platform**: LLVM backend supports x86_64 and ARM64
- **Position-independent code**: Built-in support for shellcode and PIC

## Project Structure

```
lwanga/
├── compiler/          # Compiler implementation (C++ + LLVM)
├── examples/          # Example programs
├── docs/              # Language documentation
├── stdlib/            # Optional standard library modules
├── tools/             # Additional tooling
├── .kiro/specs/       # Design specifications
└── README.md
```

## Quick Start

### Building the Compiler

```bash
cd compiler
mkdir build && cd build
cmake ..
make
sudo make install
```

### Hello World

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, Lwanga!\n" as ptr;
    let len: u64 = 15;
    
    // syscall(SYS_WRITE, fd, buf, count)
    syscall(1, 1, msg, len);
    
    return 0;
}
```

Compile and run:
```bash
lwangac hello.lwanga -o hello
./hello
```

## Documentation

- [Language Reference](docs/language-reference.md)
- [Tutorial](docs/tutorial.md)
- [Syscall Reference](docs/syscalls.md)
- [Examples](examples/)

## Development Status

This project is under active development. See `.kiro/specs/lwanga-programming-language/` for detailed requirements, design, and implementation tasks.

## License

[To be determined]

## Contributing

Contributions are welcome! Please see the implementation tasks in `.kiro/specs/lwanga-programming-language/tasks.md`.
