#!/bin/bash
# Build all distribution packages for Lwanga
# Cosmc Technologies

set -e

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
VERSION="1.0.2"

echo "Building Lwanga v${VERSION} packages..."
echo "Project root: ${PROJECT_ROOT}"
echo ""

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Build Debian package
build_debian() {
    echo -e "${YELLOW}Building Debian package...${NC}"
    cd "${PROJECT_ROOT}"
    
    # Create build directory
    BUILD_DIR="lwanga-${VERSION}-build"
    rm -rf "${BUILD_DIR}"
    mkdir -p "${BUILD_DIR}"
    
    # Copy source files (excluding git and build artifacts)
    rsync -a --exclude='.git' --exclude='compiler/build' --exclude="${BUILD_DIR}" ./ "${BUILD_DIR}/"
    cp -r packaging/debian "${BUILD_DIR}/"
    
    # Build
    cd "${BUILD_DIR}"
    dpkg-buildpackage -us -uc -b
    
    cd "${PROJECT_ROOT}"
    echo -e "${GREEN}✓ Debian package built: lwanga_${VERSION}-1_amd64.deb${NC}"
    # Cleanup
    rm -rf "${BUILD_DIR}"
}

# Build Arch package
build_arch() {
    echo -e "${YELLOW}Building Arch package...${NC}"
    cd "${PROJECT_ROOT}/packaging/arch"
    
    makepkg -sf --noconfirm
    
    echo -e "${GREEN}✓ Arch package built: lwanga-${VERSION}-1-x86_64.pkg.tar.zst${NC}"
}

# Build Docker image
build_docker() {
    echo -e "${YELLOW}Building Docker image...${NC}"
    cd "${PROJECT_ROOT}"
    
    docker build -t cosmctechnologies/lwanga:${VERSION} \
                 -t cosmctechnologies/lwanga:latest \
                 -f packaging/docker/Dockerfile .
    
    echo -e "${GREEN}✓ Docker image built: cosmctechnologies/lwanga:${VERSION}${NC}"
}

# Build source tarball
build_source() {
    echo -e "${YELLOW}Building source tarball...${NC}"
    cd "${PROJECT_ROOT}"
    
    mkdir -p dist
    git archive --format=tar.gz --prefix=lwanga-${VERSION}/ HEAD > dist/lwanga-${VERSION}.tar.gz
    
    echo -e "${GREEN}✓ Source tarball: dist/lwanga-${VERSION}.tar.gz${NC}"
}

# Main menu
case "${1}" in
    debian)
        build_debian
        ;;
    arch)
        build_arch
        ;;
    docker)
        build_docker
        ;;
    source)
        build_source
        ;;
    all)
        build_source
        build_debian
        build_arch
        build_docker
        ;;
    *)
        echo "Usage: $0 {debian|arch|docker|source|all}"
        echo ""
        echo "Options:"
        echo "  debian  - Build Debian/Ubuntu .deb package"
        echo "  arch    - Build Arch Linux package"
        echo "  docker  - Build Docker image"
        echo "  source  - Create source tarball"
        echo "  all     - Build all packages"
        exit 1
        ;;
esac

echo ""
echo -e "${GREEN}Build complete!${NC}"
