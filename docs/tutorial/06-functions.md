# Lesson 6: Functions

**In this lesson, you'll learn how to organize code into reusable functions, pass parameters, and return values.**

---

## What is a Function?

A function is a named block of code that performs a specific task. Functions help you organize code, reuse logic, and make programs more maintainable.

---

## Function Syntax

```lwanga
fn function_name(parameter: type) -> return_type {
    return value;
}
```

---

## Simple Functions

### No Parameters

```lwanga
fn get_answer() -> u64 {
    return 42;
}

fn main() -> u64 {
    let answer: u64 = get_answer();
    return answer;
}
```

### One Parameter

```lwanga
fn double(x: u64) -> u64 {
    return x * 2;
}

fn main() -> u64 {
    return double(21);  // Returns 42
}
```

### Multiple Parameters

```lwanga
fn add(a: u64, b: u64) -> u64 {
    return a + b;
}

fn main() -> u64 {
    return add(10, 20);  // Returns 30
}
```

---

## Practical Examples

```lwanga
fn calculate_area(width: u64, height: u64) -> u64 {
    return width * height;
}

fn is_even(n: u64) -> u64 {
    if n % 2 == 0 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    let area: u64 = calculate_area(10, 20);
    let even: u64 = is_even(42);
    return area;
}
```

---

**Navigation**: [← Previous: Numbers](05-numbers.md) | [Tutorial Home](README.md) | [Next: Conditionals →](07-conditionals.md)
