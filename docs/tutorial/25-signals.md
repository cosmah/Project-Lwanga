# Lesson 25: Signal Handling

**In this lesson, you'll learn about Unix signals, how to send and handle them, and use them for inter-process communication and program control.**

---

## What are Signals?

Signals are software interrupts that notify a process of events. Think of them as notifications from the OS or other processes.

**Common uses:**
- Graceful shutdown (SIGTERM)
- Interrupt programs (SIGINT - Ctrl+C)
- Kill processes (SIGKILL)
- Reload configuration (SIGHUP)
- Handle errors (SIGSEGV, SIGFPE)

---

## Signal Numbers

| Signal | Number | Default Action | Description |
|--------|--------|----------------|-------------|
| SIGHUP | 1 | Terminate | Hangup (terminal closed) |
| SIGINT | 2 | Terminate | Interrupt (Ctrl+C) |
| SIGQUIT | 3 | Core dump | Quit (Ctrl+\\) |
| SIGILL | 4 | Core dump | Illegal instruction |
| SIGTRAP | 5 | Core dump | Trace/breakpoint trap |
| SIGABRT | 6 | Core dump | Abort signal |
| SIGFPE | 8 | Core dump | Floating point exception |
| SIGKILL | 9 | Terminate | Kill (cannot be caught) |
| SIGSEGV | 11 | Core dump | Segmentation fault |
| SIGPIPE | 13 | Terminate | Broken pipe |
| SIGALRM | 14 | Terminate | Timer signal |
| SIGTERM | 15 | Terminate | Termination signal |
| SIGCHLD | 17 | Ignore | Child stopped or terminated |
| SIGCONT | 18 | Continue | Continue if stopped |
| SIGSTOP | 19 | Stop | Stop process (cannot be caught) |

---

## Sending Signals

The `kill` syscall sends signals to processes:

```lwanga
const SYS_KILL: u64 = 62;

// Signal constants
const SIGHUP: u64 = 1;
const SIGINT: u64 = 2;
const SIGQUIT: u64 = 3;
const SIGKILL: u64 = 9;
const SIGTERM: u64 = 15;

fn send_signal(pid: u64, sig: u64) -> u64 {
    unsafe {
        return syscall(SYS_KILL, pid, sig, 0);
    }
}

fn main() -> u64 {
    let target_pid: u64 = 1234;
    
    // Send SIGTERM to process
    send_signal(target_pid, SIGTERM);
    
    return 0;
}
```

**Signature:**
```c
int kill(pid_t pid, int sig);
```

**Special PIDs:**
- `pid > 0` - Send to specific process
- `pid = 0` - Send to all processes in process group
- `pid = -1` - Send to all processes (except init)
- `pid < -1` - Send to process group |pid|

### Sending Signal to Self

```lwanga
const SYS_GETPID: u64 = 39;

fn raise_signal(sig: u64) -> u64 {
    unsafe {
        let my_pid: u64 = syscall(SYS_GETPID, 0, 0, 0);
        return syscall(SYS_KILL, my_pid, sig, 0);
    }
}

fn main() -> u64 {
    // Send SIGTERM to ourselves
    raise_signal(SIGTERM);
    return 0;
}
```

---

## Signal Handlers

Signal handlers are functions that run when a signal is received:

```lwanga
const SYS_RT_SIGACTION: u64 = 13;

packed struct sigaction {
    sa_handler: ptr,     // Handler function
    sa_flags: u64,       // Flags
    sa_restorer: ptr,    // Restorer function
    sa_mask: u64,        // Signal mask
}

fn signal_handler() -> u64 {
    let msg: ptr = "Signal received!\n";
    unsafe {
        syscall(1, 1, msg, 17);
    }
    return 0;
}

fn setup_handler(sig: u64, handler: ptr) -> u64 {
    let action: sigaction = sigaction {
        sa_handler: handler,
        sa_flags: 0,
        sa_restorer: 0 as ptr,
        sa_mask: 0,
    };
    
    unsafe {
        return syscall(SYS_RT_SIGACTION, sig, &action, 0, 8);
    }
}

fn main() -> u64 {
    // Setup handler for SIGINT (Ctrl+C)
    setup_handler(SIGINT, signal_handler as ptr);
    
    // Wait for signal
    unsafe {
        syscall(162, 0, 0, 0);  // sys_nanosleep - sleep forever
    }
    
    return 0;
}
```

---

## Practical Examples

### Example 1: Graceful Shutdown

