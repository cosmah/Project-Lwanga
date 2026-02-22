# Lesson 14: Packed Structs

**In this lesson, you'll learn about memory layout control with packed structs, understand memory alignment, and see when precise memory layout is critical.**

---

## What are Packed Structs?

A packed struct is a struct where fields are laid out in memory exactly as you define them, with no padding or alignment gaps between fields.

**Regular struct:**
```lwanga
struct Regular {
    a: u8,
    b: u64,
}
// Compiler may add 7 bytes of padding after 'a' for alignment
```

**Packed struct:**
```lwanga
packed struct Packed {
    a: u8,
    b: u64,
}
// No padding - exactly 9 bytes (1 + 8)
```

**Why does this matter?**

When working with:
- Binary file formats (exact byte layout required)
- Network protocols (standard packet structures)
- Hardware interfaces (memory-mapped I/O)
- Interoperability with C code
- Reverse engineering (matching existing structures)

You need precise control over memory layout. Packed structs give you that control.

---

## Understanding Memory Alignment

### What is Memory Alignment?

Memory alignment means data is stored at addresses that are multiples of their size:

- `u8` (1 byte) can be at any address
- `u16` (2 bytes) should be at even addresses (0, 2, 4, 6...)
- `u32` (4 bytes) should be at addresses divisible by 4 (0, 4, 8, 12...)
- `u64` (8 bytes) should be at addresses divisible by 8 (0, 8, 16, 24...)

**Why does alignment matter?**

Modern CPUs access aligned data faster. Some CPUs can't access misaligned data at all!

### Regular Struct with Padding

```lwanga
struct Regular {
    a: u8,   // Offset 0, size 1
    // 7 bytes of padding added here
    b: u64,  // Offset 8, size 8 (aligned to 8-byte boundary)
    c: u8,   // Offset 16, size 1
    // 7 bytes of padding added here
}
// Total size: 24 bytes
```

**Memory layout:**
```
Offset: 0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23
Data:   [a][padding............][b...................][c][padding............]
```

**Why the padding?**

The compiler adds padding to ensure:
1. `b` (u64) is aligned to an 8-byte boundary
2. The struct size is a multiple of its largest field's alignment

### Packed Struct without Padding

```lwanga
packed struct Packed {
    a: u8,   // Offset 0, size 1
    b: u64,  // Offset 1, size 8 (NOT aligned!)
    c: u8,   // Offset 9, size 1
}
// Total size: 10 bytes
```

**Memory layout:**
```
Offset: 0  1  2  3  4  5  6  7  8  9
Data:   [a][b...................][c]
```

**Trade-off:**
- ✅ Smaller memory footprint
- ✅ Exact layout control
- ❌ Potentially slower access (misaligned reads)
- ❌ May not work on some architectures

---

## Declaring Packed Structs

### Basic Syntax

```lwanga
packed struct StructName {
    field1: type1,
    field2: type2,
    field3: type3,
}
```

The `packed` keyword before `struct` tells the compiler: "Don't add any padding."

### Example: File Magic Number

```lwanga
packed struct FileMagic {
    magic: u32,      // 4 bytes
    version: u16,    // 2 bytes
    flags: u16,      // 2 bytes
}
// Total: exactly 8 bytes

fn main() -> u64 {
    let magic: FileMagic = FileMagic {
        magic: 0x89504E47,  // PNG magic number
        version: 1,
        flags: 0,
    };
    
    return magic.magic as u64;
}
```

**Why packed here?**

File formats define exact byte layouts. If we used a regular struct, the compiler might add padding, and our file wouldn't match the specification.

---

## When to Use Packed Structs

### Use Case 1: Binary File Formats

File formats specify exact byte layouts:

```lwanga
// ELF file header (simplified)
packed struct ElfHeader {
    magic: u32,          // 0x7F 'E' 'L' 'F'
    class: u8,           // 32-bit or 64-bit
    data: u8,            // Endianness
    version: u8,         // ELF version
    os_abi: u8,          // OS/ABI
    abi_version: u8,     // ABI version
    pad: [u8; 7],        // Padding
    type: u16,           // Object file type
    machine: u16,        // Architecture
    version2: u32,       // Object file version
}
// Must be exactly 24 bytes to match ELF specification

fn read_elf_header(data: ptr) -> ElfHeader {
    // Read exactly 24 bytes from file
    unsafe {
        let header: ptr = data;
        return *(header as ptr);
    }
}
```

