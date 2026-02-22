# Lwanga Vim Plugin

Vim syntax highlighting and filetype plugin for the Lwanga programming language.

## Features

- Syntax highlighting for all Lwanga constructs
- Automatic filetype detection for `.lwanga` files
- Smart indentation and formatting
- Comment handling
- Code folding
- Compiler integration
- Formatter integration

## Installation

### Using vim-plug
```vim
Plug 'lwanga-lang/lwanga.vim'
```

### Using Vundle
```vim
Plugin 'lwanga-lang/lwanga.vim'
```

### Using Pathogen
```bash
cd ~/.vim/bundle
git clone https://github.com/lwanga-lang/lwanga.vim.git
```

### Manual Installation
```bash
# Copy files to your Vim directory
cp -r syntax ~/.vim/
cp -r ftdetect ~/.vim/
cp -r ftplugin ~/.vim/
```

## Commands

When editing `.lwanga` files, the following commands are available:

- `:LwangaFormat` - Format the current file using `lwangafmt`
- `:LwangaAddHeader` - Add a logo header to the current file
- `:LwangaCompile` - Compile the current file
- `:LwangaCompileOptimized` - Compile with `-O2` optimization

## Keybindings

Default keybindings (with `<leader>` typically being `\`):

- `<leader>lf` - Format file
- `<leader>lh` - Add header
- `<leader>lc` - Compile
- `<leader>lo` - Compile optimized

## Configuration

Add to your `.vimrc`:

```vim
" Enable syntax highlighting
syntax on

" Enable filetype detection
filetype plugin indent on

" Lwanga-specific settings
autocmd FileType lwanga setlocal tabstop=4 shiftwidth=4 expandtab

" Custom keybindings (optional)
autocmd FileType lwanga nnoremap <buffer> <F5> :LwangaCompile<CR>
autocmd FileType lwanga nnoremap <buffer> <F6> :LwangaFormat<CR>
```

## Requirements

- Vim 8.0 or later (or Neovim)
- `lwangac` compiler (for compilation commands)
- `lwangafmt` formatter (for formatting commands)

## Syntax Highlighting

The plugin provides highlighting for:

- Keywords (`fn`, `let`, `mut`, `const`, `struct`, etc.)
- Types (`u8`, `u16`, `u32`, `u64`, `ptr`, `register`)
- Control flow (`if`, `else`, `while`, `return`)
- Unsafe blocks (highlighted as errors for visibility)
- Inline assembly (`asm`, `naked`)
- System calls (`syscall`)
- Encrypted strings (`enc`)
- Comments (line and block)
- Strings and escape sequences
- Numbers (decimal, hex, binary)
- Operators
- Preprocessor directives

## Color Schemes

The syntax highlighting works with any Vim color scheme. For best results, use a color scheme that supports:
- Keyword highlighting
- Type highlighting
- String highlighting
- Comment highlighting
- Special character highlighting

Recommended color schemes:
- gruvbox
- monokai
- solarized
- dracula
- nord

## License

MIT License
