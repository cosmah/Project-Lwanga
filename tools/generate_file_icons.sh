#!/bin/bash
# Generate file type icons from the main Lwanga logo
# This script creates icons at various sizes for use in IDEs and file managers

set -e

SOURCE="lwanga.png"
OUTPUT_DIR="assets/icons"

# Check if source logo exists
if [ ! -f "$SOURCE" ]; then
    echo "Error: Source logo '$SOURCE' not found!"
    exit 1
fi

# Create output directory
mkdir -p "$OUTPUT_DIR"

echo "Generating file icons from $SOURCE..."

# Generate PNG icons at different sizes
for size in 16 32 64 128 256; do
    echo "  Creating ${size}x${size} icon..."
    convert "$SOURCE" -resize ${size}x${size} "$OUTPUT_DIR/lwanga-file-${size}.png"
done

# Generate favicon (16x16 and 32x32 in one .ico file)
echo "  Creating favicon.ico..."
convert "$SOURCE" -resize 16x16 "$OUTPUT_DIR/favicon-16.png"
convert "$SOURCE" -resize 32x32 "$OUTPUT_DIR/favicon-32.png"
convert "$OUTPUT_DIR/favicon-16.png" "$OUTPUT_DIR/favicon-32.png" "$OUTPUT_DIR/favicon.ico"
rm "$OUTPUT_DIR/favicon-16.png" "$OUTPUT_DIR/favicon-32.png"

# Create SVG wrappers that reference the PNG files
echo "  Creating SVG wrappers..."

cat > "$OUTPUT_DIR/lwanga-file.svg" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<svg width="32" height="32" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <image href="lwanga-file-32.png" x="0" y="0" width="32" height="32"/>
</svg>
EOF

cat > "$OUTPUT_DIR/lwanga-file-light.svg" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<svg width="32" height="32" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <image href="lwanga-file-32.png" x="0" y="0" width="32" height="32"/>
</svg>
EOF

cat > "$OUTPUT_DIR/lwanga-file-dark.svg" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<svg width="32" height="32" viewBox="0 0 32 32" xmlns="http://www.w3.org/2000/svg" xmlns:xlink="http://www.w3.org/1999/xlink">
  <image href="lwanga-file-32.png" x="0" y="0" width="32" height="32"/>
</svg>
EOF

# Create header file variant (with "H" badge)
echo "  Creating header file icon variant..."
convert "$OUTPUT_DIR/lwanga-file-32.png" \
    -gravity SouthEast \
    -pointsize 14 -font Arial-Bold -fill "#4CAF50" \
    -annotate +2+2 "H" \
    "$OUTPUT_DIR/lwanga-header-32.png"

# Create test file variant (with "T" badge)
echo "  Creating test file icon variant..."
convert "$OUTPUT_DIR/lwanga-file-32.png" \
    -gravity SouthEast \
    -pointsize 14 -font Arial-Bold -fill "#FF9800" \
    -annotate +2+2 "T" \
    "$OUTPUT_DIR/lwanga-test-32.png"

echo ""
echo "✓ File icons generated successfully in $OUTPUT_DIR/"
echo ""
echo "Generated files:"
ls -lh "$OUTPUT_DIR"/*.png "$OUTPUT_DIR"/*.svg "$OUTPUT_DIR"/*.ico 2>/dev/null || true
echo ""
echo "Next steps:"
echo "  1. Review the icons: open $OUTPUT_DIR/"
echo "  2. Use in VS Code extension: copy to extensions/vscode/icons/"
echo "  3. Install system icons: sudo cp $OUTPUT_DIR/*.png /usr/share/icons/hicolor/*/mimetypes/"
