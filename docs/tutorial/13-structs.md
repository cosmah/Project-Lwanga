# Lesson 13: Structs

**In this lesson, you'll learn how to create custom data types using structs, group related data together, and build complex data structures.**

---

## What are Structs?

Structs (structures) allow you to group related data together under a single name. They're essential for organizing complex data and creating custom types.

**Why use structs?**
- Group related data logically
- Create custom types
- Model real-world entities
- Build complex data structures
- Make code more organized and readable

**Real-world analogy:**
```
A Person has:
- name (string)
- age (number)
- id (number)

Instead of separate variables, group them in a struct!
```

---

## Defining Structs

### Basic Struct Definition

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn main() -> u64 {
    let p: Point = Point { x: 10, y: 20 };
    return p.x;  // Returns 10
}
```

**Syntax:**
- `struct` - Keyword to define a structure
- `Point` - Name of the struct (use PascalCase)
- `{ }` - Contains field definitions
- `x: u64` - Field name and type
- `,` - Separate fields with commas

**Design choice:** Struct names use PascalCase (like `Point`, `UserData`) to distinguish them from variables and functions.

---

## Creating Struct Instances

### Basic Initialization

```lwanga
struct Rectangle {
    width: u64,
    height: u64,
}

fn main() -> u64 {
    let rect: Rectangle = Rectangle {
        width: 100,
        height: 50,
    };
    
    return rect.width;  // Returns 100
}
```

### Multiple Instances

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn main() -> u64 {
    let p1: Point = Point { x: 0, y: 0 };
    let p2: Point = Point { x: 10, y: 20 };
    let p3: Point = Point { x: 100, y: 200 };
    
    return p2.x + p2.y;  // Returns 30
}
```

---

## Accessing Struct Fields

### Reading Fields

```lwanga
struct Person {
    age: u64,
    id: u32,
}

fn main() -> u64 {
    let person: Person = Person {
        age: 25,
        id: 12345,
    };
    
    let person_age: u64 = person.age;
    let person_id: u32 = person.id;
    
    return person_age;  // Returns 25
}
```

### Modifying Fields

```lwanga
struct Counter {
    value: u64,
}

fn main() -> u64 {
    let mut counter: Counter = Counter { value: 0 };
    
    counter.value = counter.value + 1;
    counter.value = counter.value + 1;
    counter.value = counter.value + 1;
    
    return counter.value;  // Returns 3
}
```

**Note:** The struct variable must be `mut` to modify its fields.

---

## Structs with Different Types

### Mixed Field Types

```lwanga
struct User {
    name: ptr,
    age: u64,
    id: u32,
    active: u8,
}

fn main() -> u64 {
    let user: User = User {
        name: "Alice",
        age: 30,
        id: 1001,
        active: 1,
    };
    
    return user.age;
}
```

### Struct with Pointers

```lwanga
struct Message {
    text: ptr,
    length: u64,
    priority: u8,
}

fn main() -> u64 {
    let msg: Message = Message {
        text: "Hello, World!",
        length: 13,
        priority: 1,
    };
    
    return msg.length;
}
```

---

## Nested Structs

### Basic Nesting

```lwanga
struct Address {
    street: ptr,
    number: u32,
}

struct Person {
    name: ptr,
    addr: Address,
}

fn main() -> u64 {
    let addr: Address = Address {
        street: "Main St",
        number: 123,
    };
    
    let person: Person = Person {
        name: "Bob",
        addr: addr,
    };
    
    return person.addr.number as u64;  // Returns 123
}
```

### Multiple Levels of Nesting

```lwanga
struct Coordinates {
    x: u64,
    y: u64,
}

struct Location {
    coords: Coordinates,
    name: ptr,
}

struct Building {
    location: Location,
    floors: u32,
}

fn main() -> u64 {
    let coords: Coordinates = Coordinates { x: 100, y: 200 };
    
    let location: Location = Location {
        coords: coords,
        name: "Downtown",
    };
    
    let building: Building = Building {
        location: location,
        floors: 10,
    };
    
    return building.location.coords.x;  // Returns 100
}
```

