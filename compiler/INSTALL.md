# Installation Guide

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt update
sudo apt install -y build-essential cmake llvm-18-dev clang-18
```

### Fedora/RHEL
```bash
sudo dnf install -y gcc-c++ cmake llvm-devel clang
```

### macOS
```bash
brew install cmake llvm
```

## Building from Source

1. Clone the repository and navigate to the compiler directory:
```bash
cd compiler
```

2. Run the build script:
```bash
chmod +x build.sh
./build.sh
```

Or manually:
```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

3. (Optional) Install system-wide:
```bash
sudo make install
```

## Verifying Installation

Test the compiler:
```bash
./build/lwangac --help
./build/lwangac -v test.lwanga -o test
```

## Troubleshooting

### LLVM not found
If CMake cannot find LLVM, specify the LLVM directory:
```bash
cmake -DLLVM_DIR=/usr/lib/llvm-18/cmake ..
```

### Version mismatch
Ensure you have LLVM 13 or later:
```bash
llvm-config --version
```
