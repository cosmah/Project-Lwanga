# Lesson 34: Modules and Imports

**Learn how to organize code across multiple files.**

---

## Importing Modules

```lwanga
import "lib/io.lwanga";
import "lib/string.lwanga";
import "utils/math.lwanga";

fn main() -> u64 {
    // Use functions from imported modules
    return 0;
}
```

---

## Creating a Module

File: `lib/io.lwanga`

```lwanga
const STDOUT: u64 = 1;
const SYS_WRITE: u64 = 1;

fn print(msg: ptr, len: u64) -> u64 {
    unsafe {
        return syscall(SYS_WRITE, STDOUT, msg, len);
    }
}

fn println(msg: ptr, len: u64) -> u64 {
    print(msg, len);
    print("\n", 1);
    return 0;
}
```

---

## Using the Module

File: `main.lwanga`

```lwanga
import "lib/io.lwanga";

fn main() -> u64 {
    println("Hello from module!", 19);
    return 0;
}
```

---

## Module Organization

```
project/
├── main.lwanga
├── lib/
│   ├── io.lwanga
│   ├── string.lwanga
│   └── net.lwanga
└── utils/
    ├── math.lwanga
    └── crypto.lwanga
```

---

**Navigation**: [← Previous: Preprocessor](33-preprocessor.md) | [Tutorial Home](README.md) | [Next: Optimization →](35-optimization.md)
