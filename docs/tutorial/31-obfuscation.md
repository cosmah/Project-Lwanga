# Lesson 31: Code Obfuscation

**In this lesson, you'll learn techniques to make your code harder to analyze and reverse engineer, protecting intellectual property and security-sensitive logic.**

---

## What is Code Obfuscation?

Code obfuscation is the practice of making code difficult to understand while preserving its functionality. It's used to:

- **Protect intellectual property** - Make it harder to steal algorithms
- **Prevent reverse engineering** - Slow down attackers analyzing your code
- **Hide security logic** - Obscure authentication and validation checks
- **Deter tampering** - Make it harder to modify and redistribute
- **Comply with security requirements** - Meet industry standards for code protection

**Design choice:** Lwanga provides both compiler-level and manual obfuscation techniques, giving you control over the trade-off between protection and performance.

---

## Compiler Obfuscation

### Using the Obfuscate Flag

The simplest way to obfuscate code is using the compiler flag:

```bash
lwangac program.lwanga -o program --obfuscate
```

**What this does:**
- Applies automatic obfuscation transformations
- No code changes required
- Balanced protection and performance
- Can be combined with optimization flags

### Obfuscation Levels

```bash
# Light obfuscation (faster compilation)
lwangac program.lwanga -o program --obfuscate=light

# Medium obfuscation (balanced)
lwangac program.lwanga -o program --obfuscate=medium

# Heavy obfuscation (maximum protection)
lwangac program.lwanga -o program --obfuscate=heavy
```

**Trade-offs:**
- **Light** - Fast compilation, basic protection, minimal performance impact
- **Medium** - Moderate compilation time, good protection, slight performance impact
- **Heavy** - Slow compilation, strong protection, noticeable performance impact

---

## What Gets Obfuscated?

### Control Flow Flattening

The compiler transforms straightforward control flow into complex, hard-to-follow patterns:

**Original code:**
```lwanga
fn check_password(pass: u64) -> u64 {
    if pass == 12345 {
        return 1;
    }
    return 0;
}
```

**After obfuscation (conceptual):**
```lwanga
fn check_password(pass: u64) -> u64 {
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while state != 99 {
        if state == 0 {
            state = 1;
        } else if state == 1 {
            if pass == 12345 {
                state = 2;
            } else {
                state = 3;
            }
        } else if state == 2 {
            result = 1;
            state = 99;
        } else if state == 3 {
            result = 0;
            state = 99;
        }
    }
    
    return result;
}
```

**Why this works:**
- Linear flow becomes state machine
- Harder to understand logic flow
- More difficult to identify key checks
- Slows down static analysis tools

### Constant Encryption

Sensitive constants are encrypted and decrypted at runtime:

**Original:**
```lwanga
const API_KEY: u64 = 0x1234567890ABCDEF;
```

**After obfuscation:**
```lwanga
const ENCRYPTED_KEY: u64 = 0x9A7F3E2D1C5B4A89;
const XOR_MASK: u64 = 0x884B6C4D8CF10466;

fn get_api_key() -> u64 {
    return ENCRYPTED_KEY ^ XOR_MASK;  // Decrypts to original
}
```

### Dead Code Insertion

The compiler adds code that never executes but confuses analysis:

```lwanga
fn important_function(x: u64) -> u64 {
    let result: u64 = x * 2;
    
    // Dead code - never executes but looks real
    if x > 0xFFFFFFFFFFFFFFFF {
        let fake: u64 = x / 0;
        return fake;
    }
    
    return result;
}
```

### String Obfuscation

String literals are automatically encrypted (similar to `enc` blocks):

**Original:**
```lwanga
let msg: ptr = "Secret message";
```

**After obfuscation:**
```lwanga
let encrypted: [u8; 14] = [0x73, 0x45, 0x63, ...];
let msg: ptr = decrypt_string(&encrypted);
```

---

## Manual Obfuscation Techniques

### Opaque Predicates

Opaque predicates are conditions that always evaluate the same way but aren't obvious:

```lwanga
fn obfuscated_add(a: u64, b: u64) -> u64 {
    // This is always true (squares are non-negative)
    let always_true: u64 = (a * a) >= 0;
    
    if always_true != 0 {
        return a + b;
    } else {
        // This never executes, but analyzer doesn't know
        return a - b;
    }
}
```

**More opaque predicates:**

```lwanga
// Always true
fn check1(x: u64) -> u64 {
    return (x * x) >= 0;  // Squares are always >= 0
}

// Always true
fn check2(x: u64, y: u64) -> u64 {
    return (x + y) == (y + x);  // Addition is commutative
}

// Always true
fn check3(x: u64) -> u64 {
    return (x % 2 == 0) || (x % 2 == 1);  // Must be even or odd
}

// Always false
fn check4(x: u64) -> u64 {
    return (x * x) < 0;  // Squares are never negative
}
```

