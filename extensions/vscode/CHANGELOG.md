# Change Log

All notable changes to the Lwanga VS Code extension will be documented in this file.

## [1.0.2] - 2024-02-22

### Added
- Initial release
- Syntax highlighting for all Lwanga constructs
- Code snippets for common patterns
- File icons for `.lwanga` files
- Document formatting integration with `lwangafmt`
- Compile command integration with `lwangac`
- Add logo header command
- Format on save option
- Configurable compiler and formatter paths
- Output channel for compilation results
- Keybinding for format (Shift+Alt+F)

### Features
- Full TextMate grammar for accurate syntax highlighting
- 15+ code snippets including:
  - Function definitions
  - Control flow structures
  - Unsafe blocks
  - Inline assembly
  - Shellcode templates
  - File headers with logo
- Theme-aware file icons (light/dark mode)
- Automatic bracket matching and auto-closing
- Comment toggling support
- Code folding support
- Smart indentation

### Language Support
- Keywords: `fn`, `let`, `mut`, `const`, `struct`, `import`, `as`, `packed`
- Control flow: `if`, `else`, `while`, `return`
- Types: `u8`, `u16`, `u32`, `u64`, `ptr`, `register`
- Special: `unsafe`, `asm`, `naked`, `syscall`, `enc`
- Preprocessor: `#if`, `#else`, `#endif`, `#define`

### Commands
- `Lwanga: Format Document` - Format current file
- `Lwanga: Add Logo Header` - Add logo header to file
- `Lwanga: Compile Current File` - Compile with `lwangac`

### Configuration
- `lwanga.compilerPath` - Compiler path
- `lwanga.formatterPath` - Formatter path
- `lwanga.formatOnSave` - Auto-format on save
- `lwanga.compilerArgs` - Compiler arguments
- `lwanga.logoStyle` - Logo header style
