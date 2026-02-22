# Lesson 15: Type System Deep Dive

**Master Lwanga's type system, conversions, and type safety.**

---

## Type Safety

Lwanga is strongly typed - no implicit conversions:

```lwanga
let x: u32 = 42;
let y: u64 = x;  // ERROR: type mismatch
let z: u64 = x as u64;  // OK: explicit cast
```

---

## Type Casting

```lwanga
// Widening (safe)
let a: u8 = 100;
let b: u16 = a as u16;
let c: u32 = b as u32;
let d: u64 = c as u64;

// Narrowing (can lose data)
let x: u64 = 300;
let y: u8 = x as u8;  // y = 44 (300 % 256)
```

---

## Pointer Casting

```lwanga
let x: u64 = 42;
let ptr: ptr = &x;
let addr: u64 = ptr as u64;  // Pointer to integer
let back: ptr = addr as ptr;  // Integer to pointer
```

---

**Navigation**: [← Previous: Packed Structs](14-packed-structs.md) | [Tutorial Home](README.md) | [Next: Constants →](16-constants.md)
