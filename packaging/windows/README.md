# Lwanga Windows Installation

Complete guide for building and installing Lwanga on Windows.

## Prerequisites

### Required Software

1. **Visual Studio 2019 or later**
   - Download: https://visualstudio.microsoft.com/downloads/
   - Required components:
     - Desktop development with C++
     - CMake tools for Windows
     - Windows 10 SDK

2. **LLVM 18**
   - Download: https://github.com/llvm/llvm-project/releases/tag/llvmorg-18.1.3
   - Get: `LLVM-18.1.3-win64.exe`
   - Install to: `C:\Program Files\LLVM`

3. **CMake 3.15+**
   - Download: https://cmake.org/download/
   - Or install via Visual Studio

4. **Git for Windows**
   - Download: https://git-scm.com/download/win
   - Required for cloning repository

### Optional Software

1. **NSIS (Nullsoft Scriptable Install System)**
   - Download: https://nsis.sourceforge.io/Download
   - Required only for creating installer
   - Install to default location

2. **Windows Terminal**
   - Download from Microsoft Store
   - Better terminal experience

## Quick Install (Pre-built Binary)

### Download Installer
```powershell
# Download from GitHub Releases (when available)
Invoke-WebRequest -Uri "https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.0/lwanga-1.0.0-windows-x64-installer.exe" -OutFile "lwanga-installer.exe"

# Run installer
.\lwanga-installer.exe
```

### Verify Installation
```powershell
lwangac --version
lwangafmt --help
lwangacat --help
```

## Build from Source

### 1. Clone Repository
```powershell
git clone https://github.com/cosmah/Project-Lwanga.git
cd Project-Lwanga
```

### 2. Build with PowerShell Script
```powershell
# Run build script
.\packaging\windows\build-windows.ps1

# Or with custom LLVM path
.\packaging\windows\build-windows.ps1 -LLVMPath "C:\LLVM"

# Or specify build type
.\packaging\windows\build-windows.ps1 -BuildType Debug
```

### 3. Manual Build
```powershell
# Create build directory
mkdir build-windows
cd build-windows

# Configure with CMake
cmake ..\compiler -G "Visual Studio 17 2022" -A x64 `
    -DCMAKE_BUILD_TYPE=Release `
    -DLLVM_DIR="C:\Program Files\LLVM\lib\cmake\llvm"

# Build
cmake --build . --config Release --parallel

# Run tests
ctest -C Release --output-on-failure

# Install
cmake --install . --config Release --prefix install
```

### 4. Add to PATH
```powershell
# Temporary (current session only)
$env:PATH += ";$PWD\build-windows\install\bin"

# Permanent (requires admin)
[Environment]::SetEnvironmentVariable(
    "PATH",
    [Environment]::GetEnvironmentVariable("PATH", "Machine") + ";C:\Program Files\Lwanga\bin",
    "Machine"
)
```

## Create Installer

### Prerequisites
- NSIS installed
- Lwanga built successfully

### Build Installer
```powershell
# Build binaries first
.\packaging\windows\build-windows.ps1

# Create installer with NSIS
cd packaging\windows
makensis installer.nsi

# Output: lwanga-1.0.0-windows-x64-installer.exe
```

## Using Lwanga on Windows

### Compile a Program
```powershell
# Create test file
@"
fn main() -> u64 {
    return 0;
}
"@ | Out-File -Encoding UTF8 test.lwanga

# Compile
lwangac test.lwanga -o test.exe

# Run
.\test.exe
echo $LASTEXITCODE  # Should be 0
```

### Format Code
```powershell
lwangafmt --write myfile.lwanga
```

### Syntax Highlighting
```powershell
lwangacat -n myfile.lwanga
```

## Windows-Specific Features

### Command Prompt vs PowerShell
Both work, but PowerShell is recommended for better Unicode support and colors.

### File Associations
The installer automatically associates `.lwanga` files with the compiler.

### Context Menu Integration
Right-click `.lwanga` files to:
- Compile with Lwanga
- Format with lwangafmt
- View with lwangacat

### Visual Studio Integration
Use the VS Code extension for the best Windows development experience.

## WSL (Windows Subsystem for Linux)

For a Linux-like experience on Windows:

```powershell
# Install WSL
wsl --install

# Inside WSL, follow Linux installation
wsl
git clone https://github.com/cosmah/Project-Lwanga.git
cd Project-Lwanga
# Follow Linux build instructions
```

## Troubleshooting

### LLVM Not Found
```powershell
# Verify LLVM installation
Test-Path "C:\Program Files\LLVM\bin\llvm-config.exe"

# Set LLVM_DIR explicitly
$env:LLVM_DIR = "C:\Program Files\LLVM\lib\cmake\llvm"
```

### Visual Studio Not Found
```powershell
# Check Visual Studio installation
& "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"

# Or use CMake with different generator
cmake .. -G "Ninja" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
```

### Permission Denied
```powershell
# Run PowerShell as Administrator
Start-Process powershell -Verb RunAs

# Or install to user directory
cmake --install . --prefix "$env:USERPROFILE\lwanga"
```

### Tests Failing
```powershell
# Run tests with verbose output
ctest -C Release --output-on-failure --verbose

# Run specific test
ctest -C Release -R lexer_test --verbose
```

### PATH Not Updated
```powershell
# Refresh environment variables
$env:PATH = [System.Environment]::GetEnvironmentVariable("PATH","Machine") + ";" + [System.Environment]::GetEnvironmentVariable("PATH","User")

# Or restart terminal
```

## IDE Setup

### Visual Studio Code
```powershell
# Install extension
code --install-extension lwanga-1.0.0.vsix

# Or from marketplace (when published)
# Search for "Lwanga" in Extensions
```

### Visual Studio
Use the "Open Folder" feature to work with Lwanga projects.

## Performance Notes

### Build Time
- First build: ~10-15 minutes
- Incremental builds: ~2-3 minutes
- Tests: ~1 minute

### Binary Size
- lwangac.exe: ~5MB
- lwangafmt.exe: ~2MB
- lwangacat.exe: ~2MB
- Hello World output: ~10KB

## Uninstallation

### Using Installer
```powershell
# Run uninstaller
& "C:\Program Files\Lwanga\uninst.exe"

# Or from Control Panel
# Settings > Apps > Lwanga > Uninstall
```

### Manual Uninstall
```powershell
# Remove binaries
Remove-Item -Recurse "C:\Program Files\Lwanga"

# Remove from PATH
# Settings > System > About > Advanced system settings > Environment Variables
# Edit PATH and remove Lwanga entry

# Remove registry keys
Remove-Item "HKLM:\Software\Microsoft\Windows\CurrentVersion\Uninstall\Lwanga"
```

## Getting Help

- **Documentation**: https://github.com/cosmah/Project-Lwanga/tree/main/docs
- **Issues**: https://github.com/cosmah/Project-Lwanga/issues
- **Email**: cosmahke4@gmail.com

## Known Limitations on Windows

- No native syscall support (Windows uses different syscall interface)
- Inline assembly may have platform-specific differences
- Some examples designed for Linux may not work directly
- Cross-compilation to Linux targets works

## Future Windows Features

- Native Windows syscall support
- Windows-specific examples
- Visual Studio extension
- Chocolatey package
- Scoop package
- WinGet package

---

**Cosmc Technologies** | https://github.com/cosmah/Project-Lwanga
