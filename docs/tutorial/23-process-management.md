# Lesson 23: Process Management

**Learn how to create and manage processes.**

---

## Getting Process ID

```lwanga
const SYS_GETPID: u64 = 39;

fn get_pid() -> u64 {
    let pid: u64 = 0;
    
    unsafe {
        pid = syscall(SYS_GETPID, 0, 0, 0);
    }
    
    return pid;
}
```

---

## Fork (Create Child Process)

```lwanga
const SYS_FORK: u64 = 57;

fn fork_process() -> u64 {
    let pid: u64 = 0;
    
    unsafe {
        pid = syscall(SYS_FORK, 0, 0, 0);
    }
    
    return pid;  // 0 in child, child PID in parent
}
```

---

## Execute Program

```lwanga
const SYS_EXECVE: u64 = 59;

fn exec_program(path: ptr, argv: ptr, envp: ptr) -> u64 {
    unsafe {
        return syscall(SYS_EXECVE, path, argv, envp);
    }
}
```

---

## Wait for Child

```lwanga
const SYS_WAIT4: u64 = 61;

fn wait_for_child(pid: u64) -> u64 {
    let status: u64 = 0;
    
    unsafe {
        syscall(SYS_WAIT4, pid, &status, 0, 0);
    }
    
    return status;
}
```

---

**Navigation**: [← Previous: File Operations](22-file-operations.md) | [Tutorial Home](README.md) | [Next: Network Programming →](24-network-programming.md)
