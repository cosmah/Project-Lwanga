# Contributing to Lwanga

Thank you for your interest in contributing to Lwanga! This document provides guidelines and instructions for contributing to the project.

## Table of Contents

1. [Code of Conduct](#code-of-conduct)
2. [Getting Started](#getting-started)
3. [Development Setup](#development-setup)
4. [How to Contribute](#how-to-contribute)
5. [Coding Standards](#coding-standards)
6. [Testing](#testing)
7. [Pull Request Process](#pull-request-process)
8. [Community](#community)

---

## Code of Conduct

This project adheres to a Code of Conduct that all contributors are expected to follow. Please read [CODE_OF_CONDUCT.md](CODE_OF_CONDUCT.md) before contributing.

---

## Getting Started

### Prerequisites

- C++17 or later compiler (GCC 9+, Clang 10+, MSVC 2019+)
- CMake 3.15 or later
- LLVM 18 development libraries
- Git

### Fork and Clone

```bash
# Fork the repository on GitHub
# Then clone your fork
git clone https://github.com/YOUR_USERNAME/Project-Lwanga.git
cd Project-Lwanga

# Add upstream remote
git remote add upstream https://github.com/cosmah/Project-Lwanga.git
```

---

## Development Setup

### Build from Source

```bash
# Build compiler
cd compiler
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

### Install Development Tools

```bash
# Ubuntu/Debian
sudo apt install llvm-18-dev clang-18 cmake build-essential

# macOS
brew install llvm@18 cmake

# Arch Linux
sudo pacman -S llvm cmake base-devel
```

---

## How to Contribute

### Reporting Bugs

Before creating a bug report:
1. Check existing issues to avoid duplicates
2. Verify the bug exists in the latest version
3. Collect relevant information (OS, version, error messages)

Create a bug report with:
- Clear, descriptive title
- Steps to reproduce
- Expected vs actual behavior
- Code samples (if applicable)
- System information

**Example**:
```markdown
**Bug**: Compiler crashes on nested struct definitions

**Steps to Reproduce**:
1. Create file with nested struct
2. Run: lwangac test.lwanga -o test
3. Compiler crashes with segfault

**Expected**: Should compile successfully
**Actual**: Segmentation fault

**Environment**:
- OS: Ubuntu 24.04
- Lwanga version: 1.0.2
- LLVM version: 18.1.0
```

### Suggesting Features

Feature requests should include:
- Clear description of the feature
- Use cases and benefits
- Potential implementation approach
- Examples of similar features in other languages

### Contributing Code

1. **Find an Issue**: Look for issues labeled `good first issue` or `help wanted`
2. **Discuss**: Comment on the issue to discuss your approach
3. **Implement**: Write your code following our standards
4. **Test**: Add tests for your changes
5. **Submit**: Create a pull request

---

## Coding Standards

### C++ Code Style

```cpp
// Use descriptive names
class TypeChecker {
public:
    // Public methods first
    bool checkTypes(const ASTNode* node);
    
private:
    // Private members with m_ prefix
    SymbolTable* m_symbolTable;
    std::vector<Error> m_errors;
};

// Use const correctness
const std::string& getName() const { return m_name; }

// Prefer smart pointers
std::unique_ptr<Parser> parser = std::make_unique<Parser>();

// Use auto for complex types
auto it = symbolTable.find(name);
```

### Lwanga Code Style

```lwanga
// Use 4-space indentation
fn calculate(x: u64, y: u64) -> u64 {
    let result: u64 = x + y;
    return result;
}

// Space around operators
let x: u64 = a + b * c;

// Descriptive variable names
let buffer_size: u64 = 1024;
let file_descriptor: u64 = 3;

// Comments for complex logic
// Calculate checksum using XOR
let mut checksum: u64 = 0;
while i < len {
    checksum = checksum ^ data[i];
    i = i + 1;
}
```

### Commit Messages

Follow conventional commits:

```
type(scope): subject

body

footer
```

**Types**:
- `feat`: New feature
- `fix`: Bug fix
- `docs`: Documentation
- `style`: Formatting
- `refactor`: Code restructuring
- `test`: Adding tests
- `chore`: Maintenance

**Examples**:
```
feat(parser): add support for array literals

Implements parsing of array literal expressions like [1, 2, 3].
Adds ArrayLiteralExpr AST node and updates IR generation.

Closes #42
```

```
fix(lexer): handle escaped quotes in strings

Previously, strings with escaped quotes would cause lexer errors.
Now properly handles \" and \' escape sequences.

Fixes #38
```

---

## Testing

### Running Tests

```bash
# Run all tests
cd compiler/build
ctest --output-on-failure

# Run specific test
./tests/lexer_test
./tests/parser_test

# Run with verbose output
ctest -V
```

### Writing Tests

Add tests for all new features:

```cpp
// compiler/tests/my_feature_test.cpp
#include <gtest/gtest.h>
#include "Parser.h"

TEST(MyFeatureTest, BasicFunctionality) {
    Parser parser("test input");
    auto result = parser.parse();
    
    ASSERT_NE(result, nullptr);
    EXPECT_EQ(result->getType(), NodeType::Function);
}

TEST(MyFeatureTest, ErrorHandling) {
    Parser parser("invalid input");
    auto result = parser.parse();
    
    EXPECT_EQ(result, nullptr);
    EXPECT_TRUE(parser.hasErrors());
}
```

### Test Coverage

- Aim for 80%+ code coverage
- Test both success and failure cases
- Include edge cases
- Add integration tests for major features

---

## Pull Request Process

### Before Submitting

1. **Update from upstream**:
```bash
git fetch upstream
git rebase upstream/main
```

2. **Run tests**:
```bash
cd compiler/build
ctest --output-on-failure
```

3. **Format code**:
```bash
# Format C++ code
clang-format -i src/**/*.cpp include/**/*.h

# Format Lwanga code
lwangafmt --write examples/**/*.lwanga
```

4. **Update documentation**:
- Add/update relevant docs
- Update CHANGELOG.md
- Add examples if applicable

### Creating Pull Request

1. **Push to your fork**:
```bash
git push origin feature-branch
```

2. **Create PR on GitHub**:
- Use descriptive title
- Reference related issues
- Describe changes in detail
- Add screenshots/examples if applicable

3. **PR Template**:
```markdown
## Description
Brief description of changes

## Type of Change
- [ ] Bug fix
- [ ] New feature
- [ ] Breaking change
- [ ] Documentation update

## Testing
- [ ] All tests pass
- [ ] Added new tests
- [ ] Manual testing performed

## Checklist
- [ ] Code follows style guidelines
- [ ] Self-review completed
- [ ] Documentation updated
- [ ] No new warnings
- [ ] CHANGELOG.md updated

## Related Issues
Closes #123
```

### Review Process

1. Maintainers will review your PR
2. Address feedback and requested changes
3. Once approved, PR will be merged
4. Your contribution will be credited

---

## Community

### Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Questions and general discussion
- **Email**: cosmahke4@gmail.com

### Getting Help

- Check [documentation](docs/)
- Search existing issues
- Ask in GitHub Discussions
- Email maintainers

### Recognition

Contributors are recognized in:
- CHANGELOG.md
- GitHub contributors page
- Release notes

---

## Development Guidelines

### Architecture

```
compiler/
├── include/          # Public headers
│   ├── Lexer.h
│   ├── Parser.h
│   └── ...
├── src/
│   ├── lexer/       # Lexical analysis
│   ├── parser/      # Syntax analysis
│   ├── types/       # Type checking
│   ├── codegen/     # IR generation
│   ├── backend/     # Code generation
│   └── driver/      # Compiler driver
└── tests/           # Test suite
```

### Adding New Features

1. **Design**: Discuss design in issue
2. **Implement**: Start with minimal implementation
3. **Test**: Add comprehensive tests
4. **Document**: Update relevant documentation
5. **Examples**: Add usage examples

### Modifying Existing Features

1. **Understand**: Read existing code and tests
2. **Preserve**: Maintain backward compatibility
3. **Test**: Ensure existing tests still pass
4. **Document**: Update affected documentation

---

## License

By contributing to Lwanga, you agree that your contributions will be licensed under the MIT License.

---

## Questions?

If you have questions about contributing, please:
- Open a GitHub Discussion
- Email cosmahke4@gmail.com
- Check existing documentation

Thank you for contributing to Lwanga!

---

**Back to**: [Main README](README.md)
