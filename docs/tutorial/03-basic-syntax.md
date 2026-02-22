# Lesson 3: Basic Syntax

**In this lesson, you'll learn the fundamental syntax rules of Lwanga - how to write code that the compiler understands.**

---

## Comments

Comments are text that the compiler ignores. They're for humans to read and understand code.

### Single-Line Comments

Use `//` for single-line comments:

```lwanga
// This is a comment
let x: u64 = 42;  // This is also a comment

// Comments can explain what code does
// They can span multiple lines
// By using // on each line
```

**When to use:**
- Explain complex logic
- Document why you made a decision
- Temporarily disable code during debugging

### Multi-Line Comments

Use `/* */` for multi-line comments:

```lwanga
/*
 * This is a multi-line comment
 * It can span many lines
 * Useful for longer explanations
 */

fn main() -> u64 {
    /* You can also use them inline */ let x: u64 = 42;
    return 0;
}
```

**When to use:**
- Long explanations
- Documentation blocks
- Commenting out large sections of code

### Documentation Comments

While Lwanga doesn't have special doc comments yet, use this style for consistency:

```lwanga
/*
 * calculate_sum - Adds two numbers together
 * 
 * Parameters:
 *   a: First number
 *   b: Second number
 * 
 * Returns:
 *   The sum of a and b
 */
fn calculate_sum(a: u64, b: u64) -> u64 {
    return a + b;
}
```

---

## Statements vs Expressions

Understanding the difference is crucial in Lwanga.

### Statements

A **statement** performs an action and doesn't return a value. It must end with a semicolon (`;`).

```lwanga
let x: u64 = 42;        // Variable declaration statement
return 0;               // Return statement
```

### Expressions

An **expression** produces a value.

```lwanga
42                      // Number literal expression
x + 10                  // Addition expression
calculate_sum(5, 3)     // Function call expression
```

### Why This Matters

```lwanga
fn main() -> u64 {
    let x: u64 = 10 + 5;  // 10 + 5 is an expression
                          // The whole line is a statement
    
    return x * 2;         // x * 2 is an expression
                          // return is a statement
}
```

**Design choice:** This separation makes code behavior predictable and easier to understand.

---

## Semicolons

Semicolons (`;`) are **required** at the end of statements.

### When You Need Semicolons

```lwanga
let x: u64 = 42;           // ✓ Variable declaration
return 0;                  // ✓ Return statement
syscall(1, 1, msg, 14);    // ✓ Function call
```

### When You Don't Need Semicolons

```lwanga
fn main() -> u64 {         // ✗ Function declaration
    if x > 5 {             // ✗ If statement
        return 1;          // ✓ But return needs one
    }                      // ✗ Closing brace
}                          // ✗ Closing brace
```

### Common Mistake

```lwanga
let x: u64 = 42  // ERROR: missing semicolon
```

**Error message:**
```
error[E0001]: expected ';' after statement
```

**Fix:**
```lwanga
let x: u64 = 42;  // ✓ Correct
```

---

## Whitespace

Lwanga is **whitespace-insensitive** (mostly). The compiler ignores extra spaces, tabs, and newlines.

### These Are All Equivalent

```lwanga
// Version 1: Compact
fn main()->u64{let x:u64=42;return x;}

// Version 2: Readable (recommended)
fn main() -> u64 {
    let x: u64 = 42;
    return x;
}

// Version 3: Extra whitespace
fn main()    ->    u64    {
    let    x:    u64    =    42;
    return    x;
}
```

**Best practice:** Use consistent spacing for readability (see [Style Guide](../style-guide.md)).

### Recommended Style

```lwanga
// Space after keywords
let x: u64 = 42;
if x > 5 {
    return 1;
}

// Space around operators
let y: u64 = a + b * c;

// Space after commas
fn add(a: u64, b: u64, c: u64) -> u64 {
    return a + b + c;
}
```

---

## Indentation

Indentation is **not required** by the compiler, but it's essential for readability.

### Bad (No Indentation)

```lwanga
fn main() -> u64 {
let x: u64 = 10;
if x > 5 {
let y: u64 = 20;
return y;
}
return 0;
}
```

