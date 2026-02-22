# Lwanga Formatter Guide

**Complete reference for `lwangafmt` - the Lwanga code formatter**

## Table of Contents

1. [Overview](#overview)
2. [Installation](#installation)
3. [Basic Usage](#basic-usage)
4. [Command Line Options](#command-line-options)
5. [Configuration](#configuration)
6. [Logo Headers](#logo-headers)
7. [Integration](#integration)
8. [Examples](#examples)

---

## Overview

`lwangafmt` is the official code formatter for Lwanga. It automatically formats your code according to consistent style rules and can add professional logo headers to your files.

### Features

- ✅ Consistent code formatting
- ✅ Logo header generation (3 styles)
- ✅ TOML configuration support
- ✅ Recursive directory formatting
- ✅ Check mode for CI/CD
- ✅ Colored terminal output
- ✅ Format on save support

---

## Installation

`lwangafmt` is included with the Lwanga compiler installation.

### Verify Installation

```bash
lwangafmt --help
```

### Standalone Installation

```bash
# From source
cd tools/formatter
mkdir build && cd build
cmake ..
make
sudo make install
```

---

## Basic Usage

### Format a Single File

```bash
lwangafmt --write myfile.lwanga
```

### Format Multiple Files

```bash
lwangafmt --write file1.lwanga file2.lwanga file3.lwanga
```

### Format Directory Recursively

```bash
lwangafmt --write --recursive src/
```

### Check Formatting (CI/CD)

```bash
lwangafmt --check src/**/*.lwanga
```

---

## Command Line Options

### Input/Output

| Option | Short | Description |
|--------|-------|-------------|
| `--write` | `-w` | Format files in-place |
| `--check` | `-c` | Check if files are formatted (exit 1 if not) |
| `--recursive` | `-r` | Process directories recursively |

### Logo Headers

| Option | Description |
|--------|-------------|
| `--add-header` | Add logo header to files |
| `--remove-header` | Remove logo header from files |
| `--logo-style <style>` | Logo style: minimal, block, banner |

### Output Control

| Option | Short | Description |
|--------|-------|-------------|
| `--verbose` | `-v` | Verbose output |
| `--no-color` | | Disable colored output |

### Help

| Option | Short | Description |
|--------|-------|-------------|
| `--help` | `-h` | Show help message |
| `--version` | | Show version |

---

## Configuration

### Configuration File

Create `.lwangafmt.toml` in your project root:

```toml
[formatting]
indent_width = 4
max_line_width = 100
brace_style = "kr"  # or "allman"
space_around_operators = true
trailing_commas = true
sort_imports = true

[header]
add_logo = true
logo_style = "minimal"  # "minimal", "block", "banner", "none"
include_metadata = true
author = "Your Name"
license = "MIT"

[assembly]
preserve_formatting = true
indent_instructions = true
```

### Configuration Options

#### Formatting Section

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `indent_width` | integer | 4 | Spaces per indentation level |
| `max_line_width` | integer | 100 | Maximum line length |
| `brace_style` | string | "kr" | Brace style (kr/allman) |
| `space_around_operators` | boolean | true | Add spaces around operators |
| `trailing_commas` | boolean | true | Add trailing commas |
| `sort_imports` | boolean | true | Sort import statements |

#### Header Section

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `add_logo` | boolean | false | Add logo to new files |
| `logo_style` | string | "minimal" | Logo style |
| `include_metadata` | boolean | true | Include file metadata |
| `author` | string | "" | Author name |
| `license` | string | "MIT" | License type |

#### Assembly Section

| Option | Type | Default | Description |
|--------|------|---------|-------------|
| `preserve_formatting` | boolean | true | Keep user formatting |
| `indent_instructions` | boolean | true | Indent assembly instructions |

---

## Logo Headers

### Logo Styles

#### Minimal (Default)

```lwanga
//  ╦  ╦ ╦╔═╗╔╗╔╔═╗╔═╗
//  ║  ║║║╠═╣║║║║ ╦╠═╣
//  ╩═╝╚╩╝╩ ╩╝╚╝╚═╝╩ ╩
//
//  File: example.lwanga
//  Author: Your Name
//  Created: 2026-02-22
//

fn main() -> u64 {
    return 0;
}
```

#### Block

```lwanga
//  ██╗  ██╗    ██╗ █████╗ ███╗   ██╗ ██████╗  █████╗ 
//  ██║  ██║ █╗ ██║██╔══██╗████╗  ██║██╔════╝ ██╔══██╗
//  ██║  ██║███╗██║███████║██╔██╗ ██║██║  ███╗███████║
//  ██║  ██║███╗██║██╔══██║██║╚██╗██║██║   ██║██╔══██║
//  ███████╗███╔███╔╝██║  ██║██║ ╚████║╚██████╔╝██║  ██║
//  ╚══════╝╚══╝╚══╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝ ╚═╝  ╚═╝
//
//  File: example.lwanga
//

fn main() -> u64 {
    return 0;
}
```

#### Banner

```lwanga
// ┌─────────────────────────────────────────┐
// │                                         │
// │   ██╗  ██╗    ██╗ █████╗ ███╗   ██╗    │
// │   ██║  ██║ █╗ ██║██╔══██╗████╗  ██║    │
// │   ██║  ██║███╗██║███████║██╔██╗ ██║    │
// │   ██║  ██║███╗██║██╔══██║██║╚██╗██║    │
// │   ███████╗███╔███╔╝██║  ██║██║ ╚████║   │
// │   ╚══════╝╚══╝╚══╝╚═╝  ╚═╝╚═╝  ╚═══╝   │
// │                                         │
// │   Security-Focused Systems Language    │
// │                                         │
// └─────────────────────────────────────────┘
//
//  File: example.lwanga
//  Project: MyProject
//  Author: Your Name
//  Created: 2026-02-22
//  License: MIT
//

fn main() -> u64 {
    return 0;
}
```

### Adding Headers

```bash
# Add minimal header
lwangafmt --add-header myfile.lwanga

# Add block header
lwangafmt --add-header --logo-style=block myfile.lwanga

# Add banner header
lwangafmt --add-header --logo-style=banner myfile.lwanga

# Add to all files
lwangafmt --add-header --recursive src/
```

### Removing Headers

```bash
# Remove header from file
lwangafmt --remove-header myfile.lwanga

# Remove from all files
lwangafmt --remove-header --recursive src/
```

---

## Integration

### VS Code

Add to `.vscode/settings.json`:

```json
{
  "lwanga.formatOnSave": true,
  "lwanga.formatterPath": "lwangafmt",
  "lwanga.logoStyle": "minimal"
}
```

Format with `Shift+Alt+F` or on save.

### Vim

Add to `.vimrc`:

```vim
autocmd FileType lwanga nnoremap <buffer> <leader>lf :!lwangafmt --write %<CR>
```

Format with `<leader>lf`.

### Git Pre-commit Hook

Create `.git/hooks/pre-commit`:

```bash
#!/bin/bash
# Format all staged .lwanga files

files=$(git diff --cached --name-only --diff-filter=ACM | grep '\.lwanga$')

if [ -n "$files" ]; then
    lwangafmt --write $files
    git add $files
fi
```

Make executable:
```bash
chmod +x .git/hooks/pre-commit
```

### CI/CD (GitHub Actions)

```yaml
name: Format Check

on: [push, pull_request]

jobs:
  format:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v2
      - name: Install Lwanga
        run: |
          wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.0/lwanga_1.0.0-1_amd64.deb
          sudo dpkg -i lwanga_1.0.0-1_amd64.deb
      - name: Check formatting
        run: lwangafmt --check src/**/*.lwanga
```

---

## Examples

### Example 1: Format Single File

**Before**:
```lwanga
fn main()->u64{let x:u64=42;return x;}
```

**Command**:
```bash
lwangafmt --write example.lwanga
```

**After**:
```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    return x;
}
```

### Example 2: Format with Header

**Command**:
```bash
lwangafmt --write --add-header example.lwanga
```

**Result**:
```lwanga
//  ╦  ╦ ╦╔═╗╔╗╔╔═╗╔═╗
//  ║  ║║║╠═╣║║║║ ╦╠═╣
//  ╩═╝╚╩╝╩ ╩╝╚╝╚═╝╩ ╩
//
//  File: example.lwanga
//  Created: 2026-02-22
//

fn main() -> u64 {
    let x: u64 = 42;
    return x;
}
```

### Example 3: Format Directory

```bash
# Format all .lwanga files in src/
lwangafmt --write --recursive src/

# With verbose output
lwangafmt --write --recursive --verbose src/
```

Output:
```
Formatting src/main.lwanga... ✓
Formatting src/utils.lwanga... ✓
Formatting src/lib.lwanga... ✓

3 files formatted successfully
```

### Example 4: Check Formatting

```bash
lwangafmt --check src/**/*.lwanga
```

Output if formatted:
```
✓ All files are properly formatted
```

Output if not formatted:
```
✗ The following files need formatting:
  - src/main.lwanga
  - src/utils.lwanga

Run: lwangafmt --write src/**/*.lwanga
```

---

## Formatting Rules

### Indentation

- 4 spaces per level (configurable)
- No tabs

### Braces

K&R style (default):
```lwanga
fn main() -> u64 {
    if x > 0 {
        return 1;
    }
    return 0;
}
```

Allman style:
```lwanga
fn main() -> u64
{
    if x > 0
    {
        return 1;
    }
    return 0;
}
```

### Operators

Spaces around operators:
```lwanga
let x: u64 = a + b * c;
let y: u64 = (a + b) / c;
```

### Function Calls

```lwanga
// Short
let result: u64 = add(a, b);

// Long (multi-line)
let result: u64 = very_long_function_name(
    first_argument,
    second_argument,
    third_argument
);
```

### Imports

Sorted alphabetically:
```lwanga
import "std/io.lwanga";
import "std/mem.lwanga";
import "std/str.lwanga";
```

---

## Troubleshooting

### Formatter Not Found

```bash
# Check installation
which lwangafmt

# Add to PATH
export PATH=$PATH:/usr/local/bin
```

### Configuration Not Loaded

```bash
# Check config file location
ls -la .lwangafmt.toml

# Verify syntax
cat .lwangafmt.toml
```

### Permission Denied

```bash
# Make formatter executable
chmod +x /usr/local/bin/lwangafmt

# Or run with sudo
sudo lwangafmt --write myfile.lwanga
```

---

## See Also

- [Style Guide](style-guide.md) - Coding conventions
- [Tutorial](tutorial.md) - Learn Lwanga
- [Compiler Guide](compiler.md) - Using lwangac
- [VS Code Extension](../extensions/vscode/README.md) - IDE integration

---

**Back to**: [Documentation Index](README.md) | [Main README](../README.md)