```lwanga
const SYS_RT_SIGACTION: u64 = 13;
const SYS_WRITE: u64 = 1;
const SYS_EXIT: u64 = 60;
const SIGTERM: u64 = 15;
const SIGINT: u64 = 2;
const STDOUT: u64 = 1;

let mut should_exit: u64 = 0;

fn shutdown_handler() -> u64 {
    let msg: ptr = "\nShutting down gracefully...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 29);
    }
    should_exit = 1;
    return 0;
}

fn main() -> u64 {
    // Setup handlers
    setup_handler(SIGTERM, shutdown_handler as ptr);
    setup_handler(SIGINT, shutdown_handler as ptr);
    
    let msg: ptr = "Running... Press Ctrl+C to stop\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 33);
    }
    
    // Main loop
    while should_exit == 0 {
        // Do work...
        unsafe {
            syscall(35, 1, 0, 0);  // sys_nanosleep - sleep 1 second
        }
    }
    
    let bye: ptr = "Cleanup complete. Goodbye!\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, bye, 27);
    }
    
    return 0;
}
```

### Example 2: Child Process Monitoring

```lwanga
const SIGCHLD: u64 = 17;
const SYS_FORK: u64 = 57;
const SYS_WAIT4: u64 = 61;

fn child_handler() -> u64 {
    let msg: ptr = "Child process exited\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 21);
        
        // Reap zombie
        let mut status: u64 = 0;
        syscall(SYS_WAIT4, -1, &status, 1, 0);  // WNOHANG = 1
    }
    return 0;
}

fn main() -> u64 {
    setup_handler(SIGCHLD, child_handler as ptr);
    
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child exits immediately
            syscall(SYS_EXIT, 0, 0, 0);
        }
        
        // Parent waits
        syscall(162, 2, 0, 0);  // Sleep 2 seconds
    }
    
    return 0;
}
```

### Example 3: Alarm Timer

```lwanga
const SYS_ALARM: u64 = 37;
const SIGALRM: u64 = 14;

fn alarm_handler() -> u64 {
    let msg: ptr = "Time's up!\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 11);
    }
    return 0;
}

fn main() -> u64 {
    setup_handler(SIGALRM, alarm_handler as ptr);
    
    // Set alarm for 5 seconds
    unsafe {
        syscall(SYS_ALARM, 5, 0, 0);
    }
    
    let msg: ptr = "Waiting for alarm...\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 21);
        
        // Wait
        syscall(162, 10, 0, 0);  // Sleep 10 seconds
    }
    
    return 0;
}
```

### Example 4: Signal Blocking

```lwanga
const SYS_RT_SIGPROCMASK: u64 = 14;
const SIG_BLOCK: u64 = 0;
const SIG_UNBLOCK: u64 = 1;

fn block_signal(sig: u64) -> u64 {
    let mask: u64 = 1 << (sig - 1);
    
    unsafe {
        return syscall(SYS_RT_SIGPROCMASK, SIG_BLOCK, &mask, 0, 8);
    }
}

fn unblock_signal(sig: u64) -> u64 {
    let mask: u64 = 1 << (sig - 1);
    
    unsafe {
        return syscall(SYS_RT_SIGPROCMASK, SIG_UNBLOCK, &mask, 0, 8);
    }
}

fn main() -> u64 {
    // Block SIGINT temporarily
    block_signal(SIGINT);
    
    let msg: ptr = "SIGINT blocked - Ctrl+C won't work\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 36);
        syscall(162, 3, 0, 0);  // Sleep 3 seconds
    }
    
    // Unblock SIGINT
    unblock_signal(SIGINT);
    
    let msg2: ptr = "SIGINT unblocked - Ctrl+C works now\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg2, 37);
        syscall(162, 10, 0, 0);  // Sleep 10 seconds
    }
    
    return 0;
}
```

---

## Common Mistakes

### Mistake 1: Not Handling SIGCHLD

```lwanga
// WRONG: Forking without handling SIGCHLD creates zombies
unsafe {
    let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
    
    if pid > 0 {
        // Parent doesn't wait - child becomes zombie!
    }
}
```

**Fix:**
```lwanga
setup_handler(SIGCHLD, child_handler as ptr);
```

### Mistake 2: Unsafe Operations in Handler

```lwanga
// WRONG: Complex operations in signal handler
fn bad_handler() -> u64 {
    // Allocating memory, complex I/O - dangerous!
    return 0;
}
```

**Fix:** Keep handlers simple and fast:
```lwanga
fn good_handler() -> u64 {
    should_exit = 1;  // Just set a flag
    return 0;
}
```

### Mistake 3: Ignoring Signal Return Values

```lwanga
// WRONG: Not checking if signal was sent
send_signal(pid, SIGTERM);
```

**Fix:**
```lwanga
let result: u64 = send_signal(pid, SIGTERM);

if result > 0xFFFFFFFFFFFF0000 {
    // Handle error
}
```

---

## Exercises

### Exercise 1: Timeout Implementation

Implement a function that runs code with a timeout using SIGALRM.

<details>
<summary>Solution</summary>

