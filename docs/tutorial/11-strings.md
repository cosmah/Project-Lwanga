# Lesson 11: Working with Strings

**In this lesson, you'll learn how to handle strings, calculate length, compare, copy, and manipulate text in Lwanga.**

---

## String Basics

Strings in Lwanga are null-terminated byte sequences. A string is just a pointer to the first character, and the string ends when you reach a byte with value 0 (null terminator).

```lwanga
fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    // Stored in memory as: H e l l o ,   W o r l d ! \0
    
    return 0;
}
```

**Memory layout:**
```
Address    Value
0x1000     'H' (72)
0x1001     'e' (101)
0x1002     'l' (108)
0x1003     'l' (108)
0x1004     'o' (111)
0x1005     ',' (44)
0x1006     ' ' (32)
0x1007     'W' (87)
0x1008     'o' (111)
0x1009     'r' (114)
0x100A     'l' (108)
0x100B     'd' (100)
0x100C     '!' (33)
0x100D     '\0' (0)  <- Null terminator
```

**Design choice:** Null-terminated strings are simple, efficient, and compatible with C. No length field needed - just scan until you find 0.

---

## String Length

### Basic strlen Implementation

```lwanga
fn strlen(s: ptr) -> u64 {
    let mut len: u64 = 0;
    let mut p: ptr = s;
    
    unsafe {
        while *p != 0 {
            len = len + 1;
            p = p + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let msg: ptr = "Hello";
    return strlen(msg);  // Returns 5
}
```

**How it works:**
1. Start at beginning of string (first character)
2. Check if current byte is 0 (null terminator)
3. If not 0, increment length counter
4. Move to next byte
5. Repeat until null terminator found

### Alternative Implementation

```lwanga
fn strlen_v2(s: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            i = i + 1;
        }
    }
    
    return i;
}
```

**Both versions do the same thing, just different style.**

---

## String Comparison

### Basic strcmp Implementation

```lwanga
fn strcmp(s1: ptr, s2: ptr) -> u64 {
    let mut p1: ptr = s1;
    let mut p2: ptr = s2;
    
    unsafe {
        while *p1 != 0 && *p2 != 0 {
            if *p1 != *p2 {
                return 0;  // Not equal
            }
            p1 = p1 + 1;
            p2 = p2 + 1;
        }
        
        // Check if both reached end
        if *p1 == *p2 {
            return 1;  // Equal
        }
    }
    
    return 0;  // Not equal (different lengths)
}

fn main() -> u64 {
    let str1: ptr = "Hello";
    let str2: ptr = "Hello";
    let str3: ptr = "World";
    
    let r1: u64 = strcmp(str1, str2);  // 1 (equal)
    let r2: u64 = strcmp(str1, str3);  // 0 (not equal)
    
    return r1;
}
```

**How it works:**
1. Compare characters one by one
2. If any character differs, strings are not equal
3. If we reach end of both strings together, they're equal
4. If one string ends before the other, they're not equal

### Case-Insensitive Comparison

```lwanga
fn to_lower(c: u8) -> u8 {
    if c >= 65 && c <= 90 {  // A-Z
        return c + 32;  // Convert to lowercase
    }
    return c;
}

fn strcmp_case_insensitive(s1: ptr, s2: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(s1 + i) != 0 && *(s2 + i) != 0 {
            let c1: u8 = to_lower(*(s1 + i));
            let c2: u8 = to_lower(*(s2 + i));
            
            if c1 != c2 {
                return 0;
            }
            
            i = i + 1;
        }
        
        if *(s1 + i) == *(s2 + i) {
            return 1;
        }
    }
    
    return 0;
}

fn main() -> u64 {
    return strcmp_case_insensitive("Hello", "HELLO");  // Returns 1
}
```

---

## String Copy

### Basic strcpy Implementation

```lwanga
fn strcpy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        *(dest + i) = 0;  // Null terminator
    }
    
    return i;
}

fn main() -> u64 {
    let source: ptr = "Hello";
    let buffer: [u8; 20] = [0; 20];
    
    let len: u64 = strcpy(&buffer, source);
    
    return len;  // Returns 5
}
```

**Warning:** This is unsafe if destination buffer is too small!

### Safe String Copy (with bounds checking)