---

## Functions with Structs

### Passing Structs to Functions

```lwanga
struct Rectangle {
    width: u64,
    height: u64,
}

fn calculate_area(rect: Rectangle) -> u64 {
    return rect.width * rect.height;
}

fn main() -> u64 {
    let r: Rectangle = Rectangle {
        width: 10,
        height: 20,
    };
    
    let area: u64 = calculate_area(r);
    return area;  // Returns 200
}
```

**Note:** Structs are passed by value (copied), just like other types.

### Returning Structs from Functions

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn create_point(x: u64, y: u64) -> Point {
    let p: Point = Point { x: x, y: y };
    return p;
}

fn main() -> u64 {
    let point: Point = create_point(50, 75);
    return point.x + point.y;  // Returns 125
}
```

### Struct Constructors

```lwanga
struct Circle {
    center_x: u64,
    center_y: u64,
    radius: u64,
}

fn new_circle(x: u64, y: u64, r: u64) -> Circle {
    return Circle {
        center_x: x,
        center_y: y,
        radius: r,
    };
}

fn main() -> u64 {
    let circle: Circle = new_circle(100, 100, 50);
    return circle.radius;  // Returns 50
}
```

---

## Practical Examples

### Example 1: Date Structure

```lwanga
struct Date {
    year: u32,
    month: u8,
    day: u8,
}

fn is_leap_year(year: u32) -> u64 {
    if year % 4 != 0 {
        return 0;
    }
    if year % 100 != 0 {
        return 1;
    }
    if year % 400 == 0 {
        return 1;
    }
    return 0;
}

fn days_in_month(date: Date) -> u64 {
    if date.month == 2 {
        if is_leap_year(date.year) == 1 {
            return 29;
        }
        return 28;
    }
    
    if date.month == 4 || date.month == 6 || 
       date.month == 9 || date.month == 11 {
        return 30;
    }
    
    return 31;
}

fn main() -> u64 {
    let date: Date = Date {
        year: 2024,
        month: 2,
        day: 15,
    };
    
    return days_in_month(date);  // Returns 29 (leap year)
}
```

### Example 2: Vector2D Operations

```lwanga
struct Vector2D {
    x: u64,
    y: u64,
}

fn vector_add(v1: Vector2D, v2: Vector2D) -> Vector2D {
    return Vector2D {
        x: v1.x + v2.x,
        y: v1.y + v2.y,
    };
}

fn vector_dot_product(v1: Vector2D, v2: Vector2D) -> u64 {
    return v1.x * v2.x + v1.y * v2.y;
}

fn vector_magnitude_squared(v: Vector2D) -> u64 {
    return v.x * v.x + v.y * v.y;
}

fn main() -> u64 {
    let v1: Vector2D = Vector2D { x: 3, y: 4 };
    let v2: Vector2D = Vector2D { x: 1, y: 2 };
    
    let v3: Vector2D = vector_add(v1, v2);
    // v3 is (4, 6)
    
    let dot: u64 = vector_dot_product(v1, v2);
    // dot is 11 (3*1 + 4*2)
    
    return dot;
}
```

### Example 3: RGB Color

```lwanga
struct Color {
    red: u8,
    green: u8,
    blue: u8,
}

fn color_to_grayscale(c: Color) -> u8 {
    // Average of RGB components
    let sum: u64 = (c.red as u64) + (c.green as u64) + (c.blue as u64);
    return (sum / 3) as u8;
}

fn color_mix(c1: Color, c2: Color) -> Color {
    return Color {
        red: ((c1.red as u64 + c2.red as u64) / 2) as u8,
        green: ((c1.green as u64 + c2.green as u64) / 2) as u8,
        blue: ((c1.blue as u64 + c2.blue as u64) / 2) as u8,
    };
}

