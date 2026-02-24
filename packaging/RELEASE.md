# Lwanga v1.0.2 Release Guide

Complete guide for releasing Lwanga v1.0.2 to the public.

## Pre-Release Checklist

### Code Quality
- [x] All 20 tests passing
- [x] No compiler warnings
- [x] Code formatted with lwangafmt
- [x] Documentation complete
- [x] Examples working

### Packaging
- [x] Debian package configured
- [x] Arch PKGBUILD ready
- [x] Docker image configured
- [x] Source tarball script ready

### Documentation
- [x] README.md complete
- [x] INSTALL.md comprehensive
- [x] LICENSE file present
- [x] CHANGELOG.md created
- [x] API documentation complete

### Extensions
- [x] VS Code extension ready
- [x] Vim plugin ready
- [x] File icons created
- [x] MIME types configured

## Release Steps

### 1. Version Bump
```bash
# Update version in all files
VERSION="1.0.2"

# Update package.json
sed -i 's/"version": ".*"/"version": "'$VERSION'"/' extensions/vscode/package.json

# Update PKGBUILD
sed -i 's/pkgver=.*/pkgver='$VERSION'/' packaging/arch/PKGBUILD

# Update Debian changelog
dch -v ${VERSION}-1 "Release version ${VERSION}"
```

### 2. Build All Packages
```bash
# Build everything
./packaging/build-packages.sh all

# Verify builds
ls -lh dist/
ls -lh packaging/arch/*.pkg.tar.zst
docker images | grep lwanga
```

### 3. Run Full Test Suite
```bash
cd compiler/build
ctest --output-on-failure -j$(nproc)

# Test packages
sudo dpkg -i ../lwanga_${VERSION}-1_amd64.deb
lwangac --version
lwangafmt --help
lwangacat --help
```

### 4. Create Git Tag
```bash
git tag -a v${VERSION} -m "Release version ${VERSION}"
git push origin v${VERSION}
```

### 5. Create GitHub Release

1. Go to https://github.com/cosmah/Project-Lwanga/releases/new
2. Tag: `v1.0.2`
3. Title: `Lwanga v1.0.2 - Hardening & Preprocessor Update`
4. Description: See CHANGELOG.md
5. Attach files:
   - `dist/lwanga-1.0.2.tar.gz` (source)
   - `lwanga_1.0.2-1_amd64.deb` (Debian package)
   - `lwanga-1.0.2-1-x86_64.pkg.tar.zst` (Arch package)
   - `extensions/vscode/lwanga-1.0.2.vsix` (VS Code extension)

### 6. Publish Docker Image
```bash
# Login to Docker Hub
docker login

# Push images
docker push cosmctechnologies/lwanga:1.0.2
docker push cosmctechnologies/lwanga:latest
```

### 7. Publish VS Code Extension
```bash
cd extensions/vscode

# Login to VS Code Marketplace
npx @vscode/vsce login cosmc-technologies

# Publish
npx @vscode/vsce publish
```

### 8. Submit to Package Repositories

#### Debian/Ubuntu PPA
```bash
# Create PPA on Launchpad
# Upload package
dput ppa:cosmc-technologies/lwanga lwanga_1.0.2-1_source.changes
```

#### Arch AUR
```bash
cd packaging/arch

# Update .SRCINFO
makepkg --printsrcinfo > .SRCINFO

# Push to AUR
git clone ssh://aur@aur.archlinux.org/lwanga.git aur-lwanga
cp PKGBUILD .SRCINFO aur-lwanga/
cd aur-lwanga
git add PKGBUILD .SRCINFO
git commit -m "Initial release: v1.0.2"
git push
```

### 9. Update Documentation Website

```bash
# Build documentation
cd docs
# Generate API docs, tutorials, etc.

# Deploy to GitHub Pages
git checkout gh-pages
cp -r docs/_build/html/* .
git add .
git commit -m "Update documentation for v1.0.2"
git push
```

### 10. Announce Release

#### GitHub
- Create release announcement
- Update README with installation instructions

#### Social Media
- Twitter/X
- Reddit (r/programming, r/rust, r/lowlevel)
- Hacker News
- Dev.to

#### Email
- Notify early testers
- Send to security mailing lists

## Post-Release

### Monitor
- GitHub issues
- Package installation reports
- User feedback

### Update
- Fix critical bugs immediately
- Plan v1.0.1 patch release if needed

### Metrics
- Track downloads
- Monitor Docker pulls
- VS Code extension installs
- GitHub stars/forks

## Release Announcement Template

```markdown
# Lwanga v1.0.2 Released! 🎉

We're excited to announce the v1.0.2 release of Lwanga, a security-focused systems programming language designed for low-level development, shellcode, and penetration testing.

## What is Lwanga?

Lwanga is a compiled systems language that provides:
- Direct syscall support
- Inline assembly (Intel & AT&T syntax)
- Position-independent code generation
- Encrypted string literals
- Minimal runtime overhead
- LLVM-based native code generation

## Installation

### Ubuntu/Debian
```bash
wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.2/lwanga_1.0.2-1_amd64.deb
sudo dpkg -i lwanga_1.0.2-1_amd64.deb
```

### Arch Linux
```bash
yay -S lwanga
```

### Docker
```bash
docker pull cosmctechnologies/lwanga:latest
```

## Features

- ✅ Full compiler with LLVM backend
- ✅ Code formatter (lwangafmt)
- ✅ Syntax highlighter (lwangacat)
- ✅ VS Code extension
- ✅ Vim plugin
- ✅ 20+ passing tests
- ✅ Comprehensive documentation

## Example

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 0;
}
```

## Links

- GitHub: https://github.com/cosmah/Project-Lwanga
- Documentation: https://github.com/cosmah/Project-Lwanga/tree/main/docs
- Examples: https://github.com/cosmah/Project-Lwanga/tree/main/examples

## Credits

Developed by Cosmc Technologies
Email: cosmahke4@gmail.com

## License

MIT License
```

## Rollback Plan

If critical issues are found:

1. Remove packages from repositories
2. Mark Docker images as deprecated
3. Unpublish VS Code extension
4. Create hotfix branch
5. Release v1.0.1 with fixes

## Support Channels

- GitHub Issues: Bug reports and feature requests
- Email: cosmahke4@gmail.com
- Documentation: In-repo docs and examples

---

**Cosmc Technologies** | https://github.com/cosmah/Project-Lwanga
