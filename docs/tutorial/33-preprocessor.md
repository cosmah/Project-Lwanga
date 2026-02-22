# Lesson 33: Preprocessor

**Learn how to use conditional compilation.**

---

## Conditional Compilation

```lwanga
#if DEBUG
const LOG_LEVEL: u64 = 3;
#else
const LOG_LEVEL: u64 = 1;
#endif

fn main() -> u64 {
    return LOG_LEVEL;
}
```

---

## Platform-Specific Code

```lwanga
#if LINUX
const NEWLINE: ptr = "\n";
#elif WINDOWS
const NEWLINE: ptr = "\r\n";
#else
const NEWLINE: ptr = "\n";
#endif
```

---

## Feature Flags

```lwanga
#if FEATURE_NETWORKING
fn network_init() -> u64 {
    return create_socket();
}
#endif

fn main() -> u64 {
    #if FEATURE_NETWORKING
    network_init();
    #endif
    
    return 0;
}
```

---

## Compile-Time Configuration

```bash
lwangac -D DEBUG program.lwanga -o program
lwangac -D FEATURE_NETWORKING program.lwanga -o program
```

---

**Navigation**: [← Previous: Exploit Development](32-exploit-development.md) | [Tutorial Home](README.md) | [Next: Modules →](34-modules.md)
