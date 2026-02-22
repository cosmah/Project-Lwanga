# Lesson 7: Conditionals

**In this lesson, you'll learn how to make decisions in your code using if statements, comparison operators, and boolean logic.**

---

## What are Conditionals?

Conditionals allow your program to make decisions and execute different code based on conditions. They're essential for creating programs that respond to different situations.

**Real-world analogy:**
```
IF it's raining THEN take an umbrella
IF temperature > 30 THEN turn on AC
IF password is correct THEN log in
```

---

## If Statements

### Basic If Statement

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        return 1;
    }
    
    return 0;
}
```

**Explanation:**
- `if` - Keyword that starts a conditional
- `x > 5` - The condition to check (must evaluate to true/false)
- `{ }` - Code block that executes if condition is true
- If condition is false, the block is skipped

**How it works:**
1. Evaluate the condition (`x > 5`)
2. If true (non-zero), execute the block
3. If false (zero), skip the block

**Design choice:** In Lwanga, any non-zero value is considered "true", and zero is "false". This is common in systems programming.

---

## If-Else

### Basic If-Else

```lwanga
fn main() -> u64 {
    let age: u64 = 18;
    
    if age >= 18 {
        return 1;  // Adult
    } else {
        return 0;  // Minor
    }
}
```

**Explanation:**
- `else` - Keyword for the alternative path
- If condition is true, execute first block
- If condition is false, execute else block
- Exactly one block will execute

### Multiple Conditions

```lwanga
fn check_temperature(temp: u64) -> u64 {
    if temp > 30 {
        return 3;  // Hot
    } else {
        if temp > 20 {
            return 2;  // Warm
        } else {
            if temp > 10 {
                return 1;  // Cool
            } else {
                return 0;  // Cold
            }
        }
    }
}

fn main() -> u64 {
    return check_temperature(25);  // Returns 2 (Warm)
}
```

**Better way (less nesting):**

```lwanga
fn check_temperature(temp: u64) -> u64 {
    if temp > 30 {
        return 3;  // Hot
    }
    
    if temp > 20 {
        return 2;  // Warm
    }
    
    if temp > 10 {
        return 1;  // Cool
    }
    
    return 0;  // Cold
}
```

**Why is this better?**
- Less indentation (easier to read)
- Each condition is clear
- Early returns simplify logic
- No deep nesting

---

## Comparison Operators

### All Comparison Operators

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    let y: u64 = 20;
    
    // Equal to
    if x == 10 {
        // x equals 10
    }
    
    // Not equal to
    if x != y {
        // x is not equal to y
    }
    
    // Less than
    if x < y {
        // x is less than y
    }
    
    // Greater than
    if x > 5 {
        // x is greater than 5
    }
    
    // Less than or equal
    if x <= 10 {
        // x is less than or equal to 10
    }
    
    // Greater than or equal
    if y >= 20 {
        // y is greater than or equal to 20
    }
    
    return 0;
}
```

### Comparison Operator Table

| Operator | Meaning | Example | Result |
|----------|---------|---------|--------|
| `==` | Equal to | `10 == 10` | 1 (true) |
| `!=` | Not equal to | `10 != 5` | 1 (true) |
| `<` | Less than | `5 < 10` | 1 (true) |
| `>` | Greater than | `10 > 5` | 1 (true) |
| `<=` | Less than or equal | `10 <= 10` | 1 (true) |
| `>=` | Greater than or equal | `10 >= 5` | 1 (true) |

### Comparison Examples

```lwanga
fn test_comparisons() -> u64 {
    let a: u64 = 10;
    let b: u64 = 20;
    
    if a == b {
        return 1;  // Not executed (10 != 20)
    }
    
    if a != b {
        return 2;  // Executed! (10 != 20)
    }
    
    return 0;
}

fn main() -> u64 {
    return test_comparisons();  // Returns 2
}
```

---

## Logical Operators

### AND Operator (&&)

Both conditions must be true:

```lwanga
fn main() -> u64 {
    let age: u64 = 25;
    let has_license: u64 = 1;
    
    if age >= 18 && has_license == 1 {
        return 1;  // Can drive
    }
    
    return 0;  // Cannot drive
}
```

**Truth table for AND:**

| A | B | A && B |
|---|---|--------|
| 0 | 0 | 0 |
| 0 | 1 | 0 |
| 1 | 0 | 0 |
| 1 | 1 | 1 |

**Both must be true for result to be true.**

### OR Operator (||)

At least one condition must be true:

```lwanga
fn main() -> u64 {
    let is_weekend: u64 = 1;
    let is_holiday: u64 = 0;
    
    if is_weekend == 1 || is_holiday == 1 {
        return 1;  // Day off!
    }
    
    return 0;  // Work day
}
```

**Truth table for OR:**