### Good (Proper Indentation)

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    if x > 5 {
        let y: u64 = 20;
        return y;
    }
    return 0;
}
```

**Standard:** Use 4 spaces per indentation level (not tabs).

**Why 4 spaces?**
- Readable but not too wide
- Standard in many languages
- Works in all editors

**Tip:** Use `lwangafmt` to automatically format your code:
```bash
lwangafmt --write myfile.lwanga
```

---

## Identifiers (Names)

Identifiers are names for variables, functions, constants, etc.

### Rules for Identifiers

1. **Must start with a letter or underscore**
   ```lwanga
   let x: u64 = 42;        // ✓ Valid
   let _temp: u64 = 10;    // ✓ Valid
   let 2fast: u64 = 5;     // ✗ Invalid (starts with number)
   ```

2. **Can contain letters, numbers, and underscores**
   ```lwanga
   let user_id: u64 = 123;      // ✓ Valid
   let temp2: u64 = 456;        // ✓ Valid
   let my_var_123: u64 = 789;   // ✓ Valid
   ```

3. **Cannot be a keyword**
   ```lwanga
   let fn: u64 = 42;       // ✗ Invalid (fn is a keyword)
   let return: u64 = 10;   // ✗ Invalid (return is a keyword)
   ```

4. **Case-sensitive**
   ```lwanga
   let count: u64 = 1;
   let Count: u64 = 2;     // Different variable!
   let COUNT: u64 = 3;     // Also different!
   ```

### Naming Conventions

Follow these conventions for consistency:

```lwanga
// Variables and functions: snake_case
let user_count: u64 = 10;
fn calculate_total() -> u64 { return 0; }

// Constants: SCREAMING_SNAKE_CASE
const MAX_SIZE: u64 = 1024;
const DEFAULT_PORT: u16 = 8080;

// Structs: PascalCase
struct UserData {
    id: u64,
    name: ptr,
}
```

**Why these conventions?**
- Consistent with other systems languages (C, Rust)
- Easy to distinguish between types
- Widely accepted in the community

---

## Keywords

Keywords are reserved words that have special meaning in Lwanga. You cannot use them as identifiers.

### Complete List of Keywords

```
fn          // Function declaration
let         // Variable declaration
mut         // Mutable variable
const       // Constant declaration
return      // Return from function
if          // Conditional
else        // Alternative branch
while       // Loop
unsafe      // Unsafe block
asm         // Inline assembly
naked       // Naked function
import      // Import module
struct      // Structure definition
packed      // Packed structure
enc         // Encrypted string
register    // Register type
```

### Example of Each Keyword

```lwanga
// fn - function
fn my_function() -> u64 { return 0; }

// let - variable
let x: u64 = 42;

// mut - mutable
let mut y: u64 = 10;
y = 20;

// const - constant
const MAX: u64 = 100;

// return - return value
return 42;

// if/else - conditional
if x > 5 {
    return 1;
} else {
    return 0;
}

// while - loop
while x < 10 {
    x = x + 1;
}

// unsafe - unsafe block
unsafe {
    syscall(1, 1, msg, 14);
}

// asm - inline assembly
asm {
    mov rax, 60
    syscall
}

// naked - naked function
naked fn shellcode() -> u64 {
    asm { /* ... */ }
}

// import - import module
import "lib/io.lwanga";

// struct - structure
struct Point {
    x: u64,
    y: u64,
}

// packed - packed structure
packed struct Header {
    magic: u32,
    version: u16,
}

// enc - encrypted string
let secret: ptr = enc "password";

// register - register type
let rax: register = 0;
```

---

## Code Blocks

Code blocks group statements together using curly braces `{ }`.

### Function Blocks

```lwanga
fn main() -> u64 {
    // Everything between { and } is the function body
    let x: u64 = 42;
    return x;
}
```

### Conditional Blocks

```lwanga
if x > 5 {
    // This block executes if condition is true
    return 1;
}
```

### Unsafe Blocks

```lwanga
unsafe {
    // Unsafe operations go here
    syscall(1, 1, msg, 14);
}
```

### Nested Blocks

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        let y: u64 = 20;
        
        if y > 15 {
            return 1;
        }
    }
    
    return 0;
}
```

**Design choice:** Blocks create scope - variables declared inside a block are only visible within that block.

---

## Program Structure

Every Lwanga program follows this structure:

```lwanga
// 1. Optional imports (at the top)
import "lib/io.lwanga";

// 2. Optional constants
const MAX_SIZE: u64 = 1024;

// 3. Optional struct definitions
struct Point {
    x: u64,
    y: u64,
}

// 4. Optional helper functions
fn helper_function() -> u64 {
    return 42;
}

// 5. Required: main function
fn main() -> u64 {
    // Your code here
    return 0;
}
```

