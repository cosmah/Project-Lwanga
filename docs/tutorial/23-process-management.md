# Lesson 23: Process Management

**In this lesson, you'll learn how to create and manage processes using system calls: getting process IDs, forking, executing programs, and waiting for child processes.**

---

## Understanding Processes

A process is a running instance of a program. Each process has:
- **Process ID (PID)** - Unique identifier
- **Parent Process ID (PPID)** - ID of parent process
- **Memory space** - Code, data, stack, heap
- **File descriptors** - Open files, sockets, pipes
- **Environment variables** - Configuration data

**Why manage processes?**
- Run multiple tasks concurrently
- Execute external programs
- Build shells and process managers
- Implement daemons and services
- Create parallel processing systems

---

## Getting Process ID

Every process has a unique PID:

```lwanga
const SYS_GETPID: u64 = 39;

fn get_pid() -> u64 {
    unsafe {
        return syscall(SYS_GETPID, 0, 0, 0);
    }
}

fn main() -> u64 {
    let my_pid: u64 = get_pid();
    return my_pid;
}
```

**Use cases:**
- Logging (include PID in log messages)
- Lock files (prevent multiple instances)
- IPC (inter-process communication)
- Debugging

### Getting Parent Process ID

```lwanga
const SYS_GETPPID: u64 = 110;

fn get_parent_pid() -> u64 {
    unsafe {
        return syscall(SYS_GETPPID, 0, 0, 0);
    }
}

fn main() -> u64 {
    let my_pid: u64 = get_pid();
    let parent_pid: u64 = get_parent_pid();
    
    return parent_pid;
}
```

---

## Fork - Creating Child Processes

The `fork` syscall creates a copy of the current process:

```lwanga
const SYS_FORK: u64 = 57;

fn fork_process() -> u64 {
    unsafe {
        return syscall(SYS_FORK, 0, 0, 0);
    }
}

fn main() -> u64 {
    let pid: u64 = fork_process();
    
    if pid == 0 {
        // Child process
        return 100;
    } else {
        // Parent process
        return 200;
    }
}
```

**How fork works:**

1. Parent calls fork()
2. Kernel creates child process (copy of parent)
3. Both processes continue from fork() return
4. fork() returns 0 in child, child's PID in parent

**Memory after fork:**
- Child gets copy of parent's memory
- Copy-on-write optimization (shared until modified)
- File descriptors are duplicated
- Both processes run independently

### Fork Example: Parent and Child

```lwanga
const SYS_FORK: u64 = 57;
const SYS_WRITE: u64 = 1;
const SYS_GETPID: u64 = 39;
const STDOUT: u64 = 1;

fn main() -> u64 {
    let pid: u64 = 0;
    
    unsafe {
        pid = syscall(SYS_FORK, 0, 0, 0);
    }
    
    if pid == 0 {
        // Child process
        let msg: ptr = "Child process\n";
        unsafe {
            syscall(SYS_WRITE, STDOUT, msg, 14);
        }
        return 0;
    } else {
        // Parent process
        let msg: ptr = "Parent process\n";
        unsafe {
            syscall(SYS_WRITE, STDOUT, msg, 15);
        }
        return 0;
    }
}
```

---

## Executing Programs

The `execve` syscall replaces the current process with a new program:

```lwanga
const SYS_EXECVE: u64 = 59;

fn exec_program(path: ptr, argv: ptr, envp: ptr) -> u64 {
    unsafe {
        return syscall(SYS_EXECVE, path, argv, envp);
    }
}

fn main() -> u64 {
    let program: ptr = "/bin/ls";
    let args: [ptr; 2] = [program, 0 as ptr];
    let env: [ptr; 1] = [0 as ptr];
    
    exec_program(program, &args, &env);
    
    // Never reached if exec succeeds
    return 1;
}
```

**Arguments:**
- `path`: Path to executable
- `argv`: Array of argument strings (NULL-terminated)
- `envp`: Array of environment strings (NULL-terminated)

**What happens:**
1. Kernel loads new program
2. Replaces current process memory
3. Starts execution at new program's entry point
4. If successful, never returns
5. If fails, returns -1

### Fork + Exec Pattern

Common pattern: fork then exec in child:

