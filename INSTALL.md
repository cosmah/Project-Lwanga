# Lwanga Installation Guide

Complete installation instructions for the Lwanga programming language compiler and tools.

## Table of Contents
- [System Requirements](#system-requirements)
- [Quick Install](#quick-install)
- [Build from Source](#build-from-source)
- [Package Managers](#package-managers)
- [Docker](#docker)
- [IDE Extensions](#ide-extensions)
- [Verification](#verification)
- [Troubleshooting](#troubleshooting)

## System Requirements

### Minimum Requirements
- **OS**: Linux (Ubuntu 20.04+, Arch, Debian 11+), macOS 11+, Windows 10/11
- **CPU**: x86_64 or ARM64
- **RAM**: 2GB
- **Disk**: 500MB

### Dependencies
- LLVM 18
- CMake 3.15+
- C++17 compiler (GCC 9+, Clang 10+, or MSVC 2019+)
- Git (for source builds)

## Quick Install

### Ubuntu/Debian
```bash
# Add repository (when available)
sudo add-apt-repository ppa:cosmc-technologies/lwanga
sudo apt update
sudo apt install lwanga
```

### Arch Linux
```bash
# From AUR (when available)
yay -S lwanga
# or
paru -S lwanga
```

### Docker
```bash
docker pull cosmctechnologies/lwanga:latest
docker run -it --rm cosmctechnologies/lwanga:latest
```

### Windows

#### Installer (Recommended)
```powershell
# Download and run installer
Invoke-WebRequest -Uri "https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.0/lwanga-1.0.0-windows-x64-installer.exe" -OutFile "lwanga-installer.exe"
.\lwanga-installer.exe
```

#### Chocolatey
```powershell
choco install lwanga
```

#### Scoop
```powershell
scoop bucket add cosmc https://github.com/cosmah/scoop-bucket
scoop install lwanga
```

## Build from Source

### 1. Install Dependencies

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y \
    build-essential \
    cmake \
    llvm-18 \
    llvm-18-dev \
    clang-18 \
    git
```

#### Arch Linux
```bash
sudo pacman -S base-devel cmake llvm clang git
```

#### macOS
```bash
brew install cmake llvm@18
```

### 2. Clone Repository
```bash
git clone https://github.com/cosmah/Project-Lwanga.git
cd Project-Lwanga
```

### 3. Build
```bash
cd compiler
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j$(nproc)
```

### 4. Run Tests
```bash
ctest --output-on-failure
```

### 5. Install
```bash
sudo make install
# Or install to custom location
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make install
```

## Package Managers

### Windows Installer (.exe)

#### Build Installer
```powershell
# Build binaries first
.\packaging\windows\build-windows.ps1

# Create installer with NSIS
cd packaging\windows
makensis installer.nsi
```

#### Install
```powershell
.\lwanga-1.0.0-windows-x64-installer.exe
```

### Chocolatey Package

#### Create Package
```powershell
cd packaging\windows\chocolatey
choco pack
```

#### Install
```powershell
choco install lwanga -source .
```

### Debian Package (.deb)

#### Build Package
```bash
cd packaging/debian
dpkg-buildpackage -us -uc -b
```

#### Install Package
```bash
sudo dpkg -i ../lwanga_1.0.0-1_amd64.deb
sudo apt-get install -f  # Fix dependencies if needed
```

### Arch Package (PKGBUILD)

#### Build Package
```bash
cd packaging/arch
makepkg -si
```

### RPM Package (Fedora/RHEL)

#### Build Package
```bash
cd packaging/rpm
rpmbuild -ba lwanga.spec
```

## Docker

### Pull Image
```bash
docker pull cosmctechnologies/lwanga:latest
```

### Build Image
```bash
docker build -t lwanga:latest -f packaging/docker/Dockerfile .
```

### Run Container
```bash
# Interactive shell
docker run -it --rm lwanga:latest

# Mount workspace
docker run -it --rm -v $(pwd):/workspace lwanga:latest

# Compile file
docker run -it --rm -v $(pwd):/workspace lwanga:latest \
    lwangac hello.lwanga -o hello
```

## IDE Extensions

### VS Code

#### From VSIX
```bash
cd extensions/vscode
npm install
npm run compile
npx @vscode/vsce package
code --install-extension lwanga-1.0.0.vsix
```

#### From Marketplace (when published)
1. Open VS Code
2. Go to Extensions (Ctrl+Shift+X)
3. Search for "Lwanga"
4. Click Install

### Vim/Neovim

#### Manual Installation
```bash
# Vim
mkdir -p ~/.vim/{syntax,ftdetect,ftplugin}
cp extensions/vim/syntax/lwanga.vim ~/.vim/syntax/
cp extensions/vim/ftdetect/lwanga.vim ~/.vim/ftdetect/
cp extensions/vim/ftplugin/lwanga.vim ~/.vim/ftplugin/

# Neovim
mkdir -p ~/.config/nvim/{syntax,ftdetect,ftplugin}
cp extensions/vim/syntax/lwanga.vim ~/.config/nvim/syntax/
cp extensions/vim/ftdetect/lwanga.vim ~/.config/nvim/ftdetect/
cp extensions/vim/ftplugin/lwanga.vim ~/.config/nvim/ftplugin/
```

#### Using Plugin Manager
```vim
" vim-plug
Plug 'cosmah/Project-Lwanga', {'rtp': 'extensions/vim'}

" Vundle
Plugin 'cosmah/Project-Lwanga', {'rtp': 'extensions/vim'}
```

## Verification

### Check Installation
```bash
# Check compiler
lwangac --version

# Check formatter
lwangafmt --help

# Check highlighter
lwangacat --help
```

### Compile Test Program
```bash
# Create test file
cat > test.lwanga << 'EOF'
fn main() -> u64 {
    return 0;
}
EOF

# Compile
lwangac test.lwanga -o test

# Run
./test
echo $?  # Should print 0
```

### Run Full Test Suite
```bash
cd compiler/build
ctest --output-on-failure
```

## File Type Association (Linux)

### Install MIME Type
```bash
sudo cp packaging/lwanga.xml /usr/share/mime/packages/
sudo update-mime-database /usr/share/mime
```

### Install File Icons
```bash
for size in 16 32 64 128 256; do
    sudo cp assets/icons/lwanga-file-${size}.png \
        /usr/share/icons/hicolor/${size}x${size}/mimetypes/text-x-lwanga.png
done
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

## Troubleshooting

### LLVM Not Found
```bash
# Ubuntu/Debian
sudo apt install llvm-18-dev

# Set LLVM path explicitly
cmake .. -DLLVM_DIR=/usr/lib/llvm-18/lib/cmake/llvm
```

### Permission Denied
```bash
# Make binaries executable
chmod +x compiler/build/lwangac
chmod +x compiler/build/formatter/lwangafmt
chmod +x compiler/build/highlighter/lwangacat
```

### Tests Failing
```bash
# Run tests with verbose output
cd compiler/build
ctest --output-on-failure --verbose
```

### VS Code Extension Not Working
```bash
# Ensure Node.js is installed
node --version  # Should be 18+

# Rebuild extension
cd extensions/vscode
rm -rf node_modules
npm install
npm run compile
```

## Uninstallation

### From Package
```bash
# Ubuntu/Debian
sudo apt remove lwanga

# Arch
sudo pacman -R lwanga
```

### From Source
```bash
cd compiler/build
sudo make uninstall
```

### Remove Configuration
```bash
rm -rf ~/.lwanga
rm -rf ~/.config/lwanga
```

## Getting Help

- **Documentation**: https://github.com/cosmah/Project-Lwanga/tree/main/docs
- **Issues**: https://github.com/cosmah/Project-Lwanga/issues
- **Email**: cosmahke4@gmail.com

## License

MIT License - See LICENSE file for details

---

**Cosmc Technologies** | https://github.com/cosmah/Project-Lwanga
