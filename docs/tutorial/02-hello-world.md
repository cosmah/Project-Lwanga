# Lesson 2: Your First Program - Hello World

**In this lesson, you'll write, compile, and run your first Lwanga program. We'll explain every single line in detail.**

---

## The Traditional Hello World

Let's start with the most famous program in programming history. Create a file called `hello.lwanga`:

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 0;
}
```

---

## Understanding Every Line

Let's break down this program line by line:

### Line 1: `fn main() -> u64 {`

```lwanga
fn main() -> u64 {
```

**What it means:**
- `fn` - Keyword that declares a function
- `main` - The name of the function (every program must have a `main` function)
- `()` - Empty parentheses mean this function takes no parameters
- `-> u64` - This function returns an unsigned 64-bit integer
- `{` - Opening brace starts the function body

**Why this design?**

Every Lwanga program starts execution at the `main` function. The return value becomes the program's exit code:
- `0` means success
- Non-zero means error

The operating system uses this exit code to know if your program succeeded or failed.

**Example:**
```bash
./hello
echo $?  # Prints the exit code (0 for success)
```

### Line 2: `let msg: ptr = "Hello, World!\n";`

```lwanga
let msg: ptr = "Hello, World!\n";
```

**What it means:**
- `let` - Keyword to declare a variable
- `msg` - The variable name (short for "message")
- `:` - Separates the name from the type
- `ptr` - The type (pointer - holds a memory address)
- `=` - Assignment operator
- `"Hello, World!\n"` - A string literal
- `;` - Semicolon ends the statement

**Why use `ptr` for strings?**

In Lwanga, strings are just sequences of bytes in memory. A `ptr` (pointer) holds the memory address where the string starts. This is similar to C, but more explicit.

The `\n` at the end is a newline character - it moves the cursor to the next line after printing.

**Design choice:** Lwanga uses explicit pointers instead of a complex string type. This keeps the language simple and binaries small.

### Line 3: `unsafe {`

```lwanga
unsafe {
```

**What it means:**
- `unsafe` - Keyword that marks a block of potentially dangerous code
- `{` - Opening brace starts the unsafe block

**Why is this needed?**

System calls interact directly with the operating system kernel. They can:
- Access any memory
- Perform privileged operations
- Crash the program if used incorrectly

By requiring `unsafe`, Lwanga makes it clear: "This code is doing something that could be dangerous. I know what I'm doing."

**Design choice:** This is safer than C (where everything is unsafe) but more flexible than Rust (where the borrow checker can be restrictive).

### Line 4: `syscall(1, 1, msg, 14);`

```lwanga
syscall(1, 1, msg, 14);
```

**What it means:**
- `syscall` - Built-in function to make system calls
- `1` - First argument: syscall number (1 = write)
- `1` - Second argument: file descriptor (1 = stdout)
- `msg` - Third argument: pointer to the data to write
- `14` - Fourth argument: number of bytes to write

**Breaking down the syscall:**

This is calling the Linux `write` system call:
```c
write(fd, buffer, count)
```

- `fd = 1` - File descriptor 1 is standard output (your terminal)
- `buffer = msg` - The data to write (our "Hello, World!\n" string)
- `count = 14` - Write 14 bytes (length of "Hello, World!\n")

**Why 14 bytes?**
```
H e l l o ,   W o r l d ! \n
1 2 3 4 5 6 7 8 9 0 1 2 3 4
```

**Design choice:** Direct syscalls give you full control and create smaller binaries than using libc functions like `printf`.

### Line 5: `}`

```lwanga
}
```

**What it means:**
- `}` - Closing brace ends the unsafe block

### Line 6: `return 0;`

```lwanga
return 0;
```

**What it means:**
- `return` - Keyword to exit the function and return a value
- `0` - The value to return (exit code 0 = success)
- `;` - Semicolon ends the statement

**Why return 0?**

By convention:
- `0` means the program succeeded
- Non-zero means an error occurred

This lets shell scripts and other programs check if your program worked:

```bash
./hello
if [ $? -eq 0 ]; then
    echo "Success!"
fi
```

### Line 7: `}`

```lwanga
}
```

**What it means:**
- `}` - Closing brace ends the main function

---

## Compiling Your Program

Now let's compile and run the program!

### Step 1: Save the File

Save the code above as `hello.lwanga` in your working directory.

### Step 2: Compile

```bash
lwangac hello.lwanga -o hello
```

**What this command does:**
- `lwangac` - The Lwanga compiler
- `hello.lwanga` - Your source file
- `-o hello` - Output file name (the executable)

**What happens during compilation:**

1. **Lexical Analysis**: Breaks code into tokens
2. **Parsing**: Builds an Abstract Syntax Tree (AST)
3. **Type Checking**: Verifies types are correct
4. **IR Generation**: Creates LLVM Intermediate Representation
5. **Optimization**: Improves the code
6. **Code Generation**: Produces machine code

You should see:
```
Lwanga Compiler v1.0.2
Compilation successful!
Output: hello
```

### Step 3: Run

```bash
./hello
```

You should see:
```
Hello, World!
```

Congratulations! You've written and run your first Lwanga program!

### Step 4: Check the Exit Code

```bash
echo $?
```

You should see:
```
0
```

This confirms your program returned 0 (success).

---

## Examining the Binary

Let's look at what we created:

### Check the File Size

```bash
ls -lh hello
```

You'll see something like:
```
-rwxr-xr-x 1 user user 8.9K Feb 22 10:00 hello
```

**Amazing!** The entire program is under 9KB! Compare this to:
- C with glibc: ~16KB
- Rust: ~300KB+
- Go: ~2MB+

**Why so small?**
- No standard library
- No runtime
- Direct syscalls
- Minimal overhead

### Check What's Inside

```bash
file hello
```

Output:
```
hello: ELF 64-bit LSB executable, x86-64, version 1 (SYSV), statically linked, not stripped
```

This tells us:
- It's a native executable (ELF format)
- 64-bit code
- Statically linked (no external dependencies)
- Can run on any Linux system

---

## Common Mistakes and How to Fix Them

### Mistake 1: Forgetting the Semicolon

```lwanga
let msg: ptr = "Hello, World!\n"  // ERROR: missing semicolon
```

**Error message:**
```
error[E0001]: expected ';' after statement
  --> hello.lwanga:2:37
   |
