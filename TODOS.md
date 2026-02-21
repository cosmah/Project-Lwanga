# TODOs and Placeholders Tracker

This document tracks all TODOs, placeholders, and incomplete implementations across the Lwanga compiler codebase. Any new TODO added to the codebase MUST be registered here.

## Purpose

- Ensure no placeholders or incomplete implementations are forgotten
- Track what needs to be completed before production release
- Maintain visibility of all pending work

---

## Current TODOs

### 1. Compiler Driver (main.cpp)
**File:** `compiler/src/driver/main.cpp`  
**Line:** ~88  
**Status:** Expected - Part of Task 22  
**Description:**
```cpp
// TODO: Implement compilation pipeline
// 1. Lexer: Read source file and tokenize
// 2. Parser: Build AST from tokens
// 3. Type Checker: Perform semantic analysis
// 4. IR Generator: Generate LLVM IR from AST
// 5. Backend: Optimize and generate machine code
```
**Resolution Plan:** Will be implemented in Task 22 (Implement compiler driver and CLI)

### 2. Field Access in Address-Of
**File:** `compiler/src/codegen/IRGenerator.cpp`  
**Function:** `generateLValueAddress()` - field access case  
**Line:** ~690  
**Status:** Expected - Part of Task 8  
**Description:**
```cpp
// For now, we need the struct type information to compute field offset
// This will be fully implemented in Task 8 when we handle structs
reportError("Field access in address-of not yet fully implemented (Task 8)");
```
**Resolution Plan:** Will be implemented in Task 8 (Implement arrays and structs) when we add full struct support with GEP instructions for field offsets

---

## Design Decisions (Not TODOs, but documented for clarity)

None currently - all previous design limitations have been resolved.

---

## Completed and Removed

### ✓ Address-of Operator for Complex Expressions
**Previously:** Only supported simple variables  
**Status:** COMPLETED - Now supports variables, dereferences, and array indexing  
**Completed:** Task 7 (additional work)

### ✓ Unsafe Block IR Generation
**Previously:** Not implemented  
**Status:** COMPLETED - Unsafe blocks now properly generate scoped IR  
**Completed:** Task 7 (additional work)

### ✓ Complex LValue Assignments
**Previously:** Only supported simple variable assignments  
**Status:** COMPLETED - Now supports any lvalue (dereferences, array indexing)  
**Completed:** Task 7 (additional work)

### ✓ Constant Expression Evaluation
**Previously:** Had placeholder for div, mod, bitwise ops, shifts  
**Status:** COMPLETED - All operations now fully implemented with compile-time evaluation  
**Completed:** Task 7

### ✓ Struct Size Calculation
**Previously:** Returned hardcoded value of 8  
**Status:** COMPLETED - Full production implementation with proper field layout, padding, and alignment  
**Completed:** Task 4

### ✓ Constant Declarations
**Previously:** Empty placeholder function  
**Status:** COMPLETED - Full compile-time constant evaluation with global variables  
**Completed:** Task 6

---

## Guidelines for Adding New TODOs

When adding a TODO to the codebase:

1. Add a comment in the code:
   ```cpp
   // TODO: [Brief description]
   // Tracked in TODOS.md
   ```

2. Register it in this file with:
   - File path
   - Line number (approximate)
   - Status (Expected/Blocking/Optional)
   - Full description
   - Resolution plan or task number

3. Update this file when the TODO is resolved

---

## Statistics

- **Active TODOs:** 2
- **Design Decisions:** 0
- **Completed TODOs:** 6

**Last Updated:** Task 7 completion (full address-of and lvalue support)