### Use Case 2: Network Protocols

Network packets have standardized layouts:

```lwanga
// TCP header (simplified)
packed struct TcpHeader {
    src_port: u16,       // Source port
    dst_port: u16,       // Destination port
    seq_num: u32,        // Sequence number
    ack_num: u32,        // Acknowledgment number
    flags: u16,          // Data offset + flags
    window: u16,         // Window size
    checksum: u16,       // Checksum
    urgent: u16,         // Urgent pointer
}
// Must be exactly 20 bytes (TCP minimum header size)

fn create_tcp_packet(src: u16, dst: u16, seq: u32) -> TcpHeader {
    let header: TcpHeader = TcpHeader {
        src_port: src,
        dst_port: dst,
        seq_num: seq,
        ack_num: 0,
        flags: 0x5000,  // Data offset = 5 (20 bytes)
        window: 65535,
        checksum: 0,
        urgent: 0,
    };
    
    return header;
}

fn main() -> u64 {
    let packet: TcpHeader = create_tcp_packet(8080, 80, 1000);
    return packet.src_port as u64;
}
```

**Why packed here?**

Network protocols are standardized. Every TCP implementation must use the same byte layout, or packets won't be understood.

### Use Case 3: Hardware Interfaces

Memory-mapped hardware registers:

```lwanga
// UART control register
packed struct UartControl {
    enable: u8,          // Enable bit
    baud_rate: u16,      // Baud rate divisor
    parity: u8,          // Parity setting
    stop_bits: u8,       // Stop bits
    data_bits: u8,       // Data bits
    reserved: u16,       // Reserved
}
// Must match hardware register layout exactly

fn configure_uart(base_addr: ptr) -> u64 {
    let config: UartControl = UartControl {
        enable: 1,
        baud_rate: 115200,
        parity: 0,
        stop_bits: 1,
        data_bits: 8,
        reserved: 0,
    };
    
    unsafe {
        // Write to hardware register
        *(base_addr as ptr) = config;
    }
    
    return 0;
}
```

### Use Case 4: Interoperability with C

Matching C struct layouts:

```c
// C code
struct __attribute__((packed)) CHeader {
    uint8_t type;
    uint32_t length;
    uint16_t checksum;
};
```

```lwanga
// Lwanga equivalent
packed struct CHeader {
    type: u8,
    length: u32,
    checksum: u16,
}
// Exactly 7 bytes, matches C layout
```

---

## Practical Examples

### Example 1: PNG File Header

```lwanga
packed struct PngHeader {
    signature: [u8; 8],  // PNG signature
    length: u32,         // Chunk length
    chunk_type: u32,     // Chunk type (IHDR)
    width: u32,          // Image width
    height: u32,         // Image height
    bit_depth: u8,       // Bits per pixel
    color_type: u8,      // Color type
    compression: u8,     // Compression method
    filter: u8,          // Filter method
    interlace: u8,       // Interlace method
}

fn create_png_header(w: u32, h: u32) -> PngHeader {
    let header: PngHeader = PngHeader {
        signature: [137, 80, 78, 71, 13, 10, 26, 10],  // PNG signature
        length: 13,
        chunk_type: 0x49484452,  // "IHDR"
        width: w,
        height: h,
        bit_depth: 8,
        color_type: 6,  // RGBA
        compression: 0,
        filter: 0,
        interlace: 0,
    };
    
    return header;
}

fn main() -> u64 {
    let header: PngHeader = create_png_header(800, 600);
    return header.width as u64;
}
```

### Example 2: IP Packet Header

