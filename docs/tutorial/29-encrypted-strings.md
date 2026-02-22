# Lesson 29: Encrypted Strings

**In this lesson, you'll learn how to use compile-time string encryption to hide sensitive data from binary analysis and reverse engineering.**

---

## What are Encrypted Strings?

Encrypted strings are strings that are encrypted at compile time and automatically decrypted at runtime. They never appear in plaintext in the binary.

**Why use encrypted strings?**
- Hide API keys and credentials
- Prevent string searches in binaries
- Obfuscate malware signatures
- Protect intellectual property
- Make reverse engineering harder

**How it works:**
1. Compiler encrypts string at compile time
2. Encrypted bytes stored in binary
3. Runtime decrypts when accessed
4. Plaintext never in binary

---

## Basic Usage

```lwanga
fn main() -> u64 {
    let password: ptr = enc "my_secret_password";
    let api_key: ptr = enc "sk-1234567890abcdef";
    
    // Strings are automatically decrypted when used
    unsafe {
        syscall(1, 1, password, 18);
    }
    
    return 0;
}
```

**Syntax:**
```lwanga
let variable: ptr = enc "string to encrypt";
```

The `enc` keyword tells the compiler to encrypt this string.

---

## Encryption Algorithm

Lwanga uses XOR encryption with a random key generated at compile time:

```
For each byte in string:
    encrypted_byte = original_byte XOR key_byte
```

**Properties:**
- Different key for each string
- Keys generated randomly per compilation
- Fast decryption (single XOR)
- No external dependencies

---

## Practical Examples

### Example 1: Hidden Credentials

```lwanga
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let username: ptr = enc "admin";
    let password: ptr = enc "super_secret_123";
    
    // Use credentials
    let msg: ptr = "Logging in...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 14);
    }
    
    // Credentials are encrypted in binary
    return 0;
}
```

**Binary analysis:**
```bash
strings program | grep admin
# Nothing found!

strings program | grep super_secret
# Nothing found!
```

### Example 2: API Keys

```lwanga
fn call_api() -> u64 {
    let api_key: ptr = enc "sk-proj-1234567890abcdef";
    let api_url: ptr = enc "https://api.example.com";
    
    // Make API call with encrypted credentials
    return 0;
}

fn main() -> u64 {
    return call_api();
}
```

### Example 3: License Keys

```lwanga
fn check_license() -> u64 {
    let valid_key: ptr = enc "LWANGA-PRO-2024-ABCD1234";
    let user_key: ptr = get_user_input();
    
    // Compare keys
    if compare_strings(valid_key, user_key) == 1 {
        return 1;  // Valid
    }
    
    return 0;  // Invalid
}

fn get_user_input() -> ptr {
    // Read user input
    return 0 as ptr;
}

fn compare_strings(s1: ptr, s2: ptr) -> u64 {
    // String comparison
    return 0;
}

fn main() -> u64 {
    return check_license();
}
```

### Example 4: Hidden Messages

```lwanga
fn print_hidden_message() -> u64 {
    let secret: ptr = enc "This message is hidden from analysis!";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, secret, 38);
    }
    
    return 0;
}

fn main() -> u64 {
    return print_hidden_message();
}
```

### Example 5: Configuration Data

```lwanga
fn get_config() -> u64 {
    let db_host: ptr = enc "database.internal.company.com";
    let db_user: ptr = enc "app_user";
    let db_pass: ptr = enc "complex_password_123!@#";
    
    // Use configuration
    return 0;
}

fn main() -> u64 {
    return get_config();
}
```

---

## Comparison: Plain vs Encrypted

### Plain Strings (Visible in Binary)

```lwanga
fn main() -> u64 {
    let password: ptr = "password123";
    return 0;
}
```

**Binary analysis:**
```bash
strings program
# Output: password123
```

### Encrypted Strings (Hidden in Binary)

```lwanga
fn main() -> u64 {
    let password: ptr = enc "password123";
    return 0;
}
```

**Binary analysis:**
```bash
strings program
# Output: (random bytes, no "password123")
```

---

## When to Use Encrypted Strings

### Good Use Cases

✅ **API Keys and Tokens**
```lwanga
let api_key: ptr = enc "sk-1234567890";
```

✅ **Passwords and Credentials**
```lwanga
let admin_pass: ptr = enc "admin_password";
```

