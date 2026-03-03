# Lwanga Docker Image

Official Docker image for the Lwanga programming language compiler.

## Quick Start

### Pull from Docker Hub (when published)
```bash
docker pull cosmctechnologies/lwanga:latest
```

### Build Locally
```bash
# From project root
docker build -t lwanga:latest -f packaging/docker/Dockerfile .
```

### Run Container
```bash
# Interactive shell
docker run -it --rm lwanga:latest

# Compile a file
docker run -it --rm -v $(pwd):/workspace lwanga:latest lwangac hello.lwanga -o hello

# With docker-compose
cd packaging/docker
docker-compose up -d
docker-compose exec lwanga bash
```

## Usage Examples

### Compile Hello World
```bash
docker run -it --rm -v $(pwd):/workspace lwanga:latest bash -c "
echo 'fn main() -> u64 { return 0; }' > hello.lwanga
lwangac hello.lwanga -o hello
./hello
"
```

### Format Code
```bash
docker run -it --rm -v $(pwd):/workspace lwanga:latest \
    lwangafmt --write myfile.lwanga
```

### Syntax Highlighting
```bash
docker run -it --rm -v $(pwd):/workspace lwanga:latest \
    lwangacat -n myfile.lwanga
```

## Image Details

- **Base**: Ubuntu 24.04
- **Size**: ~500MB
- **Includes**:
  - lwangac (compiler)
  - lwangafmt (formatter)
  - lwangacat (syntax highlighter)
  - LLVM 18
  - All dependencies

## Environment Variables

- `PATH`: Includes `/usr/local/bin` for Lwanga binaries

## Volumes

Mount your workspace to `/workspace`:
```bash
docker run -it --rm -v /path/to/your/code:/workspace lwanga:latest
```

## Building for Multiple Architectures

```bash
# Build for AMD64 and ARM64
docker buildx build --platform linux/amd64,linux/arm64 \
    -t cosmctechnologies/lwanga:latest \
    -f packaging/docker/Dockerfile .
```

## Publishing

```bash
# Tag
docker tag lwanga:latest cosmctechnologies/lwanga:1.0.3
docker tag lwanga:latest cosmctechnologies/lwanga:latest

# Push
docker push cosmctechnologies/lwanga:1.0.3
docker push cosmctechnologies/lwanga:latest
```

## License

MIT License - See LICENSE file for details

## Support

- GitHub: https://github.com/cosmah/Project-Lwanga
- Issues: https://github.com/cosmah/Project-Lwanga/issues
- Email: cosmahke4@gmail.com