2  |     let msg: ptr = "Hello, World!\n"
   |                                     ^ expected ';'
```

**Fix:** Add the semicolon:
```lwanga
let msg: ptr = "Hello, World!\n";
```

### Mistake 2: Wrong String Length

```lwanga
syscall(1, 1, msg, 10);  // Only prints "Hello, Wor"
```

**What happens:** Only the first 10 characters are printed.

**Fix:** Count the characters correctly (including `\n`):
```lwanga
syscall(1, 1, msg, 14);  // Correct length
```

### Mistake 3: Forgetting `unsafe`

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    syscall(1, 1, msg, 14);  // ERROR: syscall requires unsafe
    return 0;
}
```

**Error message:**
```
error[E0004]: syscall requires unsafe block
  --> hello.lwanga:3:5
   |
3  |     syscall(1, 1, msg, 14);
   |     ^^^^^^^^^^^^^^^^^^^^^^ unsafe operation
   |
   = help: wrap this in an unsafe block
```

**Fix:** Wrap in `unsafe {}`:
```lwanga
unsafe {
    syscall(1, 1, msg, 14);
}
```

### Mistake 4: Wrong Return Type

```lwanga
fn main() -> u32 {  // ERROR: should be u64
    return 0;
}
```

**Error message:**
```
error[E0005]: main function must return u64
  --> hello.lwanga:1:14
   |
1  | fn main() -> u32 {
   |              ^^^ expected u64
```