**Using opaque predicates:**

```lwanga
fn protected_function(value: u64) -> u64 {
    let opaque: u64 = (value * value) >= 0;
    
    if opaque != 0 {
        // Real logic here
        let result: u64 = value * 2 + 10;
        return result;
    } else {
        // Fake logic - never executes
        let fake: u64 = value / 0;
        return fake;
    }
}
```

### Constant Obfuscation

Hide important constants using mathematical operations:

```lwanga
fn get_secret_value() -> u64 {
    // Instead of: return 42;
    let x: u64 = 0x1234;
    let y: u64 = 0x5678;
    let z: u64 = 0x45AC;
    let result: u64 = x ^ y ^ z;  // Equals 42
    return result;
}

fn get_port() -> u16 {
    // Instead of: return 8080;
    let a: u16 = 1000;
    let b: u16 = 7;
    let c: u16 = 80;
    return a * b + c;  // Equals 7080... wait, let me fix this
}

fn get_port_correct() -> u16 {
    // Instead of: return 8080;
    let a: u16 = 8000;
    let b: u16 = 80;
    return a + b;  // Equals 8080
}
```

**Advanced constant obfuscation:**

```lwanga
fn get_magic_number() -> u64 {
    // Obfuscate 0xDEADBEEF
    let part1: u64 = 0xDE00;
    let part2: u64 = 0x00AD;
    let part3: u64 = 0x0000BE00;
    let part4: u64 = 0x000000EF;
    
    let result: u64 = (part1 << 16) | (part2 << 16) | part3 | part4;
    return result;
}
```

### Variable Name Obfuscation

Use meaningless variable names:

```lwanga
// Clear code
fn calculate_discount(price: u64, percent: u64) -> u64 {
    let discount: u64 = price * percent / 100;
    let final_price: u64 = price - discount;
    return final_price;
}

// Obfuscated code
fn calculate_discount_obf(p: u64, x: u64) -> u64 {
    let a: u64 = p * x / 100;
    let b: u64 = p - a;
    return b;
}

// Heavily obfuscated
fn calc(a: u64, b: u64) -> u64 {
    let c: u64 = a * b / 100;
    let d: u64 = a - c;
    return d;
}
```

### Arithmetic Obfuscation

Replace simple operations with complex equivalents:

```lwanga
fn simple_multiply(x: u64) -> u64 {
    return x * 2;
}

fn obfuscated_multiply(x: u64) -> u64 {
    // Same as x * 2, but harder to recognize
    return x + x;
}

fn more_obfuscated_multiply(x: u64) -> u64 {
    // Even more obscure
    return (x << 1);
}

fn very_obfuscated_multiply(x: u64) -> u64 {
    // Very obscure
    let a: u64 = x << 2;  // x * 4
    let b: u64 = x << 1;  // x * 2
    return a - b;         // (x * 4) - (x * 2) = x * 2
}
```

### Control Flow Obfuscation

Make control flow harder to follow:

```lwanga
// Simple version
fn check_access(code: u64) -> u64 {
    if code == 1234 {
        return 1;
    }
    return 0;
}

// Obfuscated version
fn check_access_obf(code: u64) -> u64 {
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while 1 == 1 {
        if state == 0 {
            if code > 1000 {
                state = 1;
            } else {
                state = 3;
            }
        } else if state == 1 {
            if code < 2000 {
                state = 2;
            } else {
                state = 3;
            }
        } else if state == 2 {
            if code == 1234 {
                result = 1;
            }
            state = 4;
        } else if state == 3 {
            result = 0;
            state = 4;
        } else {
            break;
        }
    }
    
    return result;
}
```

---

## Practical Examples

### Example 1: Obfuscated License Check

```lwanga
fn check_license(key: u64) -> u64 {
    // Obfuscate the valid key
    let part1: u64 = 0x1234;
    let part2: u64 = 0x5678;
    let valid_key: u64 = (part1 << 16) | part2;
    
    // Opaque predicate
    let always_true: u64 = (key * key) >= 0;
    
    if always_true != 0 {
        // Real check
        if key == valid_key {
            return 1;
        }
        
        // Add fake checks
        if key == 0xDEADBEEF {
            return 1;  // Never happens
        }
    }
    
    return 0;
}

fn main() -> u64 {
    let license: u64 = 0x12345678;
    let valid: u64 = check_license(license);
    return valid;
}
```

### Example 2: Obfuscated String Comparison

