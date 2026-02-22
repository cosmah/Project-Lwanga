# Lwanga Language Support for VS Code

Official VS Code extension for the Lwanga programming language - a security-focused systems programming language designed for low-level development, shellcode, and penetration testing.

## Features

### Syntax Highlighting
- Full syntax highlighting for all Lwanga constructs
- Special highlighting for `unsafe` blocks and inline assembly
- Support for encrypted strings (`enc`)
- Preprocessor directive highlighting

### Code Snippets
- Quick snippets for common patterns:
  - `fn` - Function definition
  - `main` - Main function
  - `unsafe` - Unsafe block
  - `syscall` - System call
  - `asm` - Inline assembly
  - `naked` - Naked function
  - `struct` - Struct definition
  - `header` - File header with logo
  - `shellcode` - Shellcode template
  - And many more...

### File Icons
- Custom file icons for `.lwanga` files
- Theme-aware icons (light/dark mode)

### Code Formatting
- Format on save (configurable)
- Format command (`Shift+Alt+F`)
- Add logo headers to files
- Integrates with `lwangafmt` formatter

### Compilation
- Compile current file with `Lwanga: Compile Current File` command
- View compilation output in dedicated output channel
- Configurable compiler arguments

## Requirements

- Lwanga compiler (`lwangac`) installed and in PATH
- Lwanga formatter (`lwangafmt`) installed and in PATH (optional, for formatting)

## Installation

### From VSIX
```bash
code --install-extension lwanga-1.0.0.vsix
```

### From Source
```bash
cd extensions/vscode
npm install
npm run compile
npm run package
code --install-extension lwanga-1.0.0.vsix
```

## Configuration

Configure the extension in VS Code settings:

```json
{
  "lwanga.compilerPath": "lwangac",
  "lwanga.formatterPath": "lwangafmt",
  "lwanga.formatOnSave": false,
  "lwanga.compilerArgs": ["-O2"],
  "lwanga.logoStyle": "minimal"
}
```

### Settings

- `lwanga.compilerPath` - Path to the Lwanga compiler (default: `lwangac`)
- `lwanga.formatterPath` - Path to the Lwanga formatter (default: `lwangafmt`)
- `lwanga.formatOnSave` - Automatically format files on save (default: `false`)
- `lwanga.compilerArgs` - Additional compiler arguments (default: `["-O2"]`)
- `lwanga.logoStyle` - Logo style for headers: `minimal`, `block`, `banner`, or `none` (default: `minimal`)

## Commands

- `Lwanga: Format Document` - Format the current file
- `Lwanga: Add Logo Header` - Add a logo header to the current file
- `Lwanga: Compile Current File` - Compile the current file

## Keybindings

- `Shift+Alt+F` - Format document (when editing `.lwanga` files)

## Language Features

### Supported Constructs
- Functions (`fn`)
- Variables (`let`, `mut`)
- Constants (`const`)
- Structs (`struct`, `packed struct`)
- Control flow (`if`, `else`, `while`)
- Unsafe blocks (`unsafe`)
- Inline assembly (`asm`, `naked`)
- System calls (`syscall`)
- Encrypted strings (`enc`)
- Imports (`import`)
- Preprocessor directives (`#if`, `#else`, `#endif`, `#define`)

### Types
- `u8`, `u16`, `u32`, `u64` - Unsigned integers
- `ptr` - Raw pointer
- `register` - Register type for inline assembly

## Examples

### Hello World
```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);  // write
    }
    return 0;
}
```

### Inline Assembly
```lwanga
naked fn shellcode() -> u64 {
    asm {
        mov rax, 60
        xor rdi, rdi
        syscall
    }
}
```

### Encrypted Strings
```lwanga
fn main() -> u64 {
    let secret: ptr = enc "sensitive data";
    return 0;
}
```

## Development

### Building
```bash
npm install
npm run compile
```

### Packaging
```bash
npm run package
```

### Publishing
```bash
vsce publish
```

## Contributing

Contributions are welcome! Please submit issues and pull requests to the main Lwanga repository.

## License

MIT License - See LICENSE file for details

## Links

- [Lwanga Repository](https://github.com/lwanga-lang/lwanga)
- [Documentation](https://lwanga-lang.org/docs)
- [Language Reference](https://lwanga-lang.org/docs/language-reference)

## Support

For issues and questions:
- GitHub Issues: https://github.com/lwanga-lang/lwanga/issues
- Documentation: https://lwanga-lang.org/docs