```lwanga
let mut timed_out: u64 = 0;

fn timeout_handler() -> u64 {
    timed_out = 1;
    return 0;
}

fn run_with_timeout(seconds: u64) -> u64 {
    setup_handler(SIGALRM, timeout_handler as ptr);
    
    unsafe {
        syscall(SYS_ALARM, seconds, 0, 0);
    }
    
    // Do work
    while timed_out == 0 {
        // Work here...
    }
    
    if timed_out == 1 {
        return 1;  // Timed out
    }
    
    return 0;  // Completed
}

fn main() -> u64 {
    return run_with_timeout(5);
}
```
</details>

### Exercise 2: Signal Counter

Count how many times SIGUSR1 is received.

<details>
<summary>Solution</summary>

```lwanga
const SIGUSR1: u64 = 10;

let mut signal_count: u64 = 0;

fn counter_handler() -> u64 {
    signal_count = signal_count + 1;
    return 0;
}

fn main() -> u64 {
    setup_handler(SIGUSR1, counter_handler as ptr);
    
    unsafe {
        let my_pid: u64 = syscall(SYS_GETPID, 0, 0, 0);
        
        // Send signal to self 5 times
        let mut i: u64 = 0;
        while i < 5 {
            syscall(SYS_KILL, my_pid, SIGUSR1, 0);
            i = i + 1;
        }
        
        syscall(162, 1, 0, 0);  // Sleep to let handlers run
    }
    
    return signal_count;  // Returns 5
}
```
</details>

### Exercise 3: Process Terminator

Write a function that sends SIGTERM, waits, then sends SIGKILL if needed.

<details>
<summary>Solution</summary>

```lwanga
fn terminate_process(pid: u64) -> u64 {
    unsafe {
        // Send SIGTERM
        syscall(SYS_KILL, pid, SIGTERM, 0);
        
        // Wait 5 seconds
        syscall(162, 5, 0, 0);
        
        // Check if still alive (send signal 0)
        let result: u64 = syscall(SYS_KILL, pid, 0, 0);
        
        if result == 0 {
            // Still alive - send SIGKILL
            syscall(SYS_KILL, pid, SIGKILL, 0);
            return 1;  // Had to force kill
        }
        
        return 0;  // Terminated gracefully
    }
}

fn main() -> u64 {
    let target: u64 = 1234;
    return terminate_process(target);
}
```
</details>

### Exercise 4: Signal Logger

Log all received signals to a buffer.

<details>
<summary>Solution</summary>

```lwanga
let mut log_buffer: [u64; 100] = [0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                   0, 0, 0, 0, 0, 0, 0, 0, 0, 0];
let mut log_index: u64 = 0;

fn log_handler() -> u64 {
    if log_index < 100 {
        log_buffer[log_index] = SIGINT;  // Log signal number
        log_index = log_index + 1;
    }
    return 0;
}

fn main() -> u64 {
    setup_handler(SIGINT, log_handler as ptr);
    
    // Simulate receiving signals
    unsafe {
        let my_pid: u64 = syscall(SYS_GETPID, 0, 0, 0);
        
        let mut i: u64 = 0;
        while i < 10 {
            syscall(SYS_KILL, my_pid, SIGINT, 0);
            i = i + 1;
        }
        
        syscall(162, 1, 0, 0);
    }
    
    return log_index;  // Returns 10
}
```
</details>

### Exercise 5: Watchdog Timer

Implement a watchdog that terminates if not reset within timeout.

<details>
<summary>Solution</summary>

```lwanga
let mut watchdog_active: u64 = 1;

fn watchdog_handler() -> u64 {
    if watchdog_active == 1 {
        let msg: ptr = "Watchdog timeout - terminating\n";
        unsafe {
            syscall(SYS_WRITE, STDOUT, msg, 31);
            syscall(SYS_EXIT, 1, 0, 0);
        }
    }
    return 0;
}

fn reset_watchdog() -> u64 {
    unsafe {
        syscall(SYS_ALARM, 5, 0, 0);  // Reset to 5 seconds
    }
    return 0;
}

fn main() -> u64 {
    setup_handler(SIGALRM, watchdog_handler as ptr);
    reset_watchdog();
    
    // Main loop
    let mut i: u64 = 0;
    while i < 10 {
        // Do work
        unsafe {
            syscall(162, 1, 0, 0);  // Sleep 1 second
        }
        
        // Reset watchdog every iteration
        reset_watchdog();
        
        i = i + 1;
    }
    
    watchdog_active = 0;
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What signals are and their purposes
- ✅ Common signal numbers and meanings
- ✅ Sending signals with kill()
- ✅ Setting up signal handlers
- ✅ Blocking and unblocking signals
- ✅ Practical examples: graceful shutdown, child monitoring, timers
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand signals, let's learn about inline assembly in [Lesson 26: Inline Assembly](26-inline-assembly.md).

---

**Navigation**: [← Previous: Network Programming](24-network-programming.md) | [Tutorial Home](README.md) | [Next: Inline Assembly →](26-inline-assembly.md)
