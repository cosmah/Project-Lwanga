# Lesson 24: Network Programming

**Learn how to create network connections using sockets.**

---

## Creating a Socket

```lwanga
const SYS_SOCKET: u64 = 41;
const AF_INET: u32 = 2;
const SOCK_STREAM: u32 = 1;

fn create_socket() -> u64 {
    let sockfd: u64 = 0;
    
    unsafe {
        sockfd = syscall(SYS_SOCKET, AF_INET, SOCK_STREAM, 0);
    }
    
    return sockfd;
}
```

---

## Binding to Port

```lwanga
const SYS_BIND: u64 = 49;

packed struct sockaddr_in {
    sin_family: u16,
    sin_port: u16,
    sin_addr: u32,
    sin_zero: [u8; 8],
}

fn bind_socket(sockfd: u64, port: u16) -> u64 {
    let addr: sockaddr_in = sockaddr_in {
        sin_family: AF_INET as u16,
        sin_port: htons(port),
        sin_addr: 0,  // INADDR_ANY
        sin_zero: [0; 8],
    };
    
    unsafe {
        return syscall(SYS_BIND, sockfd, &addr, 16);
    }
}
```

---

## Listening for Connections

```lwanga
const SYS_LISTEN: u64 = 50;

fn listen_socket(sockfd: u64, backlog: u32) -> u64 {
    unsafe {
        return syscall(SYS_LISTEN, sockfd, backlog, 0);
    }
}
```

---

## Accepting Connections

```lwanga
const SYS_ACCEPT: u64 = 43;

fn accept_connection(sockfd: u64) -> u64 {
    let clientfd: u64 = 0;
    
    unsafe {
        clientfd = syscall(SYS_ACCEPT, sockfd, 0, 0);
    }
    
    return clientfd;
}
```

---

**Navigation**: [← Previous: Process Management](23-process-management.md) | [Tutorial Home](README.md) | [Next: Signals →](25-signals.md)
