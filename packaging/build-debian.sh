#!/bin/bash
# Build Debian package for Lwanga
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="1.0.2"

echo "Building Lwanga Debian package v${VERSION}..."

# Check if compiler is built
if [ ! -f "${PROJECT_ROOT}/compiler/build/lwangac" ]; then
    echo "Error: Compiler not built. Building now..."
    mkdir -p "${PROJECT_ROOT}/compiler/build"
    cd "${PROJECT_ROOT}/compiler/build"
    cmake .. -DCMAKE_BUILD_TYPE=Release
    make -j$(nproc)
fi

# Create build directory
BUILD_DIR="${PROJECT_ROOT}/lwanga-${VERSION}"
rm -rf "${BUILD_DIR}"
mkdir -p "${BUILD_DIR}"

# Copy source
echo "Copying source files..."
rsync -a --exclude='.git' \
         --exclude='compiler/build' \
         --exclude='release' \
         --exclude='*.o' \
         --exclude='*.a' \
         "${PROJECT_ROOT}/" "${BUILD_DIR}/"

# Copy debian files
cp -r "${PROJECT_ROOT}/packaging/debian" "${BUILD_DIR}/"

# Build package
cd "${BUILD_DIR}"
echo "Building Debian package..."
dpkg-buildpackage -us -uc -b

# Determine architecture
ARCH=$(dpkg --print-architecture)
PKG="lwanga_${VERSION}-1_${ARCH}.deb"

# Move package to project root
cd "${PROJECT_ROOT}"
mv "${PKG}" .

# Clean up build artifacts (but not the .deb we just moved)
rm -rf "${BUILD_DIR}"
rm -f lwanga_${VERSION}-1_${ARCH}.buildinfo
rm -f lwanga_${VERSION}-1_${ARCH}.changes
rm -f lwanga-dbgsym_${VERSION}-1_${ARCH}.ddeb

echo ""
echo "✓ Debian package created: ${PKG}"
echo ""
echo "To install:"
echo "  sudo dpkg -i ${PKG}"
echo "  sudo apt-get install -f"