**Why this order?**
- Imports first (dependencies)
- Constants next (configuration)
- Types and functions (implementation)
- Main function last (entry point)

This makes code easy to navigate and understand.

---

## Complete Example

Here's a complete program demonstrating all syntax concepts:

```lwanga
/*
 * Example program demonstrating Lwanga syntax
 * This program prints a greeting and calculates a sum
 */

// Import statement (if we had modules)
// import "lib/math.lwanga";

// Constants use SCREAMING_SNAKE_CASE
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

// Struct uses PascalCase
struct Result {
    value: u64,
    success: u64,
}

// Helper function uses snake_case
fn calculate_sum(a: u64, b: u64) -> u64 {
    // Local variable
    let result: u64 = a + b;
    
    // Return the result
    return result;
}

// Main function - entry point
fn main() -> u64 {
    // Print a greeting
    let greeting: ptr = "Hello from Lwanga!\n";
    
    unsafe {
        // System call to write
        syscall(SYS_WRITE, STDOUT, greeting, 19);
    }
    
    // Calculate something
    let x: u64 = 10;
    let y: u64 = 20;
    let sum: u64 = calculate_sum(x, y);
    
    // Conditional logic
    if sum > 25 {
        return 1;  // Success
    } else {
        return 0;  // Failure
    }
}
```

---

## Common Syntax Errors

### Error 1: Missing Semicolon

```lwanga
let x: u64 = 42  // ERROR
```

**Fix:**
```lwanga
let x: u64 = 42;  // ✓
```

### Error 2: Invalid Identifier

```lwanga
let 123abc: u64 = 42;  // ERROR: starts with number
```

**Fix:**
```lwanga
let abc123: u64 = 42;  // ✓
```

### Error 3: Using Keyword as Name

```lwanga
let fn: u64 = 42;  // ERROR: fn is a keyword
```

**Fix:**
```lwanga
let func: u64 = 42;  // ✓
```

### Error 4: Mismatched Braces

```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    return x;
// ERROR: missing closing brace
```

**Fix:**
```lwanga
fn main() -> u64 {
    let x: u64 = 42;
    return x;
}  // ✓
```

---

## Exercises

### Exercise 1: Fix the Syntax Errors

Find and fix all syntax errors in this code:

```lwanga
fn main() -> u64 {
    let message: ptr = "Hello"
    let 2count: u64 = 42;
    let return: u64 = 10
    return 0
```

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let message: ptr = "Hello";      // Added semicolon
    let count2: u64 = 42;            // Fixed identifier (can't start with number)
    let result: u64 = 10;            // Changed name (return is keyword), added semicolon
    return 0;                         // Added semicolon
}                                     // Added closing brace
```
</details>

### Exercise 2: Add Comments

Add appropriate comments to this code:

```lwanga
const MAX_SIZE: u64 = 1024;

fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    let sum: u64 = x + y;
    return sum;
}
```

<details>
<summary>Solution</summary>

```lwanga
// Maximum buffer size in bytes
const MAX_SIZE: u64 = 1024;

// Main function - program entry point
fn main() -> u64 {
    // Initialize two numbers
    let x: u64 = 10;
    let y: u64 = 20;
    
    // Calculate their sum
    let sum: u64 = x + y;
    
    // Return the result
    return sum;
}
```
</details>

### Exercise 3: Format Code

Reformat this code with proper indentation and spacing:

```lwanga
fn main()->u64{let x:u64=10;if x>5{return 1;}return 0;}
```

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        return 1;
    }
    
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to write comments (single-line and multi-line)
- ✅ The difference between statements and expressions
- ✅ When to use semicolons
- ✅ How whitespace and indentation work
- ✅ Rules for naming identifiers
- ✅ All Lwanga keywords
- ✅ How to structure code with blocks
- ✅ Proper program organization
- ✅ Common syntax errors and how to fix them

---

## Next Lesson

Now that you understand the syntax rules, let's learn about variables and data types in [Lesson 4: Variables and Data Types](04-variables-types.md).

---

**Navigation**: [← Previous: Hello World](02-hello-world.md) | [Tutorial Home](README.md) | [Next: Variables and Types →](04-variables-types.md)