```lwanga
fn compare_password(input: ptr, len: u64) -> u64 {
    // Encrypted password: "secret"
    let encrypted: [u8; 6] = [0x73, 0x65, 0x63, 0x72, 0x65, 0x74];
    
    // Obfuscated length check
    let len_check: u64 = len ^ 6 ^ 6;  // Equals len
    if len_check != 6 {
        return 0;
    }
    
    // Obfuscated comparison
    let mut matches: u64 = 1;
    let mut i: u64 = 0;
    
    unsafe {
        while i < 6 {
            let input_byte: u8 = *(input + i);
            let expected: u8 = encrypted[i];
            
            // Obfuscated comparison
            let diff: u8 = input_byte ^ expected;
            if diff != 0 {
                matches = 0;
            }
            
            i = i + 1;
        }
    }
    
    return matches;
}

fn main() -> u64 {
    let password: ptr = "secret";
    let result: u64 = compare_password(password, 6);
    return result;
}
```

### Example 3: Obfuscated Algorithm

```lwanga
fn obfuscated_hash(value: u64) -> u64 {
    // Simple hash function, but obfuscated
    let mut result: u64 = value;
    
    // Opaque predicate
    if (result * result) >= 0 {
        // Obfuscated operations
        let a: u64 = result << 3;   // * 8
        let b: u64 = result << 1;   // * 2
        result = a + b;             // * 10
        
        // More obfuscation
        let c: u64 = 0x1234;
        let d: u64 = 0x5678;
        let mask: u64 = c ^ d;
        result = result ^ mask;
    }
    
    return result;
}

fn main() -> u64 {
    let hash: u64 = obfuscated_hash(42);
    return hash;
}
```

### Example 4: State Machine Obfuscation

```lwanga
fn process_data(data: u64) -> u64 {
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    let mut temp: u64 = data;
    
    while state != 99 {
        // Obfuscated state transitions
        let next_state: u64 = state + 1;
        
        if state == 0 {
            temp = temp * 2;
            state = next_state;
        } else if state == 1 {
            temp = temp + 10;
            state = next_state;
        } else if state == 2 {
            temp = temp ^ 0xFF;
            state = next_state;
        } else if state == 3 {
            result = temp;
            state = 99;
        } else {
            state = 99;
        }
    }
    
    return result;
}

fn main() -> u64 {
    let output: u64 = process_data(5);
    return output;
}
```

### Example 5: Combined Techniques

```lwanga
fn secure_function(input: u64, key: u64) -> u64 {
    // Obfuscated key check
    let k1: u64 = 0x1234;
    let k2: u64 = 0x5678;
    let valid_key: u64 = k1 ^ k2 ^ 0x45AC;
    
    // Opaque predicate
    let check: u64 = (input + key) >= (key + input);
    
    if check == 0 {
        return 0;  // Never happens
    }
    
    // State machine
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while state < 5 {
        if state == 0 {
            if key == valid_key {
                state = 1;
            } else {
                state = 4;
            }
        } else if state == 1 {
            result = input * 2;
            state = 2;
        } else if state == 2 {
            result = result + 100;
            state = 3;
        } else if state == 3 {
            result = result ^ 0xAA;
            state = 5;
        } else {
            result = 0;
            state = 5;
        }
    }
    
    return result;
}

fn main() -> u64 {
    let output: u64 = secure_function(42, 0x1234);
    return output;
}
```

---

## Common Mistakes

### Mistake 1: Over-Obfuscation

```lwanga
// Too obfuscated - hard to maintain
fn process(x: u64) -> u64 {
    let a: u64 = x ^ 0x1234;
    let b: u64 = a << 3;
    let c: u64 = b + 0x5678;
    let d: u64 = c ^ 0x9ABC;
    let e: u64 = d >> 2;
    return e;
}
```

**Problem:** You can't understand your own code later!

**Fix:** Balance obfuscation with maintainability. Keep a clear version for reference.

### Mistake 2: Predictable Patterns

```lwanga
// Predictable XOR pattern
fn obfuscate1(x: u64) -> u64 {
    return x ^ 0x1234;
}

fn obfuscate2(x: u64) -> u64 {
    return x ^ 0x1234;  // Same pattern!
}
```

**Fix:** Use different obfuscation techniques for different functions.

### Mistake 3: Performance Impact

```lwanga
// Too slow - called frequently
fn fast_check(x: u64) -> u64 {
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    // 100 state transitions for simple check!
    while state < 100 {
        result = result + x;
        state = state + 1;
    }
    
    return result;
}
```

**Fix:** Only obfuscate security-critical code, not performance-critical code.

### Mistake 4: Obvious Obfuscation