```lwanga
packed struct IpHeader {
    version_ihl: u8,     // Version (4 bits) + IHL (4 bits)
    tos: u8,             // Type of service
    total_length: u16,   // Total length
    identification: u16, // Identification
    flags_offset: u16,   // Flags (3 bits) + Fragment offset (13 bits)
    ttl: u8,             // Time to live
    protocol: u8,        // Protocol
    checksum: u16,       // Header checksum
    src_addr: u32,       // Source IP address
    dst_addr: u32,       // Destination IP address
}

fn create_ip_header(src: u32, dst: u32, proto: u8) -> IpHeader {
    let header: IpHeader = IpHeader {
        version_ihl: 0x45,  // IPv4, 20-byte header
        tos: 0,
        total_length: 20,
        identification: 0,
        flags_offset: 0,
        ttl: 64,
        protocol: proto,
        checksum: 0,
        src_addr: src,
        dst_addr: dst,
    };
    
    return header;
}

fn main() -> u64 {
    // Create IP header for TCP packet
    let header: IpHeader = create_ip_header(
        0xC0A80001,  // 192.168.0.1
        0xC0A80002,  // 192.168.0.2
        6            // TCP protocol
    );
    
    return header.protocol as u64;
}
```

### Example 3: BMP File Header

```lwanga
packed struct BmpFileHeader {
    signature: u16,      // "BM"
    file_size: u32,      // File size in bytes
    reserved1: u16,      // Reserved
    reserved2: u16,      // Reserved
    data_offset: u32,    // Offset to pixel data
}

packed struct BmpInfoHeader {
    header_size: u32,    // Header size (40 bytes)
    width: u32,          // Image width
    height: u32,         // Image height
    planes: u16,         // Color planes (always 1)
    bits_per_pixel: u16, // Bits per pixel
    compression: u32,    // Compression method
    image_size: u32,     // Image size
    x_pixels_per_m: u32, // Horizontal resolution
    y_pixels_per_m: u32, // Vertical resolution
    colors_used: u32,    // Number of colors
    colors_important: u32, // Important colors
}

fn create_bmp_headers(width: u32, height: u32) -> u64 {
    let file_header: BmpFileHeader = BmpFileHeader {
        signature: 0x4D42,  // "BM"
        file_size: 54 + (width * height * 3),
        reserved1: 0,
        reserved2: 0,
        data_offset: 54,
    };
    
    let info_header: BmpInfoHeader = BmpInfoHeader {
        header_size: 40,
        width: width,
        height: height,
        planes: 1,
        bits_per_pixel: 24,
        compression: 0,
        image_size: width * height * 3,
        x_pixels_per_m: 0,
        y_pixels_per_m: 0,
        colors_used: 0,
        colors_important: 0,
    };
    
    return file_header.file_size as u64;
}

fn main() -> u64 {
    return create_bmp_headers(800, 600);
}
```

### Example 4: USB Descriptor

```lwanga
packed struct UsbDeviceDescriptor {
    length: u8,              // Descriptor length (18 bytes)
    descriptor_type: u8,     // Descriptor type (1 = device)
    usb_version: u16,        // USB specification version
    device_class: u8,        // Device class
    device_subclass: u8,     // Device subclass
    device_protocol: u8,     // Device protocol
    max_packet_size: u8,     // Maximum packet size
    vendor_id: u16,          // Vendor ID
    product_id: u16,         // Product ID
    device_version: u16,     // Device version
    manufacturer: u8,        // Manufacturer string index
    product: u8,             // Product string index
    serial_number: u8,       // Serial number string index
    num_configurations: u8,  // Number of configurations
}

fn create_usb_descriptor() -> UsbDeviceDescriptor {
    let desc: UsbDeviceDescriptor = UsbDeviceDescriptor {
        length: 18,
        descriptor_type: 1,
        usb_version: 0x0200,  // USB 2.0
        device_class: 0,
        device_subclass: 0,
        device_protocol: 0,
        max_packet_size: 64,
        vendor_id: 0x1234,
        product_id: 0x5678,
        device_version: 0x0100,
        manufacturer: 1,
        product: 2,
        serial_number: 3,
        num_configurations: 1,
    };
    
    return desc;
}

fn main() -> u64 {
    let desc: UsbDeviceDescriptor = create_usb_descriptor();
    return desc.vendor_id as u64;
}
```

---

## Design Decisions Explained

### Why Have Both Regular and Packed Structs?

**Decision:** Lwanga provides both regular structs (with padding) and packed structs (without padding).

**Reason:**
- Regular structs are faster (aligned access)
- Packed structs give exact control (required for protocols)
- Different use cases need different trade-offs