```lwanga
fn strcpy_safe(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < max_len - 1 && *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        *(dest + i) = 0;  // Always null terminate
    }
    
    return i;
}

fn main() -> u64 {
    let source: ptr = "Hello, World!";
    let buffer: [u8; 10] = [0; 10];
    
    let len: u64 = strcpy_safe(&buffer, source, 10);
    // Only copies first 9 characters + null terminator
    
    return len;
}
```

---

## String Concatenation

```lwanga
fn strcat(dest: ptr, src: ptr) -> u64 {
    // Find end of dest
    let mut i: u64 = 0;
    unsafe {
        while *(dest + i) != 0 {
            i = i + 1;
        }
    }
    
    // Copy src to end of dest
    let mut j: u64 = 0;
    unsafe {
        while *(src + j) != 0 {
            *(dest + i + j) = *(src + j);
            j = j + 1;
        }
        *(dest + i + j) = 0;  // Null terminator
    }
    
    return i + j;
}

fn main() -> u64 {
    let buffer: [u8; 50] = [0; 50];
    
    // Copy "Hello" to buffer
    strcpy(&buffer, "Hello");
    
    // Append " World"
    strcat(&buffer, " World");
    
    // buffer now contains "Hello World"
    
    return strlen(&buffer);  // Returns 11
}
```

---

## String Search

### Find Character

```lwanga
fn strchr(s: ptr, c: u8) -> ptr {
    let mut p: ptr = s;
    
    unsafe {
        while *p != 0 {
            if *p == c {
                return p;  // Found it
            }
            p = p + 1;
        }
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    let msg: ptr = "Hello, World!";
    let pos: ptr = strchr(msg, ',' as u8);
    
    if pos != 0 {
        return 1;  // Found
    }
    
    return 0;
}
```

### Find Substring

```lwanga
fn strstr(haystack: ptr, needle: ptr) -> ptr {
    let needle_len: u64 = strlen(needle);
    
    if needle_len == 0 {
        return haystack;
    }
    
    let mut i: u64 = 0;
    
    unsafe {
        while *(haystack + i) != 0 {
            let mut j: u64 = 0;
            let mut match: u64 = 1;
            
            while j < needle_len {
                if *(haystack + i + j) != *(needle + j) {
                    match = 0;
                    break;
                }
                j = j + 1;
            }
            
            if match == 1 {
                return haystack + i;
            }
            
            i = i + 1;
        }
    }
    
    return 0;  // Not found
}

fn main() -> u64 {
    let text: ptr = "Hello, World!";
    let pattern: ptr = "World";
    let pos: ptr = strstr(text, pattern);
    
    if pos != 0 {
        return 1;  // Found
    }
    
    return 0;
}
```

---

## String Manipulation

### Reverse String

```lwanga
fn str_reverse(s: ptr) -> u64 {
    let len: u64 = strlen(s);
    let mut i: u64 = 0;
    
    unsafe {
        while i < len / 2 {
            let temp: u8 = *(s + i);
            *(s + i) = *(s + len - i - 1);
            *(s + len - i - 1) = temp;
            i = i + 1;
        }
    }
    
    return len;
}

fn main() -> u64 {
    let buffer: [u8; 20] = [0; 20];
    strcpy(&buffer, "Hello");
    
    str_reverse(&buffer);
    // buffer now contains "olleH"
    
    return 0;
}
```

### To Uppercase

```lwanga
fn str_to_upper(s: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            let c: u8 = *(s + i);
            if c >= 97 && c <= 122 {  // a-z
                *(s + i) = c - 32;  // Convert to uppercase
            }
            i = i + 1;
        }
    }
    
    return i;
}

fn main() -> u64 {
    let buffer: [u8; 20] = [0; 20];
    strcpy(&buffer, "hello");
    
    str_to_upper(&buffer);
    // buffer now contains "HELLO"
    
    return 0;
}
```

### To Lowercase

```lwanga
fn str_to_lower(s: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            let c: u8 = *(s + i);
            if c >= 65 && c <= 90 {  // A-Z
                *(s + i) = c + 32;  // Convert to lowercase
            }
            i = i + 1;
        }
    }
    
    return i;
}
```

---

## Printing Strings

### Print to stdout

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print(msg: ptr) -> u64 {
    let len: u64 = strlen(msg);
    
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, len);
    }
    
    return len;
}

fn println(msg: ptr) -> u64 {
    let len: u64 = print(msg);
    print("\n");
    return len;
}