fn main() -> u64 {
    let red: Color = Color { red: 255, green: 0, blue: 0 };
    let blue: Color = Color { red: 0, green: 0, blue: 255 };
    
    let purple: Color = color_mix(red, blue);
    // purple is (127, 0, 127)
    
    return purple.red as u64;
}
```

### Example 4: Student Record

```lwanga
struct Student {
    id: u32,
    name: ptr,
    grade: u8,
    passed: u8,
}

fn create_student(id: u32, name: ptr, grade: u8) -> Student {
    let passed: u8 = 0;
    if grade >= 60 {
        passed = 1;
    }
    
    return Student {
        id: id,
        name: name,
        grade: grade,
        passed: passed,
    };
}

fn calculate_class_average(students: ptr, count: u64) -> u64 {
    let mut total: u64 = 0;
    let mut i: u64 = 0;
    
    unsafe {
        while i < count {
            let student_ptr: ptr = students + i;
            // Access struct through pointer
            total = total + (*student_ptr).grade as u64;
            i = i + 1;
        }
    }
    
    return total / count;
}
```

### Example 5: Linked List Node

```lwanga
struct Node {
    value: u64,
    next: ptr,  // Pointer to next node
}

fn create_node(value: u64) -> Node {
    return Node {
        value: value,
        next: 0,  // null pointer
    };
}

fn list_length(head: ptr) -> u64 {
    let mut count: u64 = 0;
    let mut current: ptr = head;
    
    unsafe {
        while current != 0 {
            count = count + 1;
            current = (*current).next;
        }
    }
    
    return count;
}
```

---

## Struct Memory Layout

### Understanding Memory

```lwanga
struct Example {
    a: u8,   // 1 byte
    b: u64,  // 8 bytes
    c: u16,  // 2 bytes
}
```

**Memory layout (may have padding):**
```
Offset  Field  Size
0       a      1 byte
1-7     (pad)  7 bytes (alignment)
8       b      8 bytes
16      c      2 bytes
18-23   (pad)  6 bytes (alignment)
Total: 24 bytes
```

**Why padding?** The compiler adds padding to align fields for efficient memory access.

---

## Common Patterns

### Pattern 1: Builder Functions

```lwanga
struct Config {
    timeout: u64,
    retries: u32,
    verbose: u8,
}

fn default_config() -> Config {
    return Config {
        timeout: 30,
        retries: 3,
        verbose: 0,
    };
}

fn main() -> u64 {
    let mut cfg: Config = default_config();
    cfg.verbose = 1;  // Override one field
    
    return cfg.timeout;
}
```

### Pattern 2: Validation Functions

```lwanga
struct Rectangle {
    width: u64,
    height: u64,
}

fn is_valid_rectangle(rect: Rectangle) -> u64 {
    if rect.width == 0 || rect.height == 0 {
        return 0;  // Invalid
    }
    return 1;  // Valid
}

fn main() -> u64 {
    let rect: Rectangle = Rectangle { width: 10, height: 0 };
    return is_valid_rectangle(rect);  // Returns 0
}
```

### Pattern 3: Transformation Functions

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn translate_point(p: Point, dx: u64, dy: u64) -> Point {
    return Point {
        x: p.x + dx,
        y: p.y + dy,
    };
}

fn scale_point(p: Point, factor: u64) -> Point {
    return Point {
        x: p.x * factor,
        y: p.y * factor,
    };
}
```

---

## Common Mistakes

### Mistake 1: Forgetting mut for Modification

```lwanga
fn main() -> u64 {
    let counter: Counter = Counter { value: 0 };
    counter.value = counter.value + 1;  // ERROR: not mutable
    return counter.value;
}
```

**Fix:**
```lwanga
let mut counter: Counter = Counter { value: 0 };
counter.value = counter.value + 1;  // ✓ OK
```

### Mistake 2: Wrong Field Names

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn main() -> u64 {
    let p: Point = Point { x: 10, z: 20 };  // ERROR: no field 'z'
    return 0;
}
```

### Mistake 3: Missing Fields

```lwanga
struct Person {
    name: ptr,
    age: u64,
}

