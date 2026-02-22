# Lesson 8: Loops

**In this lesson, you'll learn how to repeat code using while loops, understand loop control, and master iteration patterns.**

---

## What are Loops?

Loops allow you to repeat code multiple times without writing it over and over. They're essential for processing collections, counting, and any repetitive task.

**Without loops:**
```lwanga
let sum: u64 = 0;
sum = sum + 1;
sum = sum + 2;
sum = sum + 3;
sum = sum + 4;
sum = sum + 5;
// ... tedious and error-prone!
```

**With loops:**
```lwanga
let mut sum: u64 = 0;
let mut i: u64 = 1;
while i <= 5 {
    sum = sum + i;
    i = i + 1;
}
```

---

## While Loops

### Basic While Loop

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    while counter < 10 {
        counter = counter + 1;
    }
    
    return counter;  // Returns 10
}
```

**Explanation:**
- `while` - Keyword that starts a loop
- `counter < 10` - Condition checked before each iteration
- `{ }` - Code block that repeats
- Loop continues as long as condition is true
- Loop stops when condition becomes false

**How it works:**
1. Check condition (`counter < 10`)
2. If true, execute the block
3. Go back to step 1
4. If false, exit the loop

**Design choice:** Lwanga only has while loops (no for loops). This keeps the language simple while still being powerful enough for all iteration needs.

### Loop Execution Flow

```lwanga
fn main() -> u64 {
    let mut i: u64 = 0;
    
    // Iteration 1: i=0, check 0<3 (true), execute, i becomes 1
    // Iteration 2: i=1, check 1<3 (true), execute, i becomes 2
    // Iteration 3: i=2, check 2<3 (true), execute, i becomes 3
    // Check: i=3, check 3<3 (false), exit loop
    
    while i < 3 {
        i = i + 1;
    }
    
    return i;  // Returns 3
}
```

---

## Loop Patterns

### Counting Up

```lwanga
fn main() -> u64 {
    let mut i: u64 = 1;
    
    while i <= 10 {
        // Do something with i
        i = i + 1;
    }
    
    return i;  // Returns 11
}
```

### Counting Down

```lwanga
fn main() -> u64 {
    let mut i: u64 = 10;
    
    while i > 0 {
        // Do something with i
        i = i - 1;
    }
    
    return i;  // Returns 0
}
```

**Warning:** Be careful with unsigned integers when counting down!

```lwanga
// Dangerous! Will underflow
let mut i: u64 = 10;
while i >= 0 {  // This is ALWAYS true!
    i = i - 1;  // When i=0, this wraps to max u64
}
```

**Safe version:**
```lwanga
let mut i: u64 = 10;
while i > 0 {  // Stop before reaching 0
    i = i - 1;
}
```

### Counting by Steps

```lwanga
fn main() -> u64 {
    let mut i: u64 = 0;
    
    // Count by 2s: 0, 2, 4, 6, 8
    while i < 10 {
        // Do something with i
        i = i + 2;
    }
    
    return i;  // Returns 10
}
```

---

## Loop with Accumulator

### Sum of Numbers

```lwanga
fn sum_to_n(n: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while i <= n {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn main() -> u64 {
    return sum_to_n(100);  // Returns 5050
}
```

**Explanation:**
- `sum` is the accumulator - it collects the result
- `i` is the counter - it tracks which iteration we're on
- Each iteration adds `i` to `sum`
- After loop, `sum` contains the total

### Product of Numbers

```lwanga
fn factorial(n: u64) -> u64 {
    let mut product: u64 = 1;
    let mut i: u64 = 1;
    
    while i <= n {
        product = product * i;
        i = i + 1;
    }
    
    return product;
}

fn main() -> u64 {
    return factorial(5);  // Returns 120 (5! = 5*4*3*2*1)
}
```

---

## Breaking Out of Loops

### Early Exit with Return

```lwanga
fn find_value(target: u64) -> u64 {
    let mut i: u64 = 0;
    
    while i < 1000 {
        if i == target {
            return i;  // Found it! Exit immediately
        }
        i = i + 1;
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    return find_value(42);  // Returns 42
}
```

**Why use return to exit?**
- Simple and clear
- Immediately exits the function
- Returns a value at the same time
- No need for break statement

### Search Pattern

```lwanga
fn contains_zero(start: u64, end: u64) -> u64 {
    let mut i: u64 = start;
    
    while i <= end {
        if i == 0 {
            return 1;  // Found zero
        }
        i = i + 1;
    }
    
    return 0;  // No zero found
}

fn main() -> u64 {
    return contains_zero(1, 100);  // Returns 0 (no zero in range)
}
```

---

## Nested Loops

### Basic Nested Loop

```lwanga
fn main() -> u64 {
    let mut i: u64 = 0;
    let mut count: u64 = 0;
    
    while i < 10 {
        let mut j: u64 = 0;
        while j < 10 {
            count = count + 1;
            j = j + 1;
        }
        i = i + 1;
    }
    
    return count;  // Returns 100 (10 * 10)
}
```

**Explanation:**
- Outer loop runs 10 times
- For each outer iteration, inner loop runs 10 times
- Total iterations: 10 × 10 = 100

### Multiplication Table

```lwanga
fn calculate_product(row: u64, col: u64) -> u64 {
    return row * col;
}

fn main() -> u64 {
    let mut i: u64 = 1;
    let mut sum: u64 = 0;
    
    while i <= 5 {
        let mut j: u64 = 1;
        while j <= 5 {
            let product: u64 = calculate_product(i, j);
            sum = sum + product;
            j = j + 1;
        }
        i = i + 1;
    }
    
    return sum;
}
```

### Triangle Pattern

```lwanga
fn count_triangle(size: u64) -> u64 {
    let mut i: u64 = 1;
    let mut total: u64 = 0;
    
    while i <= size {
        let mut j: u64 = 1;
        while j <= i {  // Inner loop depends on outer
            total = total + 1;
            j = j + 1;
        }
        i = i + 1;
    }
    
    return total;
}

fn main() -> u64 {
    return count_triangle(5);  // Returns 15 (1+2+3+4+5)
}
```

---

## Infinite Loops

### Intentional Infinite Loop

```lwanga
fn main() -> u64 {
    let mut counter: u64 = 0;
    
    while 1 == 1 {  // Always true
        counter = counter + 1;
        
        if counter > 100 {
            return counter;  // Must have exit condition!
        }
    }
}
```

**When to use infinite loops:**
- Server main loops
- Event processing
- Game loops
- When you control exit with return

**Warning:** Always ensure there's a way to exit!

### Server Loop Pattern

```lwanga
fn process_request() -> u64 {
    // Process one request
    return 1;
}

fn server_loop() -> u64 {
    let mut requests_handled: u64 = 0;
    
    while 1 == 1 {
        let result: u64 = process_request();
        
        if result == 0 {
            return requests_handled;  // Shutdown signal
        }
        
        requests_handled = requests_handled + 1;
    }
}
```

---

## Common Loop Patterns

### Pattern 1: Process N Items

```lwanga
fn process_items(count: u64) -> u64 {
    let mut i: u64 = 0;
    let mut processed: u64 = 0;
    
    while i < count {
        // Process item i
        processed = processed + 1;
        i = i + 1;
    }
    
    return processed;
}
```

### Pattern 2: Search Until Found

```lwanga
fn find_first_divisible(n: u64) -> u64 {
    let mut i: u64 = 1;
    
    while i < 1000 {
        if i % n == 0 {
            return i;  // Found first divisible number
        }
        i = i + 1;
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    return find_first_divisible(7);  // Returns 7
}
```

### Pattern 3: Accumulate Until Condition

```lwanga
fn sum_until_limit(limit: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while sum < limit {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum;
}

fn main() -> u64 {
    return sum_until_limit(100);  // Returns 105 (1+2+...+14)
}
```

### Pattern 4: Count Occurrences

```lwanga
fn count_even_numbers(start: u64, end: u64) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = start;
    
    while i <= end {
        if i % 2 == 0 {
            count = count + 1;
        }
        i = i + 1;
    }
    
    return count;
}

fn main() -> u64 {
    return count_even_numbers(1, 100);  // Returns 50
}
```

### Pattern 5: Running Average

```lwanga
fn calculate_average(count: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while i <= count {
        sum = sum + i;
        i = i + 1;
    }
    
    return sum / count;
}

fn main() -> u64 {
    return calculate_average(10);  // Returns 5
}
```

---

## Practical Examples

### Example 1: Power Function

```lwanga
fn power(base: u64, exponent: u64) -> u64 {
    let mut result: u64 = 1;
    let mut i: u64 = 0;
    
    while i < exponent {
        result = result * base;
        i = i + 1;
    }
    
    return result;
}

fn main() -> u64 {
    return power(2, 10);  // Returns 1024
}
```

### Example 2: GCD (Greatest Common Divisor)

```lwanga
fn gcd(a: u64, b: u64) -> u64 {
    let mut x: u64 = a;
    let mut y: u64 = b;
    
    while y != 0 {
        let temp: u64 = y;
        y = x % y;
        x = temp;
    }
    
    return x;
}

fn main() -> u64 {
    return gcd(48, 18);  // Returns 6
}
```

### Example 3: Fibonacci Sequence

```lwanga
fn fibonacci(n: u64) -> u64 {
    if n <= 1 {
        return n;
    }
    
    let mut a: u64 = 0;
    let mut b: u64 = 1;
    let mut i: u64 = 2;
    
    while i <= n {
        let temp: u64 = a + b;
        a = b;
        b = temp;
        i = i + 1;
    }
    
    return b;
}

fn main() -> u64 {
    return fibonacci(10);  // Returns 55
}
```

### Example 4: Prime Number Check

```lwanga
fn is_prime(n: u64) -> u64 {
    if n < 2 {
        return 0;  // Not prime
    }
    
    if n == 2 {
        return 1;  // 2 is prime
    }
    
    if n % 2 == 0 {
        return 0;  // Even numbers (except 2) aren't prime
    }
    
    let mut i: u64 = 3;
    while i * i <= n {
        if n % i == 0 {
            return 0;  // Found a divisor
        }
        i = i + 2;  // Check only odd numbers
    }
    
    return 1;  // Prime
}

fn main() -> u64 {
    return is_prime(17);  // Returns 1
}
```

### Example 5: Reverse Number

```lwanga
fn reverse_number(n: u64) -> u64 {
    let mut num: u64 = n;
    let mut reversed: u64 = 0;
    
    while num > 0 {
        let digit: u64 = num % 10;
        reversed = reversed * 10 + digit;
        num = num / 10;
    }
    
    return reversed;
}

fn main() -> u64 {
    return reverse_number(12345);  // Returns 54321
}
```

---

## Common Mistakes

### Mistake 1: Infinite Loop (Forgot to Update Counter)

```lwanga
fn main() -> u64 {
    let mut i: u64 = 0;
    
    while i < 10 {
        // ERROR: Forgot to increment i!
        // This loops forever
    }
    
    return i;
}
```

**Fix:**
```lwanga
while i < 10 {
    // Do something
    i = i + 1;  // ✓ Don't forget this!
}
```

### Mistake 2: Off-by-One Error

```lwanga
// Want to loop 10 times (0-9)
let mut i: u64 = 0;
while i <= 10 {  // ERROR: Loops 11 times!
    i = i + 1;
}

// Correct
let mut i: u64 = 0;
while i < 10 {  // ✓ Loops exactly 10 times
    i = i + 1;
}
```

### Mistake 3: Using Immutable Variable

```lwanga
fn main() -> u64 {
    let i: u64 = 0;  // ERROR: Not mutable
    
    while i < 10 {
        i = i + 1;  // Can't modify immutable variable
    }
    
    return i;
}
```

**Fix:**
```lwanga
let mut i: u64 = 0;  // ✓ Make it mutable
```

### Mistake 4: Wrong Condition

```lwanga
// Want to count from 1 to 10
let mut i: u64 = 1;
while i < 10 {  // ERROR: Stops at 9
    i = i + 1;
}

// Correct
let mut i: u64 = 1;
while i <= 10 {  // ✓ Includes 10
    i = i + 1;
}
```

---

## Exercises

### Exercise 1: Sum of Even Numbers

Write a function that returns the sum of all even numbers from 1 to n.

<details>
<summary>Solution</summary>

```lwanga
fn sum_even(n: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 2;
    
    while i <= n {
        sum = sum + i;
        i = i + 2;
    }
    
    return sum;
}

fn main() -> u64 {
    return sum_even(10);  // Returns 30 (2+4+6+8+10)
}
```
</details>

### Exercise 2: Count Digits

Write a function that counts the number of digits in a number.

<details>
<summary>Solution</summary>

```lwanga
fn count_digits(n: u64) -> u64 {
    if n == 0 {
        return 1;
    }
    
    let mut count: u64 = 0;
    let mut num: u64 = n;
    
    while num > 0 {
        count = count + 1;
        num = num / 10;
    }
    
    return count;
}

fn main() -> u64 {
    return count_digits(12345);  // Returns 5
}
```
</details>

### Exercise 3: Find Largest Divisor

Write a function that finds the largest divisor of n (other than n itself).

<details>
<summary>Solution</summary>

```lwanga
fn largest_divisor(n: u64) -> u64 {
    let mut i: u64 = n / 2;
    
    while i > 0 {
        if n % i == 0 {
            return i;
        }
        i = i - 1;
    }
    
    return 1;
}

fn main() -> u64 {
    return largest_divisor(100);  // Returns 50
}
```
</details>

### Exercise 4: Sum of Squares

Write a function that returns the sum of squares from 1² to n².

<details>
<summary>Solution</summary>

```lwanga
fn sum_of_squares(n: u64) -> u64 {
    let mut sum: u64 = 0;
    let mut i: u64 = 1;
    
    while i <= n {
        sum = sum + (i * i);
        i = i + 1;
    }
    
    return sum;
}

fn main() -> u64 {
    return sum_of_squares(5);  // Returns 55 (1+4+9+16+25)
}
```
</details>

### Exercise 5: Collatz Sequence Length

Write a function that returns the length of the Collatz sequence starting from n.
(If n is even, divide by 2. If odd, multiply by 3 and add 1. Stop at 1.)

<details>
<summary>Solution</summary>

```lwanga
fn collatz_length(n: u64) -> u64 {
    let mut length: u64 = 0;
    let mut num: u64 = n;
    
    while num != 1 {
        if num % 2 == 0 {
            num = num / 2;
        } else {
            num = num * 3 + 1;
        }
        length = length + 1;
    }
    
    return length;
}

fn main() -> u64 {
    return collatz_length(10);  // Returns 6
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How while loops work and when to use them
- ✅ Loop patterns: counting up, down, by steps
- ✅ Accumulator pattern for collecting results
- ✅ How to exit loops early with return
- ✅ Nested loops for multi-dimensional iteration
- ✅ Infinite loops and when they're useful
- ✅ Common loop patterns and idioms
- ✅ Common mistakes and how to avoid them
- ✅ Practical examples and exercises

---

## Next Lesson

Now that you can repeat code with loops, let's master all operators in Lwanga in [Lesson 9: Operators](09-operators.md).

---

**Navigation**: [← Previous: Conditionals](07-conditionals.md) | [Tutorial Home](README.md) | [Next: Operators →](09-operators.md)
