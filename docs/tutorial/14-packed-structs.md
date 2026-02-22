# Lesson 14: Packed Structs

**Learn about memory layout control with packed structs.**

---

## What are Packed Structs?

Packed structs have no padding between fields - they're laid out exactly as defined.

```lwanga
packed struct Header {
    magic: u32,
    version: u16,
    flags: u16,
}
```

---

## Regular vs Packed

### Regular Struct (May Have Padding)

```lwanga
struct Regular {
    a: u8,   // 1 byte
    b: u64,  // 8 bytes (may be padded to align)
    c: u8,   // 1 byte
}
// Total size: possibly 24 bytes with padding
```

### Packed Struct (No Padding)

```lwanga
packed struct Packed {
    a: u8,   // 1 byte
    b: u64,  // 8 bytes
    c: u8,   // 1 byte
}
// Total size: exactly 10 bytes
```

---

## When to Use Packed Structs

- Binary file formats
- Network protocols
- Hardware interfaces
- Exact memory layout required

---

## Example: File Header

```lwanga
packed struct FileHeader {
    magic: u32,      // 4 bytes: file signature
    version: u16,    // 2 bytes: format version
    flags: u16,      // 2 bytes: option flags
    size: u64,       // 8 bytes: file size
}
// Total: exactly 16 bytes

fn main() -> u64 {
    let header: FileHeader = FileHeader {
        magic: 0x12345678,
        version: 1,
        flags: 0,
        size: 1024,
    };
    
    return header.size;
}
```

---

## Example: Network Packet

```lwanga
packed struct PacketHeader {
    src_port: u16,
    dst_port: u16,
    length: u16,
    checksum: u16,
}

fn create_packet(src: u16, dst: u16) -> PacketHeader {
    let pkt: PacketHeader = PacketHeader {
        src_port: src,
        dst_port: dst,
        length: 8,
        checksum: 0,
    };
    
    return pkt;
}
```

---

**Navigation**: [← Previous: Structs](13-structs.md) | [Tutorial Home](README.md) | [Next: Type System →](15-type-system.md)
