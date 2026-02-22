# Lesson 13: Structs

**Learn how to create custom data types using structs.**

---

## Defining Structs

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

---

## Accessing Struct Fields

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
    
    return person_age;
}
```

---

## Structs with Pointers

```lwanga
struct User {
    name: ptr,
    age: u64,
}

fn main() -> u64 {
    let user: User = User {
        name: "Alice",
        age: 30,
    };
    
    return user.age;
}
```

---

## Nested Structs

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
    
    return person.addr.number as u64;
}
```

---

## Functions with Structs

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
    
    return calculate_area(r);  // Returns 200
}
```

---

**Navigation**: [← Previous: Arrays](12-arrays.md) | [Tutorial Home](README.md) | [Next: Packed Structs →](14-packed-structs.md)