```lwanga
const SYS_FORK: u64 = 57;
const SYS_EXECVE: u64 = 59;
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn run_program(path: ptr) -> u64 {
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child: execute program
            let args: [ptr; 2] = [path, 0 as ptr];
            let env: [ptr; 1] = [0 as ptr];
            
            syscall(SYS_EXECVE, path, &args, &env);
            
            // If we get here, exec failed
            let err: ptr = "Exec failed\n";
            syscall(SYS_WRITE, STDOUT, err, 12);
            return 1;
        } else {
            // Parent: continue
            return pid;
        }
    }
}

fn main() -> u64 {
    let program: ptr = "/bin/echo";
    let child_pid: u64 = run_program(program);
    return child_pid;
}
```

---

## Waiting for Child Processes

The `wait4` syscall waits for a child to terminate:

```lwanga
const SYS_WAIT4: u64 = 61;

fn wait_for_child(pid: u64) -> u64 {
    let mut status: u64 = 0;
    
    unsafe {
        syscall(SYS_WAIT4, pid, &status, 0, 0);
    }
    
    return status;
}

fn main() -> u64 {
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child
            return 42;
        } else {
            // Parent: wait for child
            let status: u64 = wait_for_child(pid);
            return status;
        }
    }
}
```

**Why wait?**
- Get child's exit status
- Prevent zombie processes
- Synchronize parent and child
- Clean up child resources

### Wait Options

```lwanga
const WNOHANG: u64 = 1;    // Don't block if no child has exited
const WUNTRACED: u64 = 2;  // Report stopped children

fn wait_nonblocking(pid: u64) -> u64 {
    let mut status: u64 = 0;
    
    unsafe {
        let result: u64 = syscall(SYS_WAIT4, pid, &status, WNOHANG, 0);
        
        if result == 0 {
            return 0;  // Child still running
        }
        
        return status;
    }
}
```

---

## Process Exit

Terminate the current process:

```lwanga
const SYS_EXIT: u64 = 60;

fn exit_process(code: u64) -> u64 {
    unsafe {
        syscall(SYS_EXIT, code, 0, 0);
    }
    return 0;  // Never reached
}

fn main() -> u64 {
    let msg: ptr = "Exiting...\n";
    
    unsafe {
        syscall(1, 1, msg, 11);
    }
    
    exit_process(0);
    return 0;
}
```

---

## Practical Examples

### Example 1: Simple Shell

```lwanga
const SYS_FORK: u64 = 57;
const SYS_EXECVE: u64 = 59;
const SYS_WAIT4: u64 = 61;
const SYS_WRITE: u64 = 1;
const STDOUT: u64 = 1;

fn execute_command(cmd: ptr) -> u64 {
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child: execute command
            let args: [ptr; 2] = [cmd, 0 as ptr];
            let env: [ptr; 1] = [0 as ptr];
            
            syscall(SYS_EXECVE, cmd, &args, &env);
            
            // Exec failed
            return 1;
        } else {
            // Parent: wait for child
            let mut status: u64 = 0;
            syscall(SYS_WAIT4, pid, &status, 0, 0);
            return 0;
        }
    }
}

fn main() -> u64 {
    let cmd: ptr = "/bin/ls";
    execute_command(cmd);
    
    let msg: ptr = "Command finished\n";
    unsafe {
        syscall(SYS_WRITE, STDOUT, msg, 17);
    }
    
    return 0;
}
```

### Example 2: Parallel Processing

```lwanga
fn spawn_workers(count: u64) -> u64 {
    let mut i: u64 = 0;
    
    unsafe {
        while i < count {
            let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
            
            if pid == 0 {
                // Child worker
                let msg: ptr = "Worker running\n";
                syscall(SYS_WRITE, STDOUT, msg, 15);
                syscall(SYS_EXIT, 0, 0, 0);
            }
            
            i = i + 1;
        }
        
        // Parent: wait for all children
        i = 0;
        while i < count {
            let mut status: u64 = 0;
            syscall(SYS_WAIT4, -1, &status, 0, 0);  // -1 = any child
            i = i + 1;
        }
    }
    
    return 0;
}

fn main() -> u64 {
    return spawn_workers(3);
}
```

### Example 3: Daemon Process

