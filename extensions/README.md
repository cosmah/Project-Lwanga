# Lwanga IDE Extensions

Official IDE and editor extensions for the Lwanga programming language.

## Available Extensions

### VS Code Extension
**Location**: `extensions/vscode/`

Full-featured VS Code extension with:
- Syntax highlighting with TextMate grammar
- 15+ code snippets
- File icons (theme-aware)
- Document formatting
- Compilation integration
- Format on save
- Logo header insertion

**Status**: Production-ready, ready to publish to VS Code Marketplace

### Vim Plugin
**Location**: `extensions/vim/`

Complete Vim plugin with:
- Syntax highlighting
- Filetype detection
- Smart indentation
- Comment handling
- Code folding
- Compiler integration
- Formatter integration
- Custom commands and keybindings

**Status**: Production-ready, ready to publish

## Quick Start

### VS Code
```bash
cd extensions/vscode
npm install
npm run compile
npm run package
code --install-extension lwanga-1.0.0.vsix
```

### Vim
```bash
# Manual installation
mkdir -p ~/.vim/{syntax,ftdetect,ftplugin}
cp extensions/vim/syntax/lwanga.vim ~/.vim/syntax/
cp extensions/vim/ftdetect/lwanga.vim ~/.vim/ftdetect/
cp extensions/vim/ftplugin/lwanga.vim ~/.vim/ftplugin/
```

See [INSTALL.md](INSTALL.md) for detailed installation instructions.

## Features Comparison

| Feature | VS Code | Vim |
|---------|---------|-----|
| Syntax Highlighting | ✅ | ✅ |
| Code Snippets | ✅ | ❌ |
| File Icons | ✅ | ❌ |
| Auto-formatting | ✅ | ✅ |
| Compilation | ✅ | ✅ |
| Format on Save | ✅ | ❌ |
| Logo Headers | ✅ | ✅ |
| Code Folding | ✅ | ✅ |
| Smart Indentation | ✅ | ✅ |

## Language Features

All extensions support:
- Keywords: `fn`, `let`, `mut`, `const`, `struct`, `import`, `as`, `packed`
- Control flow: `if`, `else`, `while`, `return`
- Types: `u8`, `u16`, `u32`, `u64`, `ptr`, `register`
- Special constructs: `unsafe`, `asm`, `naked`, `syscall`, `enc`
- Preprocessor: `#if`, `#else`, `#endif`, `#define`
- Comments: `//` and `/* */`
- Strings with escape sequences
- Numbers: decimal, hex (0x), binary (0b)

## Development

### VS Code Extension

**Prerequisites**:
- Node.js 18+
- npm or yarn
- TypeScript 5+

**Build**:
```bash
cd extensions/vscode
npm install
npm run compile
```

**Package**:
```bash
npm run package
```

**Publish** (requires publisher account):
```bash
vsce publish
```

### Vim Plugin

No build process required. Files are ready to use.

**Testing**:
```bash
vim -u NONE -c "set runtimepath+=extensions/vim" test.lwanga
```

## Publishing

### VS Code Marketplace

1. Create publisher account at https://marketplace.visualstudio.com/
2. Get Personal Access Token from Azure DevOps
3. Login: `vsce login <publisher>`
4. Publish: `vsce publish`

### Vim.org

1. Create account at https://www.vim.org/
2. Upload plugin as zip file
3. Or publish to GitHub and add to vim-plug/Vundle

## System Integration

### File Icons (Linux)

Install system-wide file icons:
```bash
# Copy icons
for size in 16 32 64 128 256; do
    sudo cp assets/icons/lwanga-file-${size}.png \
        /usr/share/icons/hicolor/${size}x${size}/mimetypes/text-x-lwanga.png
done

# Update cache
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

### MIME Type (Linux)

Register `.lwanga` file type:
```bash
sudo tee /usr/share/mime/packages/lwanga.xml > /dev/null << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<mime-info xmlns="http://www.freedesktop.org/standards/shared-mime-info">
  <mime-type type="text/x-lwanga">
    <comment>Lwanga source code</comment>
    <glob pattern="*.lwanga"/>
    <icon name="text-x-lwanga"/>
  </mime-type>
</mime-info>
EOF

sudo update-mime-database /usr/share/mime
```

## Screenshots

### VS Code
- Syntax highlighting with theme support
- File icons in explorer
- Code snippets with IntelliSense
- Compilation output

### Vim
- Syntax highlighting
- Command integration
- Smart indentation

## Future Plans

- Emacs mode
- Sublime Text syntax
- Language Server Protocol (LSP) implementation
- IntelliSense/autocomplete
- Go to definition
- Find references
- Hover documentation
- Signature help

## Contributing

Contributions welcome! Areas for improvement:
- Additional code snippets
- Better syntax highlighting patterns
- LSP server implementation
- More editor support (Emacs, Sublime, etc.)
- Improved error detection
- Linting integration

## License

MIT License - See LICENSE file for details

## Support

- GitHub Issues: https://github.com/lwanga-lang/lwanga/issues
- Documentation: https://lwanga-lang.org/docs
- Discord: https://discord.gg/lwanga (coming soon)