| A | B | A \|\| B |
|---|---|----------|
| 0 | 0 | 0 |
| 0 | 1 | 1 |
| 1 | 0 | 1 |
| 1 | 1 | 1 |

**At least one must be true for result to be true.**

### NOT Operator (!)

Inverts the condition:

```lwanga
fn main() -> u64 {
    let is_raining: u64 = 0;
    
    if !(is_raining == 1) {
        return 1;  // Not raining, go outside!
    }
    
    return 0;
}
```

**Truth table for NOT:**

| A | !A |
|---|-----|
| 0 | 1 |
| 1 | 0 |

### Combining Logical Operators

```lwanga
fn can_vote(age: u64, is_citizen: u64, is_registered: u64) -> u64 {
    if age >= 18 && is_citizen == 1 && is_registered == 1 {
        return 1;  // Can vote
    }
    
    return 0;  // Cannot vote
}

fn main() -> u64 {
    let result: u64 = can_vote(25, 1, 1);
    return result;  // Returns 1
}
```

### Complex Conditions

```lwanga
fn check_access(age: u64, is_member: u64, has_ticket: u64) -> u64 {
    // Can enter if: (age >= 18 AND member) OR has ticket
    if (age >= 18 && is_member == 1) || has_ticket == 1 {
        return 1;  // Access granted
    }
    
    return 0;  // Access denied
}

fn main() -> u64 {
    let r1: u64 = check_access(25, 1, 0);  // 1 (adult member)
    let r2: u64 = check_access(16, 0, 1);  // 1 (has ticket)
    let r3: u64 = check_access(16, 0, 0);  // 0 (denied)
    
    return r1;
}
```

**Use parentheses to make complex conditions clear!**

---

## Nested Conditions

### Basic Nesting

```lwanga
fn check_grade(score: u64) -> u64 {
    if score >= 50 {
        if score >= 70 {
            if score >= 90 {
                return 4;  // A
            }
            return 3;  // B
        }
        return 2;  // C
    }
    return 1;  // F
}

fn main() -> u64 {
    return check_grade(85);  // Returns 3 (B)
}
```

### Better: Using Early Returns

```lwanga
fn check_grade(score: u64) -> u64 {
    if score >= 90 {
        return 4;  // A
    }
    
    if score >= 70 {
        return 3;  // B
    }
    
    if score >= 50 {
        return 2;  // C
    }
    
    return 1;  // F
}
```

**Why is this better?**
- Easier to read (no deep nesting)
- Each condition is independent
- Clear flow from top to bottom
- Easier to modify

---

## Practical Examples

### Example 1: Check if Number is Positive, Negative, or Zero

```lwanga
fn check_sign(n: u64) -> u64 {
    if n > 0 {
        return 1;  // Positive
    }
    
    if n == 0 {
        return 0;  // Zero
    }
    
    // Note: u64 can't be negative, but this shows the pattern
    return 2;  // Would be negative
}

fn main() -> u64 {
    return check_sign(42);  // Returns 1
}
```

### Example 2: Leap Year Check

```lwanga
fn is_leap_year(year: u64) -> u64 {
    // Divisible by 4
    if year % 4 != 0 {
        return 0;  // Not a leap year
    }
    
    // Divisible by 100
    if year % 100 != 0 {
        return 1;  // Leap year
    }
    
    // Divisible by 400
    if year % 400 == 0 {
        return 1;  // Leap year
    }
    
    return 0;  // Not a leap year
}

fn main() -> u64 {
    let is_2024_leap: u64 = is_leap_year(2024);
    return is_2024_leap;  // Returns 1 (yes)
}
```

### Example 3: Grade Calculator

```lwanga
fn calculate_grade(score: u64) -> u64 {
    if score > 100 {
        return 0;  // Invalid score
    }
    
    if score >= 90 {
        return 5;  // A
    }
    
    if score >= 80 {
        return 4;  // B
    }
    
    if score >= 70 {
        return 3;  // C
    }
    
    if score >= 60 {
        return 2;  // D
    }
    
    return 1;  // F
}

fn main() -> u64 {
    return calculate_grade(85);  // Returns 4 (B)
}
```

### Example 4: Password Strength Checker

```lwanga
fn check_password_strength(length: u64, has_numbers: u64, has_special: u64) -> u64 {
    if length < 8 {
        return 1;  // Weak
    }
    
    if length >= 8 && has_numbers == 1 {
        return 2;  // Medium
    }
    
    if length >= 12 && has_numbers == 1 && has_special == 1 {
        return 3;  // Strong
    }
    
    return 1;  // Weak
}

fn main() -> u64 {
    let strength: u64 = check_password_strength(15, 1, 1);
    return strength;  // Returns 3 (Strong)
}
```

### Example 5: Triangle Type Checker

