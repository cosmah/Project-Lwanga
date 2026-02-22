# lwangacat - Lwanga Syntax Highlighter

A terminal-based syntax highlighter for Lwanga source code with multiple color themes.

## Features

- Syntax highlighting for all Lwanga constructs
- Multiple color themes (default, dark, light, monokai)
- Line numbers with optional highlighting
- Context display for error messages
- Pipe support for integration with other tools
- Automatic color detection (disables colors when piping)

## Usage

```bash
# Basic highlighting
lwangacat file.lwanga

# With line numbers
lwangacat -n file.lwanga

# Choose a theme
lwangacat -t monokai file.lwanga

# Highlight specific line (shows context)
lwangacat -n -h 10 file.lwanga

# Show only first N lines
lwangacat -n -l 20 file.lwanga

# Pipe from stdin
cat file.lwanga | lwangacat

# Disable colors
lwangacat --no-color file.lwanga
```

## Color Themes

### Default
Balanced colors suitable for most terminals.

### Dark
Bright colors optimized for dark backgrounds.

### Light
Subdued colors optimized for light backgrounds.

### Monokai
Popular color scheme inspired by Sublime Text.

## Integration

### With less
```bash
lwangacat file.lwanga | less -R
```

### With grep
```bash
lwangacat file.lwanga | grep "function"
```

### In scripts
```bash
#!/bin/bash
for file in *.lwanga; do
    echo "=== $file ==="
    lwangacat -n "$file"
done
```

## Building

Built automatically with the main compiler:

```bash
cd compiler/build
cmake ..
make lwangacat
```

Binary location: `compiler/build/highlighter/lwangacat`

## Implementation

- Uses the Lwanga lexer for accurate tokenization
- ANSI escape codes for terminal colors
- Reuses compiler infrastructure for consistency
- Zero external dependencies beyond LLVM
