# Lesson 31: Code Obfuscation

**Learn techniques to make code harder to analyze.**

---

## Compiler Obfuscation

Use the `--obfuscate` flag:

```bash
lwangac program.lwanga -o program --obfuscate
```

---

## What Gets Obfuscated?

- Control flow flattening
- Constant encryption
- Dead code insertion
- String obfuscation

---

## Manual Obfuscation Techniques

### Opaque Predicates

```lwanga
fn obfuscated_add(a: u64, b: u64) -> u64 {
    let always_true: u64 = (a * a) >= 0;  // Always true
    
    if always_true != 0 {
        return a + b;
    } else {
        return 0;  // Never executed
    }
}
```

### Constant Obfuscation

```lwanga
fn get_value() -> u64 {
    let x: u64 = 0x1234;
    let y: u64 = 0x5678;
    let result: u64 = x ^ y ^ 0x45AC;  // Obfuscated 42
    return result;
}
```

---

**Navigation**: [← Previous: Shellcode Basics](30-shellcode-basics.md) | [Tutorial Home](README.md) | [Next: Exploit Development →](32-exploit-development.md)