**Impact:** You choose the right tool for the job.

### Why Explicit `packed` Keyword?

**Decision:** You must explicitly write `packed` to get a packed struct.

**Reason:**
- Makes the performance trade-off visible
- Prevents accidental misalignment
- Clear intent in code
- Safer default (regular structs are aligned)

**Impact:** Code is more explicit and safer.

### Why Allow Misaligned Access?

**Decision:** Packed structs allow misaligned field access.

**Reason:**
- Required for binary formats and protocols
- Gives you full control
- You know what you're doing (explicit choice)

**Impact:** More power, but you must be careful.

---

## Common Mistakes

### Mistake 1: Using Regular Struct for Binary Format

```lwanga
// WRONG: Regular struct for file format
struct FileHeader {
    magic: u32,
    version: u16,
}
// Compiler may add padding!

fn main() -> u64 {
    let header: FileHeader = FileHeader {
        magic: 0x12345678,
        version: 1,
    };
    
    // Size might be 8 bytes instead of 6!
    return 0;
}
```

**Fix:** Use packed struct:
```lwanga
packed struct FileHeader {
    magic: u32,
    version: u16,
}
// Guaranteed to be exactly 6 bytes
```

### Mistake 2: Assuming Packed is Always Better

```lwanga
// WRONG: Using packed for internal data structures
packed struct Point {
    x: u64,
    y: u64,
}
// No benefit - fields are already aligned!
```

**Fix:** Use regular struct for internal data:
```lwanga
struct Point {
    x: u64,
    y: u64,
}
// Faster access, same size
```

**Rule of thumb:**
- Use `packed` for external formats (files, networks, hardware)
- Use regular structs for internal data structures

### Mistake 3: Wrong Size Calculation

```lwanga
packed struct Data {
    a: u8,
    b: u32,
    c: u8,
}

fn main() -> u64 {
    // WRONG: Assuming size is 8 bytes
    // Actual size is 6 bytes (1 + 4 + 1)
    return 8;
}
```

**Fix:** Calculate correctly:
```lwanga
fn main() -> u64 {
    // Correct: 1 + 4 + 1 = 6 bytes
    return 6;
}
```

---

## Performance Considerations

### Aligned vs Misaligned Access

**Aligned access (regular struct):**
```lwanga
struct Aligned {
    a: u64,  // At offset 0 (aligned)
    b: u64,  // At offset 8 (aligned)
}
// Fast: CPU can read in one operation
```

**Misaligned access (packed struct):**
```lwanga
packed struct Misaligned {
    a: u8,   // At offset 0
    b: u64,  // At offset 1 (misaligned!)
}
// Slower: CPU may need multiple operations
```

**Benchmark example:**
```
Aligned read:    1 cycle
Misaligned read: 3-7 cycles (or crash on some CPUs!)
```

### When Performance Matters

Use regular structs for:
- Hot code paths
- Frequently accessed data
- Internal data structures
- Performance-critical code

Use packed structs for:
- Binary I/O
- Network protocols
- Hardware interfaces
- Exact layout requirements

---

## Exercises

### Exercise 1: Create a WAV File Header

Create a packed struct for a WAV audio file header. WAV headers have:
- "RIFF" signature (4 bytes)
- File size minus 8 (4 bytes)
- "WAVE" format (4 bytes)
- "fmt " subchunk (4 bytes)
- Subchunk size: 16 (4 bytes)

<details>
<summary>Solution</summary>

```lwanga
packed struct WavHeader {
    riff: u32,           // "RIFF"
    file_size: u32,      // File size - 8
    wave: u32,           // "WAVE"
    fmt: u32,            // "fmt "
    subchunk_size: u32,  // 16
}

fn create_wav_header(size: u32) -> WavHeader {
    let header: WavHeader = WavHeader {
        riff: 0x46464952,      // "RIFF"
        file_size: size - 8,
        wave: 0x45564157,      // "WAVE"
        fmt: 0x20746D66,       // "fmt "
        subchunk_size: 16,
    };
    
    return header;
}

fn main() -> u64 {
    let header: WavHeader = create_wav_header(1000);
    return header.file_size as u64;
}
```
</details>

