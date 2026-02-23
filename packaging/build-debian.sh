#!/bin/bash
# Build Debian package for Lwanga
set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="1.0.0"

echo "Building Lwanga Debian package v${VERSION}..."

# Check if compiler is built
if [ ! -f "${PROJECT_ROOT}/compiler/build/lwangac" ]; then
    echo "Error: Compiler not built. Building now..."
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

# Move package to project root
cd "${PROJECT_ROOT}"
mv lwanga_${VERSION}-1_amd64.deb .
rm -rf "${BUILD_DIR}" lwanga_${VERSION}*

echo ""
echo "✓ Debian package created: lwanga_${VERSION}-1_amd64.deb"
echo ""
echo "To install:"
echo "  sudo dpkg -i lwanga_${VERSION}-1_amd64.deb"
echo "  sudo apt-get install -f"
