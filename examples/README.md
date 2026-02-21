# Lwanga Examples

This directory contains example programs written in Lwanga to demonstrate language features.

## Examples

- `hello_world.lwanga` - Basic syscall-based output
- `tcp_server.lwanga` - Network socket programming
- `shellcode.lwanga` - Position-independent code example
- `encrypted_strings.lwanga` - Encrypted string literals
- `memory_ops.lwanga` - Pointer arithmetic and memory manipulation
- `inline_asm.lwanga` - Inline assembly examples
- `structs_arrays.lwanga` - Data structures

## Running Examples

```bash
# Compile and run
lwangac hello_world.lwanga -o hello
./hello

# JIT execution
lwangac --jit hello_world.lwanga
```