### Exercise 2: Create an Ethernet Frame Header

Create a packed struct for an Ethernet frame header with:
- Destination MAC (6 bytes)
- Source MAC (6 bytes)
- EtherType (2 bytes)

<details>
<summary>Solution</summary>

```lwanga
packed struct EthernetHeader {
    dst_mac: [u8; 6],
    src_mac: [u8; 6],
    ethertype: u16,
}

fn create_ethernet_header() -> EthernetHeader {
    let header: EthernetHeader = EthernetHeader {
        dst_mac: [0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF],  // Broadcast
        src_mac: [0x00, 0x11, 0x22, 0x33, 0x44, 0x55],
        ethertype: 0x0800,  // IPv4
    };
    
    return header;
}

fn main() -> u64 {
    let header: EthernetHeader = create_ethernet_header();
    return header.ethertype as u64;
}
```
</details>

### Exercise 3: Calculate Struct Sizes

Calculate the exact size in bytes of these structs:

```lwanga
packed struct A {
    x: u8,
    y: u16,
    z: u32,
}

packed struct B {
    a: u64,
    b: u8,
    c: u16,
}

struct C {  // Regular struct
    x: u8,
    y: u64,
}
```

<details>
<summary>Solution</summary>

```lwanga
// Struct A (packed): 1 + 2 + 4 = 7 bytes
// Struct B (packed): 8 + 1 + 2 = 11 bytes
// Struct C (regular): 1 + 7 (padding) + 8 = 16 bytes

fn main() -> u64 {
    return 7 + 11 + 16;  // Total: 34 bytes
}
```
</details>

### Exercise 4: Create a ZIP File Header

Create a packed struct for a ZIP local file header (simplified):
- Signature: 0x04034b50 (4 bytes)
- Version: 20 (2 bytes)
- Flags: 0 (2 bytes)
- Compression: 0 (2 bytes)
- Filename length (2 bytes)

<details>
<summary>Solution</summary>

```lwanga
packed struct ZipLocalHeader {
    signature: u32,
    version: u16,
    flags: u16,
    compression: u16,
    filename_len: u16,
}

fn create_zip_header(name_len: u16) -> ZipLocalHeader {
    let header: ZipLocalHeader = ZipLocalHeader {
        signature: 0x04034b50,
        version: 20,
        flags: 0,
        compression: 0,
        filename_len: name_len,
    };
    
    return header;
}

fn main() -> u64 {
    let header: ZipLocalHeader = create_zip_header(10);
    return header.signature as u64;
}
```
</details>

### Exercise 5: Compare Regular vs Packed

Create both regular and packed versions of this struct and calculate their sizes:

```lwanga
struct Data {
    flag: u8,
    count: u64,
    status: u8,
}
```

<details>
<summary>Solution</summary>

```lwanga
// Regular struct
struct RegularData {
    flag: u8,      // Offset 0, size 1
    // 7 bytes padding
    count: u64,    // Offset 8, size 8
    status: u8,    // Offset 16, size 1
    // 7 bytes padding
}
// Total: 24 bytes

// Packed struct
packed struct PackedData {
    flag: u8,      // Offset 0, size 1
    count: u64,    // Offset 1, size 8
    status: u8,    // Offset 9, size 1
}
// Total: 10 bytes

fn main() -> u64 {
    // Regular: 24 bytes
    // Packed: 10 bytes
    // Savings: 14 bytes (58% smaller!)
    return 24 - 10;
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ What packed structs are and how they differ from regular structs
- ✅ Understanding memory alignment and padding
- ✅ When to use packed structs (binary formats, protocols, hardware)
- ✅ When to use regular structs (internal data, performance)
- ✅ How to declare packed structs with the `packed` keyword
- ✅ Practical examples: file formats, network protocols, hardware interfaces
- ✅ Design decisions behind packed vs regular structs
- ✅ Performance trade-offs of aligned vs misaligned access
- ✅ Common mistakes and how to avoid them

---

## Next Lesson

Now that you understand memory layout control, let's explore the complete type system in [Lesson 15: Type System](15-type-system.md).

---

**Navigation**: [← Previous: Structs](13-structs.md) | [Tutorial Home](README.md) | [Next: Type System →](15-type-system.md)