fn main() -> u64 {
    println("Hello, World!");
    println("This is Lwanga!");
    
    return 0;
}
```

---

## Escape Sequences

### Common Escape Sequences

```lwanga
fn test_escapes() -> u64 {
    let newline: ptr = "Line 1\nLine 2";      // Newline
    let tab: ptr = "Col1\tCol2";              // Tab
    let quote: ptr = "He said \"Hello\"";    // Double quote
    let backslash: ptr = "Path: C:\\Users";  // Backslash
    
    println(newline);
    println(tab);
    println(quote);
    println(backslash);
    
    return 0;
}
```

**Escape sequence table:**

| Sequence | Meaning | ASCII Value |
|----------|---------|-------------|
| `\n` | Newline | 10 |
| `\t` | Tab | 9 |
| `\r` | Carriage return | 13 |
| `\"` | Double quote | 34 |
| `\'` | Single quote | 39 |
| `\\` | Backslash | 92 |
| `\0` | Null | 0 |

---

## Practical Examples

### Example 1: Count Words

```lwanga
fn is_space(c: u8) -> u64 {
    if c == 32 || c == 9 || c == 10 {  // space, tab, newline
        return 1;
    }
    return 0;
}

fn count_words(s: ptr) -> u64 {
    let mut count: u64 = 0;
    let mut in_word: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            if is_space(*(s + i)) == 1 {
                in_word = 0;
            } else {
                if in_word == 0 {
                    count = count + 1;
                    in_word = 1;
                }
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    return count_words("Hello World from Lwanga");  // Returns 4
}
```

### Example 2: Trim Whitespace

```lwanga
fn str_trim(s: ptr) -> u64 {
    // Find first non-space
    let mut start: u64 = 0;
    unsafe {
        while *(s + start) == 32 {
            start = start + 1;
        }
    }
    
    // Find last non-space
    let len: u64 = strlen(s);
    let mut end: u64 = len;
    
    unsafe {
        while end > start && *(s + end - 1) == 32 {
            end = end - 1;
        }
    }
    
    // Move trimmed string to beginning
    let mut i: u64 = 0;
    unsafe {
        while start + i < end {
            *(s + i) = *(s + start + i);
            i = i + 1;
        }
        *(s + i) = 0;
    }
    
    return i;
}
```

### Example 3: String Replace

```lwanga
fn str_replace_char(s: ptr, old: u8, new: u8) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            if *(s + i) == old {
                *(s + i) = new;
                count = count + 1;
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    let buffer: [u8; 20] = [0; 20];
    strcpy(&buffer, "Hello World");
    
    let count: u64 = str_replace_char(&buffer, 'o' as u8, '0' as u8);
    // buffer now contains "Hell0 W0rld"
    
    return count;  // Returns 2
}
```

### Example 4: Check Palindrome

```lwanga
fn is_palindrome(s: ptr) -> u64 {
    let len: u64 = strlen(s);
    let mut i: u64 = 0;
    
    unsafe {
        while i < len / 2 {
            if *(s + i) != *(s + len - i - 1) {
                return 0;  // Not a palindrome
            }
            i = i + 1;
        }
    }
    
    return 1;  // Is a palindrome
}

fn main() -> u64 {
    let r1: u64 = is_palindrome("racecar");  // 1
    let r2: u64 = is_palindrome("hello");    // 0
    
    return r1;
}
```

### Example 5: String to Number

```lwanga
fn str_to_num(s: ptr) -> u64 {
    let mut result: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            let c: u8 = *(s + i);
            
            if c >= 48 && c <= 57 {  // '0' to '9'
                result = result * 10 + (c - 48) as u64;
            } else {
                break;  // Stop at non-digit
            }
            
            i = i + 1;
        }
    }
    
    return result;
}

fn main() -> u64 {
    return str_to_num("12345");  // Returns 12345
}
```

---

## Common Mistakes

### Mistake 1: Buffer Overflow

```lwanga
// Bad: No bounds checking
fn bad_copy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while *(src + i) != 0 {
            *(dest + i) = *(src + i);  // Could overflow dest!
            i = i + 1;
        }
    }
    return i;
}
```

**Fix:** Always check bounds
```lwanga
fn safe_copy(dest: ptr, src: ptr, max_len: u64) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while i < max_len - 1 && *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        *(dest + i) = 0;
    }
    return i;
}
```

### Mistake 2: Forgetting Null Terminator

