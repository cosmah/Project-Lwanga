# Lesson 29: Encrypted Strings

**Learn how to use compile-time string encryption.**

---

## What are Encrypted Strings?

Encrypted strings are encrypted at compile time and decrypted at runtime, so they don't appear in plaintext in the binary.

```lwanga
let secret: ptr = enc "password123";
```

---

## Basic Usage

```lwanga
fn main() -> u64 {
    let password: ptr = enc "my_secret_password";
    let api_key: ptr = enc "sk-1234567890abcdef";
    
    // Strings are automatically decrypted when used
    return 0;
}
```

---

## Why Use Encrypted Strings?

- Hide sensitive data from binary analysis
- Prevent simple string searches
- Obfuscate configuration
- Protect credentials

---

## Example: Hidden Message

```lwanga
fn main() -> u64 {
    let msg: ptr = enc "This is hidden!\n";
    
    unsafe {
        syscall(1, 1, msg, 16);
    }
    
    return 0;
}
```

---

## Comparison

```lwanga
// Plaintext - visible in binary
let plain: ptr = "password";

// Encrypted - not visible in binary
let encrypted: ptr = enc "password";
```

---

## When to Use

- API keys and tokens
- Passwords and credentials
- License keys
- Sensitive configuration
- Hidden messages

---

**Navigation**: [← Previous: Registers](28-registers.md) | [Tutorial Home](README.md) | [Next: Shellcode Basics →](30-shellcode-basics.md)
