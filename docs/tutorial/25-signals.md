# Lesson 25: Signal Handling

**Learn how to handle Unix signals.**

---

## Common Signals

| Signal | Number | Meaning |
|--------|--------|---------|
| SIGHUP | 1 | Hangup |
| SIGINT | 2 | Interrupt (Ctrl+C) |
| SIGQUIT | 3 | Quit |
| SIGKILL | 9 | Kill (cannot be caught) |
| SIGTERM | 15 | Terminate |

---

## Sending Signals

```lwanga
const SYS_KILL: u64 = 62;
const SIGTERM: u32 = 15;

fn send_signal(pid: u64, sig: u32) -> u64 {
    unsafe {
        return syscall(SYS_KILL, pid, sig, 0);
    }
}
```

---

## Signal Handlers

```lwanga
const SYS_RT_SIGACTION: u64 = 13;

fn setup_signal_handler(sig: u32, handler: ptr) -> u64 {
    unsafe {
        return syscall(SYS_RT_SIGACTION, sig, handler, 0);
    }
}
```

---

**Navigation**: [← Previous: Network Programming](24-network-programming.md) | [Tutorial Home](README.md) | [Next: Inline Assembly →](26-inline-assembly.md)
