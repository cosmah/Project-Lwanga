# Lesson 6: Functions

**In this lesson, you'll learn how to organize code into reusable functions, pass parameters, return values, and understand function scope.**

---

## What is a Function?

A function is a named block of code that performs a specific task. Think of it as a mini-program within your program.

**Why use functions?**
- **Organize code** - Break complex problems into smaller, manageable pieces
- **Reuse code** - Write once, use many times
- **Avoid repetition** - DRY (Don't Repeat Yourself) principle
- **Make code readable** - Clear, descriptive names explain what code does
- **Easier testing** - Test individual functions separately
- **Easier debugging** - Isolate problems to specific functions

---

## Function Syntax

### Basic Function Declaration

```lwanga
fn function_name(parameter: type) -> return_type {
    // Function body
    return value;
}
```

**Parts explained:**
- `fn` - Keyword that declares a function
- `function_name` - The name you choose (use snake_case)
- `(parameter: type)` - Input parameters (can be empty)
- `-> return_type` - The type of value this function returns
- `{ }` - Curly braces contain the function body
- `return value;` - Statement that returns a value

---

## Simple Functions

### Function with No Parameters

```lwanga
fn get_answer() -> u64 {
    return 42;
}

fn main() -> u64 {
    let answer: u64 = get_answer();
    return answer;  // Returns 42
}
```

**Explanation:**
- `get_answer()` takes no parameters (empty parentheses)
- It always returns the value `42`
- We call it with `get_answer()` - note the empty parentheses
- The return value is stored in the `answer` variable

**Why is this useful?**

Even simple functions like this are valuable:
- Makes code self-documenting (the name explains what it does)
- Easy to change the value in one place
- Can add logic later without changing calling code

### Function with One Parameter

```lwanga
fn double(x: u64) -> u64 {
    return x * 2;
}

fn main() -> u64 {
    let result: u64 = double(21);
    return result;  // Returns 42
}
```

**Explanation:**
- `double` takes one parameter `x` of type `u64`
- It multiplies `x` by 2 and returns the result
- We call it with `double(21)` - passing 21 as the argument
- The function returns `42` (21 * 2)

**Parameter vs Argument:**
- **Parameter** - Variable in the function definition (`x` in this case)
- **Argument** - Actual value passed when calling (`21` in this case)

### Function with Multiple Parameters

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let sum: u64 = add(10, 20);
    return sum;  // Returns 30
}
```

**Explanation:**
- `add` takes two parameters: `a` and `b`, both type `u64`
- Parameters are separated by commas
- The function returns the sum of `a` and `b`
- We call it with `add(10, 20)` - passing two arguments

**Design choice:** Lwanga requires explicit types for all parameters. This makes code clear and catches type errors at compile time.

---

## Return Values

### Returning a Value

Every function in Lwanga must return a value of its declared type:

```lwanga
fn get_ten() -> u64 {
    return 10;  // Must return u64
}

fn get_hundred() -> u64 {
    let x: u64 = 100;
    return x;  // Can return a variable
}

fn calculate() -> u64 {
    return 5 + 5;  // Can return an expression
}
```

**Why must every function return?**

This is a design choice for simplicity and safety:
- No "void" functions to remember
- Always clear what a function produces
- Easier to reason about code flow
- If you don't need a return value, just return 0

### Returning Early

You can return from anywhere in a function:

```lwanga
fn check_value(x: u64) -> u64 {
    if (x > 100) {
        return 1;  // Early return
    }
    
    if (x > 50) {
        return 2;  // Another early return
    }
    
    return 0;  // Default return
}

fn main() -> u64 {
    let result1: u64 = check_value(150);  // Returns 1
    let result2: u64 = check_value(75);   // Returns 2
    let result3: u64 = check_value(25);   // Returns 0
    
    return result1;
}
```

**Why use early returns?**
- Simplify logic - handle special cases first
- Avoid deep nesting - no need for else-if chains
- Make code more readable - clear exit points
- Reduce cognitive load - less to keep in mind

### Returning Expressions

You can return the result of any expression:

```lwanga
fn calculate(x: u64, y: u64) -> u64 {
    return x * 2 + y * 3;
}

fn is_even(n: u64) -> u64 {
    return n % 2 == 0;  // Returns 1 (true) or 0 (false)
}

fn main() -> u64 {
    let result: u64 = calculate(5, 10);  // Returns 40
    let even: u64 = is_even(42);         // Returns 1
    
    return result;
}
```

---

## Function Parameters

### Parameter Passing

Parameters in Lwanga are **passed by value** - the function gets a copy:

```lwanga
fn modify(x: u64) -> u64 {
    let y: u64 = x + 10;
    return y;
}

