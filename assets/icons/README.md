# Lwanga File Icons

This directory contains file type icons for Lwanga source files, used in IDEs and file managers.

## Icon Files

### PNG Icons (From Actual Logo)
Generated from the main `lwanga.png` logo at various sizes:
- `lwanga-file-16.png` - 16x16px (standard, for IDE tabs)
- `lwanga-file-32.png` - 32x32px (retina displays)
- `lwanga-file-64.png` - 64x64px (high-DPI)
- `lwanga-file-128.png` - 128x128px (extra high-DPI)
- `lwanga-file-256.png` - 256x256px (preview/thumbnails)
- `favicon.ico` - Multi-size favicon for web

### SVG Icons (Wrappers)
SVG files that reference the PNG icons for better IDE compatibility:
- `lwanga-file.svg` - Default file icon (references PNG)
- `lwanga-file-light.svg` - Light theme variant
- `lwanga-file-dark.svg` - Dark theme variant

### Icon Variants
- `lwanga-header-32.png` - Header files (`.lwangah`) with "H" badge
- `lwanga-test-32.png` - Test files (`.lwangatest`) with "T" badge

## Usage

### VS Code Extension
The SVG icons are used in the VS Code extension to display next to `.lwanga` files in:
- File explorer tree
- Editor tabs
- Quick open dialog
- Search results

See `extensions/vscode/` for the extension implementation.

### System File Managers
PNG icons are installed to system directories for file manager integration:

**Linux**:
```bash
sudo cp lwanga-file-16.png /usr/share/icons/hicolor/16x16/mimetypes/text-x-lwanga.png
sudo cp lwanga-file-32.png /usr/share/icons/hicolor/32x32/mimetypes/text-x-lwanga.png
sudo cp lwanga-file-64.png /usr/share/icons/hicolor/64x64/mimetypes/text-x-lwanga.png
sudo gtk-update-icon-cache /usr/share/icons/hicolor/
```

**macOS**:
Icons are bundled with the application and registered via Info.plist.

**Windows**:
Icons are embedded in the installer and registered via registry.

## Design Guidelines

### Using the Actual Logo
The file icons are generated directly from `lwanga.png` (1024x1024) and scaled down to appropriate sizes. This ensures:
- **Brand consistency** - Same logo everywhere
- **Professional appearance** - High-quality original artwork
- **Recognition** - Users see the same logo in files, docs, and website

### Colors
Colors are inherited from the original logo design in `lwanga.png`.

### Size Requirements
- Must be legible at 16x16px (smallest size for IDE tabs)
- Should work on both light and dark backgrounds
- Original logo is scaled proportionally
- Transparency is preserved from original PNG

### Theme Variants
- **Light theme**: Lighter background, darker text
- **Dark theme**: Darker background, lighter text
- **Neutral**: Works on any background

## Generating Icons

### Automatic Generation (Recommended)
Use the provided script to generate all icons from the main logo:
```bash
# Run from project root
./tools/generate_file_icons.sh
```

This will:
- Generate PNG icons at sizes: 16, 32, 64, 128, 256
- Create favicon.ico
- Create SVG wrappers
- Create variant icons (header, test files)

### Manual Generation
If you need custom sizes:
```bash
# Generate specific size
convert lwanga.png -resize 48x48 assets/icons/lwanga-file-48.png

# Generate with transparency preserved
convert lwanga.png -resize 32x32 -background none assets/icons/lwanga-file-32.png
```

### From Main Logo
The icons are always generated from `lwanga.png` in the project root:
```bash
# Update icons after logo change
./tools/generate_file_icons.sh
```

## Testing

### VS Code
1. Install the extension: `code --install-extension ../../extensions/vscode/lwanga-1.0.0.vsix`
2. Open a `.lwanga` file
3. Check that the icon appears in the tab and file explorer

### System File Manager
1. Install icons to system directories (see above)
2. Open file manager
3. Navigate to a directory with `.lwanga` files
4. Verify icons appear

## Icon Variants

### File Types
- `.lwanga` - Main source files (blue "L")
- `.lwangah` - Header files (blue "L" with green "H" badge)
- `.lwangamod` - Module files (blue "L" with purple "M" badge)
- `.lwangatest` - Test files (blue "L" with orange "T" badge)

### Creating Variants
To create a variant with a badge:
```bash
# Add a badge to the icon
convert lwanga-file-32.png \
    -pointsize 12 -fill "#4CAF50" \
    -annotate +24+24 "H" \
    lwanga-header-32.png
```

## License

These icons are part of the Lwanga project and are licensed under the same terms as the main project (MIT License).
