# Lwanga Syntax Highlighter Guide

**Complete reference for `lwangacat` - the Lwanga syntax highlighter**

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Command Line Options](#command-line-options)
5. [Color Themes](#color-themes)
6. [Integration](#integration)
7. [Examples](#examples)

---

## Overview

`lwangacat` is the official syntax highlighter for Lwanga. It provides beautiful, colorized output for Lwanga source code in the terminal, making code review and debugging easier.

### Features

- ✅ 4 built-in color themes
- ✅ Line numbers with highlighting
- ✅ Context display for errors
- ✅ Pipe support for integration
- ✅ Compiler integration
- ✅ Customizable colors

---

## Installation

`lwangacat` is included with the Lwanga compiler installation.

### Verify Installation

```bash
lwangacat --help
```

### Standalone Installation

```bash
# From source
cd tools/highlighter
mkdir build && cd build
cmake ..
make
sudo make install
```

---

## Basic Usage

### Display a File

```bash
lwangacat myfile.lwanga
```

### Display with Line Numbers

```bash
lwangacat --line-numbers myfile.lwanga
```

### Use Specific Theme

```bash
lwangacat --theme monokai myfile.lwanga
```

### Display Specific Lines

```bash
lwangacat --lines 10-20 myfile.lwanga
```

---

## Command Line Options

### Input

| Option | Short | Description |
|--------|-------|-------------|
| `<file>` | | Input file to highlight |
| `-` | | Read from stdin |

### Display Options

| Option | Short | Description |
|--------|-------|-------------|
| `--line-numbers` | `-n` | Show line numbers |
| `--lines <range>` | `-l` | Display specific lines (e.g., 10-20) |
| `--context <n>` | `-c` | Show n lines of context |

### Themes

| Option | Short | Description |
|--------|-------|-------------|
| `--theme <name>` | `-t` | Color theme (default, dark, light, monokai) |
| `--list-themes` | | List available themes |

### Output Control

| Option | Short | Description |
|--------|-------|-------------|
| `--no-color` | | Disable colors |
| `--plain` | `-p` | Plain output (no formatting) |

### Help

| Option | Short | Description |
|--------|-------|-------------|
| `--help` | `-h` | Show help message |
| `--version` | | Show version |

---

## Color Themes

### Default Theme

Balanced colors for most terminals:

```bash
lwangacat --theme default myfile.lwanga
```

**Color Scheme**:
- Keywords: Blue
- Types: Cyan
- Strings: Green
- Numbers: Magenta
- Comments: Gray
- Functions: Yellow

### Dark Theme

Optimized for dark terminal backgrounds:

```bash
lwangacat --theme dark myfile.lwanga
```

**Color Scheme**:
- Keywords: Bright Blue
- Types: Bright Cyan
- Strings: Bright Green
- Numbers: Bright Magenta
- Comments: Dark Gray
- Functions: Bright Yellow

### Light Theme

Optimized for light terminal backgrounds:

```bash
lwangacat --theme light myfile.lwanga
```

**Color Scheme**:
- Keywords: Dark Blue
- Types: Dark Cyan
- Strings: Dark Green
- Numbers: Dark Magenta
- Comments: Light Gray
- Functions: Dark Yellow

### Monokai Theme

Popular color scheme inspired by Sublime Text:

```bash
lwangacat --theme monokai myfile.lwanga
```

**Color Scheme**:
- Keywords: Pink
- Types: Blue
- Strings: Yellow
- Numbers: Purple
- Comments: Gray
- Functions: Green

---

## Integration

### With Compiler

The compiler automatically uses `lwangacat` for colorized output:

```bash
lwangac myfile.lwanga -o myfile
```

Output includes:
- Colorized error messages
- Syntax-highlighted code snippets
- Context lines with highlighting

### With Less/More

```bash
lwangacat myfile.lwanga | less -R
```

The `-R` flag preserves ANSI color codes.

### With Grep

```bash
lwangacat myfile.lwanga | grep "function"
```

### In Scripts

```bash
#!/bin/bash
# Display all .lwanga files with syntax highlighting

for file in *.lwanga; do
    echo "=== $file ==="
    lwangacat "$file"
    echo
done
```

### Git Diff Integration

Add to `.gitconfig`:

```ini
[diff "lwanga"]
    textconv = lwangacat --plain
```

Add to `.gitattributes`:

```
*.lwanga diff=lwanga
```

---

## Examples

### Example 1: Basic Highlighting

**Command**:
```bash
lwangacat hello.lwanga
```

**Output** (with colors):
```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 0;
}
```

### Example 2: With Line Numbers

**Command**:
```bash
lwangacat --line-numbers hello.lwanga
```

**Output**:
```
  1 │ fn main() -> u64 {
  2 │     let msg: ptr = "Hello, World!\n";
  3 │     unsafe {
  4 │         syscall(1, 1, msg, 14);
  5 │     }
  6 │     return 0;
  7 │ }
```

### Example 3: Specific Lines

**Command**:
```bash
lwangacat --lines 3-5 hello.lwanga
```

**Output**:
```
  3 │     unsafe {
  4 │         syscall(1, 1, msg, 14);
  5 │     }
```

### Example 4: Error Context

**Command**:
```bash
lwangacat --context 2 --lines 4 hello.lwanga
```

**Output** (shows line 4 with 2 lines of context):
```
  2 │     let msg: ptr = "Hello, World!\n";
  3 │     unsafe {
  4 │         syscall(1, 1, msg, 14);
  5 │     }
  6 │     return 0;
```

### Example 5: From Stdin

**Command**:
```bash
echo 'fn main() -> u64 { return 0; }' | lwangacat -
```

### Example 6: Compare Themes

```bash
# Default theme
lwangacat --theme default myfile.lwanga

# Dark theme
lwangacat --theme dark myfile.lwanga

# Light theme
lwangacat --theme light myfile.lwanga

# Monokai theme
lwangacat --theme monokai myfile.lwanga
```

---

## Syntax Elements

### Keywords

Highlighted in blue (default theme):

```lwanga
fn if else while return let mut const
unsafe asm naked import struct packed enc
```

### Types

Highlighted in cyan (default theme):

```lwanga
u8 u16 u32 u64 ptr register
```

### Strings

Highlighted in green (default theme):

```lwanga
"Hello, World!"
enc "encrypted string"
```

### Numbers

Highlighted in magenta (default theme):

```lwanga
42
0x1234
0b1010
```

### Comments

Highlighted in gray (default theme):

```lwanga
// Single-line comment
/* Multi-line comment */
```

### Functions

Highlighted in yellow (default theme):

```lwanga
fn main() -> u64 {
    syscall(1, 1, msg, 14);
}
```

---

## Advanced Usage

### Custom Context Display

Show errors with context:

```bash
# Show error at line 10 with 3 lines of context
lwangacat --lines 10 --context 3 myfile.lwanga
```

### Batch Processing

```bash
# Highlight all files in directory
for file in src/*.lwanga; do
    echo "=== $file ==="
    lwangacat --line-numbers "$file"
done
```

### Integration with Find

```bash
# Find and highlight all .lwanga files
find . -name "*.lwanga" -exec lwangacat --line-numbers {} \;
```

### Pipe to Editor

```bash
# View highlighted code in vim
lwangacat myfile.lwanga | vim -R -
```

---

## Configuration

### Environment Variables

| Variable | Description | Default |
|----------|-------------|---------|
| `LWANGACAT_THEME` | Default theme | default |
| `LWANGACAT_LINE_NUMBERS` | Show line numbers | false |
| `NO_COLOR` | Disable colors | false |

### Example Configuration

Add to `.bashrc` or `.zshrc`:

```bash
# Set default theme
export LWANGACAT_THEME=monokai

# Always show line numbers
export LWANGACAT_LINE_NUMBERS=true

# Alias for quick access
alias lcat='lwangacat'
```

---

## Troubleshooting

### Colors Not Showing

**Problem**: No colors in output

**Solutions**:
```bash
# Check if terminal supports colors
echo $TERM

# Force color output
lwangacat --theme default myfile.lwanga

# Check NO_COLOR variable
unset NO_COLOR
```

### Wrong Colors

**Problem**: Colors look wrong or unreadable

**Solutions**:
```bash
# Try different theme
lwangacat --theme dark myfile.lwanga  # For dark backgrounds
lwangacat --theme light myfile.lwanga  # For light backgrounds

# Check terminal color support
tput colors  # Should show 256
```

### Line Numbers Misaligned

**Problem**: Line numbers don't align properly

**Solution**:
```bash
# Use monospace font in terminal
# Ensure terminal width is sufficient
```

---

## Performance

### Large Files

For large files, use line ranges:

```bash
# Display first 100 lines
lwangacat --lines 1-100 large_file.lwanga

# Display around specific line
lwangacat --lines 500 --context 50 large_file.lwanga
```

### Batch Processing

Process multiple files efficiently:

```bash
# Parallel processing
find . -name "*.lwanga" | xargs -P 4 -I {} lwangacat {}
```

---

## See Also

- [Compiler Guide](compiler.md) - Using lwangac
- [Formatter Guide](formatter.md) - Code formatting
- [Tutorial](tutorial.md) - Learn Lwanga
- [VS Code Extension](../extensions/vscode/README.md) - IDE integration

---

**Back to**: [Documentation Index](README.md) | [Main README](../README.md)