✅ **License Keys**
```lwanga
let license: ptr = enc "PRODUCT-KEY-12345";
```

✅ **Internal URLs**
```lwanga
let internal_url: ptr = enc "https://internal.company.com";
```

✅ **Sensitive Configuration**
```lwanga
let secret_config: ptr = enc "production_secret";
```

### When NOT to Use

❌ **User-Visible Strings**
```lwanga
// Don't encrypt UI text
let greeting: ptr = "Hello, User!";  // Plain is fine
```

❌ **Debug Messages**
```lwanga
// Don't encrypt debug output
let debug: ptr = "Debug: value = ";  // Plain is fine
```

❌ **Public Constants**
```lwanga
// Don't encrypt public data
let version: ptr = "1.0.0";  // Plain is fine
```

---

## Security Considerations

### Encryption is Not Perfect

**What it protects against:**
- ✅ Simple string searches (`strings` command)
- ✅ Casual binary analysis
- ✅ Automated signature scanning
- ✅ Quick reverse engineering

**What it doesn't protect against:**
- ❌ Determined reverse engineers
- ❌ Dynamic analysis (debuggers)
- ❌ Memory dumps at runtime
- ❌ Advanced static analysis

### Defense in Depth

Combine encrypted strings with other techniques:

```lwanga
fn secure_function() -> u64 {
    // 1. Encrypted strings
    let key: ptr = enc "secret_key";
    
    // 2. Obfuscated code (compile with --obfuscate)
    let x: u64 = compute_obfuscated();
    
    // 3. Anti-debugging checks
    if is_debugger_present() == 1 {
        return 0;
    }
    
    // 4. Use the key
    return 0;
}

fn compute_obfuscated() -> u64 {
    return 42;
}

fn is_debugger_present() -> u64 {
    return 0;
}

fn main() -> u64 {
    return secure_function();
}
```

---

## Implementation Details

### How Decryption Works

At runtime, the compiler generates decryption code:

```lwanga
// Your code:
let msg: ptr = enc "Hello";

// Compiler generates (conceptually):
let encrypted: [u8; 5] = [0x42, 0x7F, 0x3A, 0x3A, 0x5D];
let key: [u8; 5] = [0x2A, 0x37, 0x52, 0x54, 0x39];
let decrypted: [u8; 5] = [0, 0, 0, 0, 0];

// Decrypt on first access
for i in 0..5 {
    decrypted[i] = encrypted[i] ^ key[i];
}

let msg: ptr = &decrypted;
```

### Performance Impact

- **Compile time:** Slightly slower (encryption)
- **Binary size:** Slightly larger (decryption code)
- **Runtime:** Minimal (one-time XOR per string)
- **Memory:** Same as plain strings

---

## Advanced Techniques

### Lazy Decryption

Strings are decrypted only when first accessed:

```lwanga
fn main() -> u64 {
    let key1: ptr = enc "key1";  // Not decrypted yet
    let key2: ptr = enc "key2";  // Not decrypted yet
    
    // Only key1 is decrypted here
    use_key(key1);
    
    // key2 never decrypted if not used
    return 0;
}

fn use_key(key: ptr) -> u64 {
    return 0;
}
```

### Multiple Encryption Layers

```lwanga
fn double_encrypted() -> u64 {
    // Encrypted at compile time
    let layer1: ptr = enc "secret";
    
    // Additional runtime encryption
    let mut buffer: [u8; 6] = [0, 0, 0, 0, 0, 0];
    
    unsafe {
        let mut i: u64 = 0;
        while i < 6 {
            let ch: u8 = *((layer1 as u64 + i) as ptr);
            buffer[i] = ch ^ 0x42;  // Second layer
            i = i + 1;
        }
    }
    
    return 0;
}

fn main() -> u64 {
    return double_encrypted();
}
```

---

## Common Mistakes

### Mistake 1: Encrypting Everything

```lwanga
// WRONG: Encrypting non-sensitive strings
let hello: ptr = enc "Hello, World!";  // Unnecessary
let version: ptr = enc "1.0.0";        // Unnecessary
```

**Fix:** Only encrypt sensitive data:
```lwanga
let hello: ptr = "Hello, World!";      // Plain is fine
let api_key: ptr = enc "secret_key";   // Encrypt this
```

### Mistake 2: Logging Encrypted Strings