```lwanga
const SYS_FORK: u64 = 57;
const SYS_SETSID: u64 = 112;
const SYS_CHDIR: u64 = 80;
const SYS_CLOSE: u64 = 3;

fn daemonize() -> u64 {
    unsafe {
        // First fork
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid > 0 {
            // Parent exits
            syscall(SYS_EXIT, 0, 0, 0);
        }
        
        // Create new session
        syscall(SYS_SETSID, 0, 0, 0);
        
        // Second fork
        let pid2: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid2 > 0 {
            // First child exits
            syscall(SYS_EXIT, 0, 0, 0);
        }
        
        // Change to root directory
        let root: ptr = "/";
        syscall(SYS_CHDIR, root, 0, 0);
        
        // Close standard file descriptors
        syscall(SYS_CLOSE, 0, 0, 0);  // stdin
        syscall(SYS_CLOSE, 1, 0, 0);  // stdout
        syscall(SYS_CLOSE, 2, 0, 0);  // stderr
        
        // Now running as daemon
        return 0;
    }
}

fn main() -> u64 {
    daemonize();
    
    // Daemon code here
    // (runs in background)
    
    return 0;
}
```

### Example 4: Process Monitor

```lwanga
fn monitor_process(pid: u64) -> u64 {
    unsafe {
        let mut status: u64 = 0;
        
        // Wait for process to exit
        let result: u64 = syscall(SYS_WAIT4, pid, &status, 0, 0);
        
        if result == pid {
            let msg: ptr = "Process exited\n";
            syscall(SYS_WRITE, STDOUT, msg, 15);
            return status;
        }
        
        return 0;
    }
}

fn main() -> u64 {
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child: do some work
            let msg: ptr = "Working...\n";
            syscall(SYS_WRITE, STDOUT, msg, 11);
            return 0;
        } else {
            // Parent: monitor child
            return monitor_process(pid);
        }
    }
}
```

---

## Common Mistakes

### Mistake 1: Not Checking Fork Return

```lwanga
// WRONG: Not checking fork result
unsafe {
    syscall(SYS_FORK, 0, 0, 0);
    // Both parent and child execute this!
}
```

**Fix:**
```lwanga
unsafe {
    let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
    
    if pid == 0 {
        // Child code
    } else {
        // Parent code
    }
}
```

### Mistake 2: Zombie Processes

```lwanga
// WRONG: Not waiting for child
unsafe {
    let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
    
    if pid > 0 {
        // Parent doesn't wait - child becomes zombie!
        return 0;
    }
}
```

**Fix:**
```lwanga
unsafe {
    let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
    
    if pid > 0 {
        let mut status: u64 = 0;
        syscall(SYS_WAIT4, pid, &status, 0, 0);
        return 0;
    }
}
```

### Mistake 3: Forgetting NULL Terminator

```lwanga
// WRONG: argv not NULL-terminated
let args: [ptr; 1] = [program];
syscall(SYS_EXECVE, program, &args, 0);
```

**Fix:**
```lwanga
let args: [ptr; 2] = [program, 0 as ptr];
syscall(SYS_EXECVE, program, &args, 0);
```

---

## Exercises

### Exercise 1: Count Children

Write a function that forks N child processes and counts them.

<details>
<summary>Solution</summary>

```lwanga
const SYS_FORK: u64 = 57;
const SYS_WAIT4: u64 = 61;
const SYS_EXIT: u64 = 60;

fn spawn_children(count: u64) -> u64 {
    let mut i: u64 = 0;
    let mut children: u64 = 0;
    
    unsafe {
        while i < count {
            let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
            
            if pid == 0 {
                // Child exits immediately
                syscall(SYS_EXIT, 0, 0, 0);
            } else {
                children = children + 1;
            }
            
            i = i + 1;
        }
        
        // Wait for all children
        i = 0;
        while i < children {
            let mut status: u64 = 0;
            syscall(SYS_WAIT4, -1, &status, 0, 0);
            i = i + 1;
        }
    }
    
    return children;
}

fn main() -> u64 {
    return spawn_children(5);
}
```
</details>

### Exercise 2: Execute with Arguments

Write a function that executes a program with command-line arguments.

<details>
<summary>Solution</summary>