fn main() -> u64 {
    let original: u64 = 5;
    let result: u64 = modify(original);
    
    // original is still 5 (unchanged)
    // result is 15
    
    return result;
}
```

**Why pass by value?**

**Design choice:** Pass-by-value is simple and safe:
- Original variable is never modified
- No hidden side effects
- Easy to understand what happens
- Prevents accidental bugs

**What about large data?**

For large data structures, you can pass pointers (we'll learn this later):
```lwanga
fn process_data(data: ptr) -> u64 {
    // Work with pointer to data
    return 0;
}
```

### Multiple Parameters of Different Types

```lwanga
fn process(count: u64, flag: u8, message: ptr) -> u64 {
    // Use the parameters
    if (flag == 1) {
        return count;
    }
    return 0;
}

fn main() -> u64 {
    let msg: ptr = "Hello";
    let result: u64 = process(10, 1, msg);
    return result;  // Returns 10
}
```

### Parameter Order Matters

```lwanga
fn subtract(a: u64, b: u64) -> u64 {
    return a - b;
}

fn main() -> u64 {
    let r1: u64 = subtract(10, 3);  // 7 (10 - 3)
    let r2: u64 = subtract(3, 10);  // Underflow! (wraps around to large number)
    
    return r1;
}
```

**Important:** The order you pass arguments must match the order of parameters!

---

## Calling Functions

### Basic Function Call

```lwanga
fn greet() -> u64 {
    let msg: ptr = "Hello!";
    unsafe {
        syscall(1, 1, msg, 6);
    }
    return 0;
}

fn main() -> u64 {
    greet();  // Call the function
    greet();  // Can call multiple times
    greet();
    
    return 0;
}
```

### Using Return Values

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    // Store return value in variable
    let sum: u64 = add(5, 3);
    
    // Use return value in expression
    let total: u64 = add(10, 20) + add(5, 5);
    
    // Use return value in condition
    if add(2, 2) == 4 {
        return 1;
    }
    
    // Use return value directly
    return add(100, 200);
}
```

### Nested Function Calls

```lwanga
fn double(x: u64) -> u64 {
    return x * 2;
}

fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    // Nested calls - inner function executes first
    let result: u64 = double(add(5, 3));
    // Step 1: add(5, 3) = 8
    // Step 2: double(8) = 16
    
    return result;  // Returns 16
}
```

**Execution order:**
1. Innermost function calls execute first
2. Their return values become arguments to outer functions
3. Outer functions execute with those values

---

## Function Scope

### Local Variables

Variables declared in a function are local to that function:

```lwanga
fn calculate() -> u64 {
    let x: u64 = 10;  // Local to calculate()
    let y: u64 = 20;  // Local to calculate()
    return x + y;
}

fn main() -> u64 {
    let result: u64 = calculate();
    
    // ERROR: x and y don't exist here!
    // let z: u64 = x + y;
    
    return result;
}
```

**Why local scope?**
- Prevents naming conflicts
- Variables are automatically cleaned up
- Each function has its own workspace
- Makes code easier to understand

### Parameters are Local

Function parameters are also local variables:

```lwanga
fn process(value: u64) -> u64 {
    // value is local to this function
    let result: u64 = value * 2;
    return result;
}

fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = process(x);
    
    // x is still 10
    // value doesn't exist here
    
    return y;
}
```

---

## Practical Examples

### Example 1: Calculate Area

```lwanga
fn calculate_area(width: u64, height: u64) -> u64 {
    return width * height;
}

fn main() -> u64 {
    let room_width: u64 = 10;
    let room_height: u64 = 15;
    
    let area: u64 = calculate_area(room_width, room_height);
    
    return area;  // Returns 150
}
```

### Example 2: Temperature Conversion

```lwanga
fn fahrenheit_to_celsius(f: u64) -> u64 {
    // C = (F - 32) * 5 / 9
    if (f < 32) {
        return 0;  // Avoid underflow
    }
    
    let celsius: u64 = (f - 32) * 5 / 9;
    return celsius;
}

fn main() -> u64 {
    let temp_f: u64 = 100;
    let temp_c: u64 = fahrenheit_to_celsius(temp_f);
    
    return temp_c;  // Returns 37
}
```

### Example 3: Check Range

```lwanga
fn is_in_range(value: u64, min: u64, max: u64) -> u64 {
    if (value >= min && value <= max) {
        return 1;  // True
    }
    return 0;  // False
}

fn main() -> u64 {
    let age: u64 = 25;
    let is_adult: u64 = is_in_range(age, 18, 100);
    
    return is_adult;  // Returns 1
}
```

### Example 4: Maximum of Two Numbers

```lwanga
fn max(a: u64, b: u64) -> u64 {
    if (a > b) {
        return a;
    }
    return b;
}

fn main() -> u64 {
    let x: u64 = 42;
    let y: u64 = 17;
    let maximum: u64 = max(x, y);
    
    return maximum;  // Returns 42
}
```

### Example 5: Factorial