```lwanga
// Bad: No null terminator
fn bad_copy(dest: ptr, src: ptr) -> u64 {
    let mut i: u64 = 0;
    unsafe {
        while *(src + i) != 0 {
            *(dest + i) = *(src + i);
            i = i + 1;
        }
        // Missing: *(dest + i) = 0;
    }
    return i;
}
```

### Mistake 3: Modifying String Literals

```lwanga
// Dangerous: String literals are read-only
fn main() -> u64 {
    let msg: ptr = "Hello";
    unsafe {
        *msg = 'h' as u8;  // May crash!
    }
    return 0;
}
```

---

## Exercises

### Exercise 1: Count Vowels

Write a function that counts vowels in a string.

<details>
<summary>Solution</summary>

```lwanga
fn is_vowel(c: u8) -> u64 {
    if c == 'a' as u8 || c == 'e' as u8 || c == 'i' as u8 || 
       c == 'o' as u8 || c == 'u' as u8 ||
       c == 'A' as u8 || c == 'E' as u8 || c == 'I' as u8 || 
       c == 'O' as u8 || c == 'U' as u8 {
        return 1;
    }
    return 0;
}

fn count_vowels(s: ptr) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            if is_vowel(*(s + i)) == 1 {
                count = count + 1;
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    return count_vowels("Hello World");  // Returns 3
}
```
</details>

### Exercise 2: Remove Spaces

Write a function that removes all spaces from a string.

<details>
<summary>Solution</summary>

```lwanga
fn remove_spaces(s: ptr) -> u64 {
    let mut i: u64 = 0;
    let mut j: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            if *(s + i) != 32 {  // Not a space
                *(s + j) = *(s + i);
                j = j + 1;
            }
            i = i + 1;
        }
        *(s + j) = 0;
    }
    
    return j;
}

fn main() -> u64 {
    let buffer: [u8; 20] = [0; 20];
    strcpy(&buffer, "Hello World");
    
    let len: u64 = remove_spaces(&buffer);
    // buffer now contains "HelloWorld"
    
    return len;  // Returns 10
}
```
</details>

### Exercise 3: String Starts With

Write a function that checks if a string starts with a prefix.

<details>
<summary>Solution</summary>

```lwanga
fn starts_with(s: ptr, prefix: ptr) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while *(prefix + i) != 0 {
            if *(s + i) != *(prefix + i) {
                return 0;
            }
            i = i + 1;
        }
    }
    
    return 1;
}

fn main() -> u64 {
    return starts_with("Hello World", "Hello");  // Returns 1
}
```
</details>

### Exercise 4: Count Character Occurrences

Write a function that counts how many times a character appears.

<details>
<summary>Solution</summary>

```lwanga
fn count_char(s: ptr, c: u8) -> u64 {
    let mut count: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while *(s + i) != 0 {
            if *(s + i) == c {
                count = count + 1;
            }
            i = i + 1;
        }
    }
    
    return count;
}

fn main() -> u64 {
    return count_char("Hello", 'l' as u8);  // Returns 2
}
```
</details>

### Exercise 5: String Ends With

Write a function that checks if a string ends with a suffix.

<details>
<summary>Solution</summary>

```lwanga
fn ends_with(s: ptr, suffix: ptr) -> u64 {
    let s_len: u64 = strlen(s);
    let suffix_len: u64 = strlen(suffix);
    
    if suffix_len > s_len {
        return 0;
    }
    
    let start: u64 = s_len - suffix_len;
    let mut i: u64 = 0;
    
    unsafe {
        while i < suffix_len {
            if *(s + start + i) != *(suffix + i) {
                return 0;
            }
            i = i + 1;
        }
    }
    
    return 1;
}

fn main() -> u64 {
    return ends_with("Hello World", "World");  // Returns 1
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How strings are stored in memory
- ✅ Null-terminated string format
- ✅ String length calculation
- ✅ String comparison and searching
- ✅ String copying and concatenation
- ✅ String manipulation (reverse, case conversion)
- ✅ Escape sequences
- ✅ Safe string handling with bounds checking
- ✅ Common string operations and patterns

---

## Next Lesson

Now that you can work with strings, let's learn about arrays in [Lesson 12: Arrays](12-arrays.md).

---

**Navigation**: [← Previous: Pointers](10-pointers.md) | [Tutorial Home](README.md) | [Next: Arrays →](12-arrays.md)