```lwanga
fn triangle_type(a: u64, b: u64, c: u64) -> u64 {
    // Check if valid triangle
    if a + b <= c || a + c <= b || b + c <= a {
        return 0;  // Invalid triangle
    }
    
    // Equilateral (all sides equal)
    if a == b && b == c {
        return 1;
    }
    
    // Isosceles (two sides equal)
    if a == b || b == c || a == c {
        return 2;
    }
    
    // Scalene (all sides different)
    return 3;
}

fn main() -> u64 {
    return triangle_type(5, 5, 5);  // Returns 1 (Equilateral)
}
```

---

## Common Mistakes

### Mistake 1: Using = Instead of ==

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x = 10 {  // ERROR: assignment, not comparison
        return 1;
    }
    
    return 0;
}
```

**Fix:**
```lwanga
if x == 10 {  // ✓ Correct: comparison
    return 1;
}
```

### Mistake 2: Forgetting Braces

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5
        return 1;  // ERROR: missing braces
    
    return 0;
}
```

**Fix:**
```lwanga
if x > 5 {  // ✓ Correct: with braces
    return 1;
}
```

### Mistake 3: Wrong Operator Precedence

```lwanga
fn main() -> u64 {
    let x: u64 = 5;
    let y: u64 = 10;
    
    // Intended: (x > 3) AND (y < 20)
    if x > 3 && y < 20 {  // ✓ Correct
        return 1;
    }
    
    return 0;
}
```

**Use parentheses when in doubt:**
```lwanga
if (x > 3) && (y < 20) {  // Even clearer
    return 1;
}
```

### Mistake 4: Unreachable Code

```lwanga
fn main() -> u64 {
    let x: u64 = 10;
    
    if x > 5 {
        return 1;
    }
    
    if x > 3 {  // This will never execute!
        return 2;
    }
    
    return 0;
}
```

**Why?** If `x > 5` is true, we return immediately. The second condition never gets checked.

---

## Exercises

### Exercise 1: Even or Odd

Write a function that returns 1 if a number is even, 0 if odd.

<details>
<summary>Solution</summary>

```lwanga
fn is_even(n: u64) -> u64 {
    if n % 2 == 0 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    return is_even(42);  // Returns 1
}
```
</details>

### Exercise 2: Maximum of Three

Write a function that returns the largest of three numbers.

<details>
<summary>Solution</summary>

```lwanga
fn max_of_three(a: u64, b: u64, c: u64) -> u64 {
    if a >= b && a >= c {
        return a;
    }
    
    if b >= a && b >= c {
        return b;
    }
    
    return c;
}

fn main() -> u64 {
    return max_of_three(10, 25, 15);  // Returns 25
}
```
</details>

### Exercise 3: Age Category

Write a function that returns:
- 1 for child (0-12)
- 2 for teenager (13-19)
- 3 for adult (20-64)
- 4 for senior (65+)

<details>
<summary>Solution</summary>

```lwanga
fn age_category(age: u64) -> u64 {
    if age <= 12 {
        return 1;  // Child
    }
    
    if age <= 19 {
        return 2;  // Teenager
    }
    
    if age <= 64 {
        return 3;  // Adult
    }
    
    return 4;  // Senior
}

fn main() -> u64 {
    return age_category(25);  // Returns 3
}
```
</details>

### Exercise 4: Divisibility Check

Write a function that returns 1 if a number is divisible by both 3 and 5, 0 otherwise.

<details>
<summary>Solution</summary>

```lwanga
fn divisible_by_3_and_5(n: u64) -> u64 {
    if n % 3 == 0 && n % 5 == 0 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    return divisible_by_3_and_5(15);  // Returns 1
}
```
</details>

### Exercise 5: Temperature Warning

Write a function that returns:
- 0 for safe temperature (10-30)
- 1 for too cold (< 10)
- 2 for too hot (> 30)

<details>
<summary>Solution</summary>

```lwanga
fn temperature_warning(temp: u64) -> u64 {
    if temp < 10 {
        return 1;  // Too cold
    }
    
    if temp > 30 {
        return 2;  // Too hot
    }
    
    return 0;  // Safe
}

fn main() -> u64 {
    return temperature_warning(25);  // Returns 0
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to use if statements to make decisions
- ✅ If-else for alternative paths
- ✅ All comparison operators (==, !=, <, >, <=, >=)
- ✅ Logical operators (&&, ||, !)
- ✅ How to combine conditions
- ✅ Nested conditions and why to avoid deep nesting
- ✅ Early returns for cleaner code
- ✅ Common mistakes and how to fix them
- ✅ Practical examples and exercises

---

## Next Lesson

Now that you can make decisions, let's learn how to repeat code using loops in [Lesson 8: Loops](08-loops.md).

---

**Navigation**: [← Previous: Functions](06-functions.md) | [Tutorial Home](README.md) | [Next: Loops →](08-loops.md)
