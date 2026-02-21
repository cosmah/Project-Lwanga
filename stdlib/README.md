# Lwanga Standard Library

This directory contains optional standard library modules for Lwanga.

Note: Lwanga is designed to work without a standard library for minimal binaries,
but these modules provide common functionality for convenience.

## Modules

- `io.lwanga` - File and console I/O helpers
- `net.lwanga` - Network socket helpers
- `mem.lwanga` - Memory management utilities
- `string.lwanga` - String manipulation functions
- `crypto.lwanga` - Basic cryptographic primitives

## Usage

```lwanga
import "stdlib/io.lwanga";

fn main() -> u64 {
    io::println("Hello from stdlib!");
    return 0;
}
```
