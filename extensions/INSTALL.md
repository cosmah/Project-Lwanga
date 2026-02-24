# Lwanga IDE Extensions Installation Guide

This directory contains IDE and editor extensions for the Lwanga programming language.

## VS Code Extension

### Quick Install
```bash
cd extensions/vscode
npm install
npm run compile
npm run package
code --install-extension lwanga-1.0.2.vsix
```

### From Marketplace (when published)
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "Lwanga"
4. Click Install

### Manual Installation
1. Download `lwanga-1.0.2.vsix`
2. Open VS Code
3. Go to Extensions
4. Click "..." menu → "Install from VSIX"
5. Select the downloaded file

### Configuration
Open VS Code settings (Ctrl+,) and configure:
```json
{
  "lwanga.compilerPath": "/usr/local/bin/lwangac",
  "lwanga.formatterPath": "/usr/local/bin/lwangafmt",
  "lwanga.formatOnSave": true,
  "lwanga.compilerArgs": ["-O2"],
  "lwanga.logoStyle": "minimal"
}
```

## Vim/Neovim Plugin

### Using vim-plug
Add to your `.vimrc` or `init.vim`:
```vim
Plug 'lwanga-lang/lwanga.vim'
```

Then run `:PlugInstall`

### Using Vundle
Add to your `.vimrc`:
```vim
Plugin 'lwanga-lang/lwanga.vim'
```

Then run `:PluginInstall`

### Manual Installation
```bash
# For Vim
mkdir -p ~/.vim/{syntax,ftdetect,ftplugin}
cp extensions/vim/syntax/lwanga.vim ~/.vim/syntax/
cp extensions/vim/ftdetect/lwanga.vim ~/.vim/ftdetect/
cp extensions/vim/ftplugin/lwanga.vim ~/.vim/ftplugin/

# For Neovim
mkdir -p ~/.config/nvim/{syntax,ftdetect,ftplugin}
cp extensions/vim/syntax/lwanga.vim ~/.config/nvim/syntax/
cp extensions/vim/ftdetect/lwanga.vim ~/.config/nvim/ftdetect/
cp extensions/vim/ftplugin/lwanga.vim ~/.config/nvim/ftplugin/
```

### Configuration
Add to your `.vimrc` or `init.vim`:
```vim
" Enable syntax highlighting
syntax on
filetype plugin indent on

" Lwanga settings
autocmd FileType lwanga setlocal tabstop=4 shiftwidth=4 expandtab

" Keybindings
autocmd FileType lwanga nnoremap <buffer> <F5> :LwangaCompile<CR>
autocmd FileType lwanga nnoremap <buffer> <F6> :LwangaFormat<CR>
```

## System-Wide File Association (Linux)

### MIME Type Registration
```bash
# Create MIME type definition
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

# Update MIME database
sudo update-mime-database /usr/share/mime
```

### File Icon Installation
```bash
# Install icons
for size in 16 32 64 128 256; do
    sudo cp assets/icons/lwanga-file-${size}.png \
        /usr/share/icons/hicolor/${size}x${size}/mimetypes/text-x-lwanga.png
done

# Update icon cache
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

### Desktop File Association
```bash
# Create desktop entry
tee ~/.local/share/applications/lwanga.desktop > /dev/null << 'EOF'
[Desktop Entry]
Type=Application
Name=Lwanga Compiler
Comment=Compile Lwanga source files
Exec=lwangac %f -o %f.out
Icon=text-x-lwanga
Terminal=true
MimeType=text/x-lwanga;
Categories=Development;
EOF

# Update desktop database
update-desktop-database ~/.local/share/applications/
```

## Emacs Mode (Coming Soon)

Emacs support is planned for a future release.

## Sublime Text (Coming Soon)

Sublime Text support is planned for a future release.

## Testing Your Installation

### VS Code
1. Open a `.lwanga` file
2. Verify syntax highlighting is active
3. Try formatting with Shift+Alt+F
4. Try compiling with Command Palette → "Lwanga: Compile Current File"

### Vim
1. Open a `.lwanga` file
2. Verify syntax highlighting: `:set syntax?` should show `syntax=lwanga`
3. Try formatting: `:LwangaFormat`
4. Try compiling: `:LwangaCompile`

## Troubleshooting

### VS Code: "Command not found: lwangac"
- Ensure `lwangac` is in your PATH
- Or set full path in settings: `"lwanga.compilerPath": "/full/path/to/lwangac"`

### Vim: Syntax highlighting not working
- Ensure filetype detection is enabled: `:filetype plugin on`
- Check if file is detected: `:set filetype?`
- Manually set if needed: `:set filetype=lwanga`

### Icons not showing
- Ensure icon files are copied to the correct location
- Update icon cache: `sudo gtk-update-icon-cache /usr/share/icons/hicolor/`
- Restart your file manager

## Uninstallation

### VS Code
```bash
code --uninstall-extension lwanga-lang.lwanga
```

### Vim
```bash
# Remove plugin files
rm -rf ~/.vim/syntax/lwanga.vim
rm -rf ~/.vim/ftdetect/lwanga.vim
rm -rf ~/.vim/ftplugin/lwanga.vim
```

### System Files
```bash
# Remove MIME type
sudo rm /usr/share/mime/packages/lwanga.xml
sudo update-mime-database /usr/share/mime

# Remove icons
sudo rm /usr/share/icons/hicolor/*/mimetypes/text-x-lwanga.png
sudo gtk-update-icon-cache /usr/share/icons/hicolor/

# Remove desktop entry
rm ~/.local/share/applications/lwanga.desktop
update-desktop-database ~/.local/share/applications/
```

## Support

For issues and questions:
- GitHub Issues: https://github.com/lwanga-lang/lwanga/issues
- Documentation: https://lwanga-lang.org/docs