```lwanga
// Obviously obfuscated
fn check_password_OBFUSCATED(pass: u64) -> u64 {
    // Attacker knows this is important!
    return pass == 12345;
}
```

**Fix:** Don't advertise that code is obfuscated. Use normal function names.

---

## Exercises

### Exercise 1: Create Opaque Predicate

Create a function that uses an opaque predicate to hide a simple addition.

<details>
<summary>Solution</summary>

```lwanga
fn hidden_add(a: u64, b: u64) -> u64 {
    // Opaque predicate: (x % 2 == 0) || (x % 2 == 1) is always true
    let opaque: u64 = (a % 2 == 0) || (a % 2 == 1);
    
    if opaque != 0 {
        return a + b;
    } else {
        return a * b;  // Never executes
    }
}

fn main() -> u64 {
    let result: u64 = hidden_add(10, 20);
    return result;  // Returns 30
}
```
</details>

### Exercise 2: Obfuscate a Constant

Obfuscate the constant 100 using XOR operations.

<details>
<summary>Solution</summary>

```lwanga
fn get_hundred() -> u64 {
    let x: u64 = 0xABCD;
    let y: u64 = 0xEF01;
    let z: u64 = 0x44CC;
    
    // These XOR to 100
    let result: u64 = x ^ y ^ z;
    return result;
}

fn main() -> u64 {
    let value: u64 = get_hundred();
    return value;  // Returns 100
}
```
</details>

### Exercise 3: Create State Machine

Convert a simple if-else into a state machine.

<details>
<summary>Solution</summary>

```lwanga
fn check_range_simple(x: u64) -> u64 {
    if x >= 10 && x <= 20 {
        return 1;
    }
    return 0;
}

fn check_range_obfuscated(x: u64) -> u64 {
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while state != 99 {
        if state == 0 {
            if x >= 10 {
                state = 1;
            } else {
                state = 2;
            }
        } else if state == 1 {
            if x <= 20 {
                result = 1;
            }
            state = 99;
        } else if state == 2 {
            result = 0;
            state = 99;
        }
    }
    
    return result;
}

fn main() -> u64 {
    let r1: u64 = check_range_obfuscated(15);
    let r2: u64 = check_range_obfuscated(25);
    return r1;  // Returns 1
}
```
</details>

### Exercise 4: Obfuscate Multiplication

Create a function that multiplies by 3 without using the * operator.

<details>
<summary>Solution</summary>

```lwanga
fn multiply_by_three(x: u64) -> u64 {
    // x * 3 = x + x + x
    return x + x + x;
}

fn multiply_by_three_obf(x: u64) -> u64 {
    // x * 3 = (x << 1) + x = 2x + x
    let doubled: u64 = x << 1;
    return doubled + x;
}

fn main() -> u64 {
    let result: u64 = multiply_by_three_obf(7);
    return result;  // Returns 21
}
```
</details>

### Exercise 5: Combined Obfuscation

Create a function that checks if a number equals 42, using multiple obfuscation techniques.

<details>
<summary>Solution</summary>

```lwanga
fn check_magic_number(n: u64) -> u64 {
    // Obfuscate 42
    let part1: u64 = 40;
    let part2: u64 = 2;
    let magic: u64 = part1 + part2;
    
    // Opaque predicate
    let opaque: u64 = (n * n) >= 0;
    
    if opaque == 0 {
        return 0;  // Never happens
    }
    
    // State machine
    let mut state: u64 = 0;
    let mut result: u64 = 0;
    
    while state < 3 {
        if state == 0 {
            if n == magic {
                state = 1;
            } else {
                state = 2;
            }
        } else if state == 1 {
            result = 1;
            state = 3;
        } else if state == 2 {
            result = 0;
            state = 3;
        }
    }
    
    return result;
}

fn main() -> u64 {
    let r1: u64 = check_magic_number(42);
    let r2: u64 = check_magic_number(100);
    return r1;  // Returns 1
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What code obfuscation is and why it's used
- ✅ How to use compiler obfuscation flags
- ✅ What transformations the compiler applies
- ✅ Manual obfuscation techniques (opaque predicates, constant obfuscation, control flow)
- ✅ How to balance protection with maintainability
- ✅ Common mistakes and how to avoid them
- ✅ Practical examples combining multiple techniques

**Remember:** Obfuscation is not encryption. Determined attackers can still reverse engineer obfuscated code, but it significantly raises the bar and time required.

---

## Next Lesson

Now that you understand code obfuscation, let's explore exploit development techniques in [Lesson 32: Exploit Development](32-exploit-development.md).

---

**Navigation**: [← Previous: Shellcode Basics](30-shellcode-basics.md) | [Tutorial Home](README.md) | [Next: Exploit Development →](32-exploit-development.md)