```lwanga
fn factorial(n: u64) -> u64 {
    if (n == 0 || n == 1) {
        return 1;
    }
    
    let result: u64 = 1;
    let i: u64 = 2;
    
    while (i <= n) {
        result = result * i;
        i = i + 1;
    }
    
    return result;
}

fn main() -> u64 {
    let fact5: u64 = factorial(5);
    return fact5;  // Returns 120 (5! = 5*4*3*2*1)
}
```

---

## Common Mistakes

### Mistake 1: Forgetting Return Statement

```lwanga
fn add(a: u64, b: u64) -> u64 {
    let sum: u64 = a + b;
    // ERROR: missing return statement - compiler requires explicit return
}
```

**Error message:**
```
error[E0007]: missing return statement
  --> program.lwanga:3:1
   |
1  | fn add(a: u64, b: u64) -> u64 {
   |                           --- expected u64
2  |     let sum: u64 = a + b;
3  | }
   | ^ expected return statement
```

**Fix:**
```lwanga
fn add(a: u64, b: u64) -> u64 {
    let sum: u64 = a + b;
    return sum;  // ✓ Correct
}
```

### Mistake 2: Wrong Return Type

```lwanga
fn get_value() -> u64 {
    return 42;  // 42 is u64 by default
}
```

**Fix:**
```lwanga
fn get_value() -> u64 {
    return 42;  // ✓ Correct
}

// Lwanga defaults to u64 for all integers
```

### Mistake 3: Wrong Number of Arguments

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    let sum: u64 = add(5);  // ERROR: missing argument
    return sum;
}
```

**Error message:**
```
error[E0008]: function expects 2 arguments, found 1
```

**Fix:**
```lwanga
let sum: u64 = add(5, 3);  // ✓ Correct
```

### Mistake 4: Using Variable from Another Function

```lwanga
fn calculate() -> u64 {
    let x: u64 = 10;
    return x;
}

fn main() -> u64 {
    return x;  // ERROR: x doesn't exist here
}
```

**Fix:**
```lwanga
fn main() -> u64 {
    let result: u64 = calculate();
    return result;  // ✓ Correct
}
```

---

## Exercises

### Exercise 1: Create a Square Function

Write a function that calculates the square of a number (n²).

<details>
<summary>Solution</summary>

```lwanga
fn square(n: u64) -> u64 {
    return n * n;
}

fn main() -> u64 {
    let result: u64 = square(7);
    return result;  // Returns 49
}
```
</details>

### Exercise 2: Create a Minimum Function

Write a function that returns the smaller of two numbers.

<details>
<summary>Solution</summary>

```lwanga
fn min(a: u64, b: u64) -> u64 {
    if (a < b) {
        return a;
    }
    return b;
}

fn main() -> u64 {
    let result: u64 = min(42, 17);
    return result;  // Returns 17
}
```
</details>

### Exercise 3: Create an Average Function

Write a function that calculates the average of three numbers.

<details>
<summary>Solution</summary>

```lwanga
fn average(a: u64, b: u64, c: u64) -> u64 {
    let sum: u64 = a + b + c;
    let avg: u64 = sum / 3;
    return avg;
}

fn main() -> u64 {
    let result: u64 = average(10, 20, 30);
    return result;  // Returns 20
}
```
</details>

### Exercise 4: Create a Power Function

Write a function that calculates a^b (a to the power of b).

<details>
<summary>Solution</summary>

```lwanga
fn power(base: u64, exponent: u64) -> u64 {
    if (exponent == 0) {
        return 1;
    }
    
    let mut result: u64 = 1;
    let mut i: u64 = 0;
    
    while (i < exponent) {
        result = result * base;
        i = i + 1;
    }
    
    return result;
}

fn main() -> u64 {
    let result: u64 = power(2, 10);
    return result;  // Returns 1024 (2^10)
}
```
</details>

### Exercise 5: Create a Digit Sum Function

Write a function that returns the sum of digits in a number.

<details>
<summary>Solution</summary>

```lwanga
fn digit_sum(n: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut num: u64 = n;
    
    while (num > 0) {
        sum = sum + (num % 10);
        num = num / 10;
    }
    
    return sum;
}

fn main() -> u64 {
    let result: u64 = digit_sum(12345);
    return result;  // Returns 15 (1+2+3+4+5)
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What functions are and why they're useful
- ✅ How to declare functions with parameters and return types
- ✅ How to call functions and use return values
- ✅ Parameter passing (by value)
- ✅ Function scope and local variables
- ✅ Early returns and when to use them
- ✅ Nested function calls
- ✅ Common mistakes and how to fix them
- ✅ Practical examples and exercises

---

## Next Lesson

Now that you can organize code with functions, let's learn how to make decisions in your code using conditionals in [Lesson 7: Conditionals](07-conditionals.md).

---

**Navigation**: [← Previous: Working with Numbers](05-numbers.md) | [Tutorial Home](README.md) | [Next: Conditionals →](07-conditionals.md)