**Fix:** Use `u64`:
```lwanga
fn main() -> u64 {
    return 0;
}
```

---

## Variations of Hello World

### Version 1: With Comments

```lwanga
// Hello World program in Lwanga
// This program prints a greeting to the terminal

fn main() -> u64 {
    // Store the message we want to print
    let msg: ptr = "Hello, World!\n";
    
    // Write the message to stdout (file descriptor 1)
    // syscall(1, ...) is the write syscall
    unsafe {
        syscall(1, 1, msg, 14);
    }
    
    // Return 0 to indicate success
    return 0;
}
```

### Version 2: Multiple Messages

```lwanga
fn main() -> u64 {
    let msg1: ptr = "Hello, ";
    let msg2: ptr = "World!\n";
    
    unsafe {
        syscall(1, 1, msg1, 7);   // Print "Hello, "
        syscall(1, 1, msg2, 7);   // Print "World!\n"
    }
    
    return 0;
}
```

### Version 3: Using Constants

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 14);
    }
    
    return 0;
}
```

**Why use constants?**
- Makes code more readable
- Easier to maintain
- Self-documenting

---

## Understanding System Calls

### What is a System Call?

A system call is how your program asks the operating system to do something:
- Write to the screen
- Read from a file
- Create a network connection
- Allocate memory

### Why Not Use `printf`?

You might be wondering: "Why not just use `printf` like in C?"

**Answer:** `printf` is part of the C standard library (libc). Using it would:
1. Increase binary size (adds ~100KB+)
2. Add dependencies (requires libc)
3. Add complexity (printf does formatting, parsing, etc.)

For "Hello, World!", we just need to write bytes to stdout. A syscall is perfect!

### The Write Syscall

```
syscall number: 1 (write)
arguments:
  - fd: file descriptor (1 = stdout)
  - buf: pointer to data
  - count: number of bytes
returns:
  - number of bytes written, or -1 on error
```

---

## Exercises

### Exercise 1: Change the Message

Modify the program to print "Welcome to Lwanga!\n" instead.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let msg: ptr = "Welcome to Lwanga!\n";
    unsafe {
        syscall(1, 1, msg, 19);  // 19 characters
    }
    return 0;
}
```
</details>

### Exercise 2: Print Your Name

Create a program that prints "Hello, [Your Name]!\n".

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, Alice!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 0;
}
```
</details>

### Exercise 3: Multiple Lines

Print three lines:
```
Line 1
Line 2
Line 3
```

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let line1: ptr = "Line 1\n";
    let line2: ptr = "Line 2\n";
    let line3: ptr = "Line 3\n";
    
    unsafe {
        syscall(1, 1, line1, 7);
        syscall(1, 1, line2, 7);
        syscall(1, 1, line3, 7);
    }
    
    return 0;
}
```
</details>

### Exercise 4: Error Exit Code

Modify the program to return exit code 1 instead of 0. Run it and check the exit code.

<details>
<summary>Solution</summary>

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!\n";
    unsafe {
        syscall(1, 1, msg, 14);
    }
    return 1;  // Changed from 0 to 1
}
```

Check with:
```bash
./hello
echo $?  # Should print 1
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to write a complete Lwanga program
- ✅ The structure of the `main` function
- ✅ How to declare variables with `let`
- ✅ What `unsafe` blocks are and why they're needed
- ✅ How to make system calls
- ✅ How to compile and run programs
- ✅ How to check exit codes
- ✅ Common mistakes and how to fix them

---

## Next Lesson

Now that you can write and run programs, let's learn about the basic syntax rules in [Lesson 3: Basic Syntax](03-basic-syntax.md).

---

**Navigation**: [← Previous: Introduction](01-introduction.md) | [Tutorial Home](README.md) | [Next: Basic Syntax →](03-basic-syntax.md)