fn main() -> u64 {
    let p: Person = Person { name: "Alice" };  // ERROR: missing 'age'
    return 0;
}
```

---

## Exercises

### Exercise 1: Circle Area

Create a Circle struct and a function to calculate its area (use π ≈ 3).

<details>
<summary>Solution</summary>

```lwanga
struct Circle {
    radius: u64,
}

fn circle_area(c: Circle) -> u64 {
    let pi: u64 = 3;
    return pi * c.radius * c.radius;
}

fn main() -> u64 {
    let circle: Circle = Circle { radius: 10 };
    return circle_area(circle);  // Returns 300
}
```
</details>

### Exercise 2: Temperature Conversion

Create a Temperature struct with Celsius field and a function to convert to Fahrenheit.

<details>
<summary>Solution</summary>

```lwanga
struct Temperature {
    celsius: u64,
}

fn to_fahrenheit(temp: Temperature) -> u64 {
    return temp.celsius * 9 / 5 + 32;
}

fn main() -> u64 {
    let temp: Temperature = Temperature { celsius: 100 };
    return to_fahrenheit(temp);  // Returns 212
}
```
</details>

### Exercise 3: Distance Between Points

Calculate the squared distance between two points.

<details>
<summary>Solution</summary>

```lwanga
struct Point {
    x: u64,
    y: u64,
}

fn distance_squared(p1: Point, p2: Point) -> u64 {
    let dx: u64 = if p1.x > p2.x { p1.x - p2.x } else { p2.x - p1.x };
    let dy: u64 = if p1.y > p2.y { p1.y - p2.y } else { p2.y - p1.y };
    return dx * dx + dy * dy;
}

fn main() -> u64 {
    let p1: Point = Point { x: 0, y: 0 };
    let p2: Point = Point { x: 3, y: 4 };
    return distance_squared(p1, p2);  // Returns 25
}
```
</details>

### Exercise 4: Book Struct

Create a Book struct and a function to check if it's a long book (>500 pages).

<details>
<summary>Solution</summary>

```lwanga
struct Book {
    title: ptr,
    pages: u32,
    year: u32,
}

fn is_long_book(book: Book) -> u64 {
    if book.pages > 500 {
        return 1;
    }
    return 0;
}

fn main() -> u64 {
    let book: Book = Book {
        title: "War and Peace",
        pages: 1225,
        year: 1869,
    };
    
    return is_long_book(book);  // Returns 1
}
```
</details>

### Exercise 5: Time Duration

Create a Duration struct (hours, minutes, seconds) and convert to total seconds.

<details>
<summary>Solution</summary>

```lwanga
struct Duration {
    hours: u32,
    minutes: u32,
    seconds: u32,
}

fn to_seconds(d: Duration) -> u64 {
    let h_secs: u64 = (d.hours as u64) * 3600;
    let m_secs: u64 = (d.minutes as u64) * 60;
    let s_secs: u64 = d.seconds as u64;
    
    return h_secs + m_secs + s_secs;
}

fn main() -> u64 {
    let duration: Duration = Duration {
        hours: 1,
        minutes: 30,
        seconds: 45,
    };
    
    return to_seconds(duration);  // Returns 5445
}
```
</details>

---

## Key Takeaways

In this lesson, you learned:

- ✅ How to define structs with multiple fields
- ✅ Creating and initializing struct instances
- ✅ Accessing and modifying struct fields
- ✅ Nested structs for complex data
- ✅ Passing structs to functions
- ✅ Returning structs from functions
- ✅ Common struct patterns and idioms
- ✅ Memory layout considerations
- ✅ Practical examples and exercises

---

## Next Lesson

Now that you understand structs, let's learn about packed structs for precise memory control in [Lesson 14: Packed Structs](14-packed-structs.md).

---

**Navigation**: [← Previous: Arrays](12-arrays.md) | [Tutorial Home](README.md) | [Next: Packed Structs →](14-packed-structs.md)
