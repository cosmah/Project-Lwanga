# Lwanga v1.0.3 Release Steps

## 1. Build the Debian Package

```bash
./packaging/build-debian.sh
```

This will create: `lwanga_1.0.3-1_amd64.deb` (or `arm64` depending on your architecture)

## 2. Generate Checksum

```bash
sha256sum lwanga_1.0.3-1_*.deb > lwanga_1.0.3-1_$(dpkg --print-architecture).deb.sha256
```

## 3. Create GitHub Release

1. Go to: https://github.com/cosmah/Project-Lwanga/releases/new

2. Fill in:
   - **Tag**: `v1.0.3`
   - **Release title**: `Lwanga v1.0.3 - Hardening & Preprocessor Update`
   - **Description**: 
   ````markdown
   # Lwanga v1.0.3 - Hardening & Preprocessor Update
   
   Point release with compiler hardening and preprocessor improvements.
   
   ## Installation
   
   ### Debian/Ubuntu (x64/ARM64)
   ```bash
   ARCH="$(dpkg --print-architecture)"
   wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.3/lwanga_1.0.3-1_${ARCH}.deb
   sudo dpkg -i lwanga_1.0.3-1_${ARCH}.deb
   sudo apt-get install -f
   ```
   
   ### Verify Installation
   ```bash
   lwangac --version
   ```
   
   ## What's Included
   - lwangac: Compiler
   - lwangafmt: Code formatter
   - lwangacat: Syntax highlighter
   - Documentation and examples
   
   ## Changelog
   See [CHANGELOG.md](https://github.com/cosmah/Project-Lwanga/blob/main/CHANGELOG.md)
   ````

3. **Attach files**:
   - Drag and drop `lwanga_1.0.3-1_<arch>.deb`
   - Drag and drop `lwanga_1.0.3-1_<arch>.deb.sha256`

4. Click **Publish release**

## 4. Update README.md

Update the download link in README.md to point to the actual release:

````markdown
### Ubuntu/Debian
```bash
ARCH="$(dpkg --print-architecture)"
wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.3/lwanga_1.0.3-1_${ARCH}.deb
sudo dpkg -i lwanga_1.0.3-1_${ARCH}.deb
sudo apt-get install -f
```
````

## 5. Test the Installation

After publishing, test the download:

```bash
ARCH="$(dpkg --print-architecture)"
# Download
wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.3/lwanga_1.0.3-1_${ARCH}.deb
wget https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.3/lwanga_1.0.3-1_${ARCH}.deb.sha256

# Verify checksum
sha256sum -c lwanga_1.0.3-1_${ARCH}.deb.sha256

# Install
sudo dpkg -i lwanga_1.0.3-1_${ARCH}.deb
sudo apt-get install -f

# Test
lwangac --version
lwangac examples/hello_world.lwanga -o hello
./hello
```

## Done!

Your package is now available for download at:
`https://github.com/cosmah/Project-Lwanga/releases/download/v1.0.3/lwanga_1.0.3-1_<arch>.deb`