```lwanga
const SYS_FORK: u64 = 57;
const SYS_EXECVE: u64 = 59;
const SYS_WAIT4: u64 = 61;

fn exec_with_args(program: ptr, arg1: ptr, arg2: ptr) -> u64 {
    unsafe {
        let pid: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid == 0 {
            // Child
            let args: [ptr; 4] = [program, arg1, arg2, 0 as ptr];
            let env: [ptr; 1] = [0 as ptr];
            
            syscall(SYS_EXECVE, program, &args, &env);
            return 1;  // Exec failed
        } else {
            // Parent
            let mut status: u64 = 0;
            syscall(SYS_WAIT4, pid, &status, 0, 0);
            return 0;
        }
    }
}

fn main() -> u64 {
    let prog: ptr = "/bin/echo";
    let arg1: ptr = "Hello";
    let arg2: ptr = "World";
    
    return exec_with_args(prog, arg1, arg2);
}
```
</details>

### Exercise 3: Check if Process Exists

Write a function that checks if a process with given PID exists.

<details>
<summary>Solution</summary>

```lwanga
const SYS_KILL: u64 = 62;

fn process_exists(pid: u64) -> u64 {
    unsafe {
        // Send signal 0 (null signal) to check existence
        let result: u64 = syscall(SYS_KILL, pid, 0, 0);
        
        if result == 0 {
            return 1;  // Process exists
        }
        
        return 0;  // Process doesn't exist
    }
}

fn main() -> u64 {
    let my_pid: u64 = 0;
    unsafe {
        my_pid = syscall(SYS_GETPID, 0, 0, 0);
    }
    
    return process_exists(my_pid);  // Returns 1
}
```
</details>

### Exercise 4: Fork Bomb Protection

Write a safe fork function that limits the number of children.

<details>
<summary>Solution</summary>

```lwanga
const MAX_CHILDREN: u64 = 10;

fn safe_fork(current_count: u64) -> u64 {
    if current_count >= MAX_CHILDREN {
        return 0;  // Limit reached
    }
    
    unsafe {
        return syscall(SYS_FORK, 0, 0, 0);
    }
}

fn main() -> u64 {
    let mut count: u64 = 0;
    
    while count < 5 {
        let pid: u64 = safe_fork(count);
        
        if pid == 0 {
            // Child
            return 0;
        } else if pid > 0 {
            count = count + 1;
        } else {
            break;  // Fork failed
        }
    }
    
    // Wait for all children
    let mut i: u64 = 0;
    unsafe {
        while i < count {
            let mut status: u64 = 0;
            syscall(SYS_WAIT4, -1, &status, 0, 0);
            i = i + 1;
        }
    }
    
    return count;
}
```
</details>

### Exercise 5: Process Tree

Create a process tree: parent creates 2 children, each child creates 2 grandchildren.

<details>
<summary>Solution</summary>

```lwanga
const SYS_FORK: u64 = 57;
const SYS_WAIT4: u64 = 61;
const SYS_GETPID: u64 = 39;
const SYS_EXIT: u64 = 60;

fn create_tree(level: u64) -> u64 {
    if level == 0 {
        return 0;
    }
    
    unsafe {
        // Create first child
        let pid1: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid1 == 0 {
            // First child
            create_tree(level - 1);
            syscall(SYS_EXIT, 0, 0, 0);
        }
        
        // Create second child
        let pid2: u64 = syscall(SYS_FORK, 0, 0, 0);
        
        if pid2 == 0 {
            // Second child
            create_tree(level - 1);
            syscall(SYS_EXIT, 0, 0, 0);
        }
        
        // Wait for both children
        let mut status: u64 = 0;
        syscall(SYS_WAIT4, pid1, &status, 0, 0);
        syscall(SYS_WAIT4, pid2, &status, 0, 0);
    }
    
    return 0;
}

fn main() -> u64 {
    create_tree(2);  // 2 levels: 2 children + 4 grandchildren
    return 0;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ Process concepts: PID, PPID, memory space
- ✅ Getting process IDs with getpid/getppid
- ✅ Creating child processes with fork
- ✅ Executing programs with execve
- ✅ Waiting for children with wait4
- ✅ Fork + exec pattern for running programs
- ✅ Practical examples: shell, parallel processing, daemons
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand process management, let's learn about network programming in [Lesson 24: Network Programming](24-network-programming.md).

---

**Navigation**: [← Previous: File Operations](22-file-operations.md) | [Tutorial Home](README.md) | [Next: Network Programming →](24-network-programming.md)