```lwanga
// WRONG: Logging defeats encryption
let password: ptr = enc "secret";

unsafe {
    syscall(1, 1, password, 6);  // Now visible in logs!
}
```

**Fix:** Don't log sensitive data:
```lwanga
let password: ptr = enc "secret";
// Use password internally, don't log it
```

### Mistake 3: Storing Decrypted Strings

```lwanga
// WRONG: Storing decrypted value
let encrypted: ptr = enc "secret";
let decrypted: ptr = encrypted;  // Now in memory as plaintext
```

**Fix:** Use encrypted strings directly:
```lwanga
let encrypted: ptr = enc "secret";
// Use encrypted directly, don't copy
```

---

## Exercises

### Exercise 1: Encrypt Configuration

Encrypt database configuration strings.

<details>
<summary>Solution</summary>

```lwanga
fn get_db_config() -> u64 {
    let host: ptr = enc "db.internal.company.com";
    let port: ptr = enc "5432";
    let database: ptr = enc "production_db";
    let username: ptr = enc "app_user";
    let password: ptr = enc "complex_pass_123!";
    
    // Use configuration
    return 0;
}

fn main() -> u64 {
    return get_db_config();
}
```
</details>

### Exercise 2: Hidden Command

Execute a command with encrypted path.

<details>
<summary>Solution</summary>

```lwanga
const SYS_EXECVE: u64 = 59;

fn execute_hidden_command() -> u64 {
    let command: ptr = enc "/bin/ls";
    let args: [ptr; 2] = [command, 0 as ptr];
    let env: [ptr; 1] = [0 as ptr];
    
    unsafe {
        return syscall(SYS_EXECVE, command, &args, &env);
    }
}

fn main() -> u64 {
    return execute_hidden_command();
}
```
</details>

### Exercise 3: License Validator

Check license key against encrypted valid key.

<details>
<summary>Solution</summary>

```lwanga
fn validate_license(user_key: ptr) -> u64 {
    let valid_key: ptr = enc "LWANGA-2024-ABCD1234";
    
    // Compare strings
    let mut i: u64 = 0;
    unsafe {
        while i < 20 {
            let valid_ch: u8 = *((valid_key as u64 + i) as ptr);
            let user_ch: u8 = *((user_key as u64 + i) as ptr);
            
            if valid_ch != user_ch {
                return 0;  // Invalid
            }
            
            if valid_ch == 0 {
                break;
            }
            
            i = i + 1;
        }
    }
    
    return 1;  // Valid
}

fn main() -> u64 {
    let key: ptr = "LWANGA-2024-ABCD1234";
    return validate_license(key);
}
```
</details>

### Exercise 4: Encrypted URL Fetcher

Fetch data from encrypted URL.

<details>
<summary>Solution</summary>

```lwanga
fn fetch_from_hidden_url() -> u64 {
    let url: ptr = enc "https://api.internal.company.com/data";
    let api_key: ptr = enc "Bearer sk-1234567890";
    
    // Make HTTP request with encrypted credentials
    // (Implementation would use network syscalls)
    
    return 0;
}

fn main() -> u64 {
    return fetch_from_hidden_url();
}
```
</details>

### Exercise 5: Multi-String Encryption

Encrypt multiple related strings.

<details>
<summary>Solution</summary>

```lwanga
fn encrypted_config() -> u64 {
    let service_name: ptr = enc "payment-processor";
    let api_endpoint: ptr = enc "https://pay.internal.com/api/v1";
    let api_key: ptr = enc "pk_live_1234567890";
    let webhook_secret: ptr = enc "whsec_abcdef123456";
    let encryption_key: ptr = enc "aes256_key_here";
    
    // All strings encrypted in binary
    return 0;
}

fn main() -> u64 {
    return encrypted_config();
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What encrypted strings are and why they're useful
- ✅ How to use the `enc` keyword
- ✅ When to encrypt strings (credentials, keys, sensitive data)
- ✅ When NOT to encrypt (UI text, debug messages)
- ✅ Security considerations and limitations
- ✅ Performance and implementation details
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand encrypted strings, let's learn about shellcode development in [Lesson 30: Shellcode Basics](30-shellcode-basics.md).

---

**Navigation**: [← Previous: Registers](28-registers.md) | [Tutorial Home](README.md) | [Next: Shellcode Basics →](30-shellcode-basics.md)
