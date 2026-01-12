# C++ Operator Overloading Cheat Sheet

## Quick Reference Card

### Operator Categories at a Glance

| Category | Operators | Return Type | Member/Friend |
|----------|-----------|-------------|---------------|
| **Arithmetic Assignment** | `+=` `-=` `*=` `/=` `%=` | `Type&` | Member |
| **Binary Arithmetic** | `+` `-` `*` `/` `%` | `Type` | Friend/Non-member |
| **Increment/Decrement** | `++` `--` | `Type&` (prefix), `Type` (postfix) | Member |
| **Comparison** | `==` `!=` `<` `>` `<=` `>=` | `bool` | Member or Friend |
| **Subscript** | `[]` | `T&` or `const T&` | Member (MUST) |
| **Assignment** | `=` | `Type&` | Member (MUST) |
| **Stream** | `<<` `>>` | `stream&` | Friend (MUST) |
| **Unary** | `-` `!` `~` `+` | `Type` or `bool` | Member |
| **Logical** | `&&` `||` | `bool` | Avoid overloading |

---

## Essential Operator Templates

### 1. Assignment Operator
```cpp
Type& operator=(const Type& other) {
    if (this != &other) {
        // Copy members
    }
    return *this;
}
```
**✓ Enables:** `a = b = c;`

---

### 2. Compound Assignment (`+=` `-=` `*=`)
```cpp
Type& operator+=(const Type& other) {
    // Modify this
    return *this;
}
```
**✓ Enables:** `a += b += c;`

---

### 3. Binary Arithmetic (`+` `-` `*`)
```cpp
friend Type operator+(const Type& lhs, const Type& rhs) {
    Type result = lhs;
    result += rhs;
    return result;
}
```
**✓ Enables:** `a + b + c`

---

### 4. Scalar Multiplication (BOTH directions)
```cpp
friend Type operator*(const Type& v, int s) {
    Type result = v;
    result *= s;
    return result;
}

friend Type operator*(int s, const Type& v) {
    return v * s;
}
```
**✓ Enables:** `v * 2` AND `2 * v`

---

### 5. Prefix Increment/Decrement
```cpp
Type& operator++() {
    // Increment members
    return *this;
}
```
**✓ Returns:** Modified object  
**✓ Usage:** `++v`

---

### 6. Postfix Increment/Decrement
```cpp
Type operator++(int) {
    Type temp(*this);
    ++(*this);
    return temp;
}
```
**✓ Returns:** Old value  
**✓ Usage:** `v++`  
**✓ Note:** Dummy `int` parameter

---

### 7. Subscript Operator (TWO versions)
```cpp
T& operator[](size_t i) {
    return data[i];
}

const T& operator[](size_t i) const {
    return data[i];
}
```
**✓ Enables:** `v[0] = 5;` and `int x = constV[0];`

---

### 8. Comparison Operators
```cpp
bool operator==(const Type& other) const {
    return /* comparison */;
}

bool operator!=(const Type& other) const {
    return !(*this == other);
}
```
**✓ Const:** These don't modify objects

---

### 9. Unary Minus
```cpp
Type operator-() const {
    return Type(/* negated values */);
}
```
**✓ Enables:** `-v`

---

### 10. Stream Output
```cpp
friend std::ostream& operator<<(std::ostream& os, const Type& obj) {
    os << /* formatted output */;
    return os;
}
```
**✓ Enables:** `std::cout << obj << obj2;`

---

## Must-Know Rules

### 1. Return Types
| Operator | Return | Why |
|----------|--------|-----|
| `=` `+=` `-=` `*=` | `Type&` | Chaining + efficiency |
| `+` `-` `*` `/` | `Type` | New object |
| `++` `--` prefix | `Type&` | Modified object |
| `++` `--` postfix | `Type` | Old value |
| `==` `!=` `<` `>` | `bool` | Boolean result |
| `[]` | `T&` or `const T&` | Allow modification |
| `<<` `>>` | `stream&` | Chaining |

### 2. Parameter Passing
| Type | When | Example |
|------|------|---------|
| `const Type&` | Large objects, read-only | `operator+(const Type& other)` |
| `Type&` | Need to modify | `operator>>(istream& is, Type& obj)` |
| `Type` | Small primitives | `operator*(int scalar)` |

### 3. Member vs Non-Member
| **MUST be Member** | **MUST be Non-Member** | **Either Works** |
|-------------------|------------------------|------------------|
| `=` | `<<` `>>` (stream) | `+` `-` `*` `/` |
| `[]` | `scalar * object` | `==` `!=` |
| `()` | | `<` `>` `<=` `>=` |
| `->` | | |

---

## Common Patterns

### Pattern 1: Binary from Compound
```cpp
Type& operator+=(const Type& o) { /* modify */ return *this; }
Type operator+(const Type& a, const Type& b) { Type r=a; r+=b; return r; }
```

### Pattern 2: Postfix from Prefix
```cpp
Type& operator++() { /* increment */ return *this; }
Type operator++(int) { Type t(*this); ++(*this); return t; }
```

### Pattern 3: != from ==
```cpp
bool operator==(const Type& o) const { /* compare */ }
bool operator!=(const Type& o) const { return !(*this == o); }
```

---

## Critical Mistakes to Avoid

| ❌ WRONG | ✓ CORRECT | Issue |
|---------|----------|-------|
| `Type operator+=(...)` | `Type& operator+=(...)` | No chaining |
| `operator*(Type, int)` only | Both `operator*(Type,int)` and `operator*(int,Type)` | Can't do `2*v` |
| `int& operator[]()` only | Both const and non-const versions | Const objects fail |
| No `return *this;` | Always `return *this;` in `=`, `+=`, etc. | Breaks chaining |
| No self-assignment check | `if (this != &other)` in `operator=` | Potential bug |
| `operator++()` and `operator++()`  | `operator++()` and `operator++(int)` | Can't distinguish |

---

## Syntax Quick Reference

```cpp
// MEMBER function syntax
ReturnType operator@(parameters) [const] {
    // implementation
    return /* value */;
}

// NON-MEMBER (friend) syntax
friend ReturnType operator@(Type& lhs, Type& rhs) {
    // implementation
    return /* value */;
}

// Examples:
vect2& operator=(const vect2& other);              // Member
friend vect2 operator+(const vect2& a, const vect2& b);  // Friend
bool operator==(const vect2& other) const;         // Const member
```

---

## Complete vect2 Operator Checklist

- [ ] `vect2()` - Default constructor
- [ ] `vect2(int, int)` - Parameterized constructor
- [ ] `vect2(const vect2&)` - Copy constructor
- [ ] `operator=` - Assignment
- [ ] `operator[]` - Subscript (non-const)
- [ ] `operator[]` - Subscript (const)
- [ ] `operator+=` - Addition assignment
- [ ] `operator-=` - Subtraction assignment
- [ ] `operator*=` - Scalar multiplication assignment
- [ ] `operator+` - Binary addition
- [ ] `operator-` - Binary subtraction
- [ ] `operator*` - Scalar multiplication (v * scalar)
- [ ] `operator*` - Scalar multiplication (scalar * v)
- [ ] `operator++()` - Prefix increment
- [ ] `operator++(int)` - Postfix increment
- [ ] `operator--()` - Prefix decrement
- [ ] `operator--(int)` - Postfix decrement
- [ ] `operator-()` - Unary minus
- [ ] `operator==` - Equality
- [ ] `operator!=` - Inequality
- [ ] `operator<<` - Stream output

---

## Const Correctness Guide

```cpp
// Read-only methods → const
bool operator==(const vect2& other) const;
int operator[](size_t i) const;

// Parameters you don't modify → const reference
vect2& operator+=(const vect2& other);

// Return non-const reference for modification
int& operator[](size_t i);

// Return const reference for read-only
const int& operator[](size_t i) const;
```

---

## Prefix vs Postfix Quick Guide

```cpp
vect2 v(1, 2);

// Prefix: modify first, return new value
vect2 a = ++v;  // v = {2,3}, a = {2,3}

// Postfix: save old, modify, return old
vect2 b = v++;  // v = {3,4}, b = {2,3}
```

**Signature Difference:**
```cpp
Type& operator++();     // Prefix - no parameter
Type operator++(int);   // Postfix - dummy int parameter
```

---

## Operator Precedence (Relevant Subset)

| Precedence | Operators | Associativity |
|------------|-----------|---------------|
| 1 | `::` | Left-to-right |
| 2 | `++` `--` (postfix), `[]`, `()` | Left-to-right |
| 3 | `++` `--` (prefix), `+` `-` (unary), `!`, `~` | Right-to-left |
| 4 | `*` `/` `%` | Left-to-right |
| 5 | `+` `-` (binary) | Left-to-right |
| 6 | `<<` `>>` | Left-to-right |
| 7 | `<` `<=` `>` `>=` | Left-to-right |
| 8 | `==` `!=` | Left-to-right |
| 9 | `&&` | Left-to-right |
| 10 | `||` | Left-to-right |
| 11 | `=` `+=` `-=` `*=` etc. | Right-to-left |

---

## Testing Checklist

```cpp
// Constructors
vect2 v1;              // Default: {0, 0}
vect2 v2(1, 2);        // Parameterized: {1, 2}
vect2 v3(v2);          // Copy: {1, 2}
vect2 v4 = v2;         // Copy assignment: {1, 2}

// Subscript
v2[0] = 10;            // Non-const
int x = v2[1];         // Non-const or const
int y = v3[0];         // Const (if v3 is const)

// Arithmetic
v1 += v2;              // Compound assignment
v1 = v2 + v3;          // Binary addition
v1 = v2 - v3;          // Binary subtraction
v1 = v2 * 3;           // Scalar multiplication
v1 = 3 * v2;           // Reverse scalar multiplication

// Increment/Decrement
++v1;                  // Prefix increment
v1++;                  // Postfix increment
--v1;                  // Prefix decrement
v1--;                  // Postfix decrement

// Comparison
if (v1 == v2) {}       // Equality
if (v1 != v2) {}       // Inequality

// Unary
v1 = -v2;              // Negation

// Stream
std::cout << v1;       // Output

// Chaining
v1 = v2 = v3;          // Assignment chaining
v1 += v2 += v3;        // Compound chaining
std::cout << v1 << v2; // Stream chaining
```

---

## Memory Aid

**"RAC" for Member Operators**
- **R**eturn `Type&` for assignment/compound operators
- **A**lways return `*this`
- **C**haining support is the goal

**"NBC" for Binary Operators**
- **N**ew object created
- **B**y value return
- **C**ommutative operations need both orders

**"PIP" for Increment/Decrement**
- **P**refix: modify, return reference
- **I**nt parameter for postfix
- **P**ostfix: save, modify, return copy

---

## One-Line Reminders

- **Assignment** → `Type&`, check `this != &other`, return `*this`
- **Compound** → `Type&`, modify `*this`, return `*this`
- **Binary** → `Type`, use compound operator, return new object
- **Subscript** → Need BOTH const and non-const versions
- **Comparison** → `bool`, mark `const`, implement `!=` via `==`
- **Increment** → Prefix returns `Type&`, postfix returns `Type`
- **Stream** → Friend function, return `stream&`, first param is stream
- **Scalar** → Need BOTH `v*s` and `s*v` for commutativity

---

## Common Use Cases

```cpp
// Expression involving multiple operators
vect2 result = (v1 + v2) * 3 + v3 * 2 - v4;

// Chained assignments
v1 = v2 = v3 = vect2(5, 5);

// Compound chaining
v1 += v2 += v3;

// Prefix in loop
for (vect2 v; v[0] < 10; ++v) { /* ... */ }

// Stream output
std::cout << "Vector: " << v1 << std::endl;

// Comparison in conditional
if (v1 == v2 || v1 != v3) { /* ... */ }
```

---

## Compilation Command

```bash
# Compile
g++ -std=c++17 -Wall -Wextra main.cpp vect2.cpp -o vect2_program

# Run
./vect2_program
```

---

## Quick Debug Tips

1. **Chaining doesn't work?** → Check return type is reference
2. **`2 * v` fails?** → Missing `operator*(int, const Type&)`
3. **Const object can't use `[]`?** → Missing const version of `operator[]`
4. **Increment gives wrong value?** → Check prefix vs postfix implementation
5. **Assignment crashes?** → Missing self-assignment check

---

## Pro Tips

- ✅ Implement binary operators using compound operators
- ✅ Implement postfix using prefix operators
- ✅ Implement `!=` using `==`
- ✅ Mark all read-only methods as `const`
- ✅ Use friend for symmetric operations
- ✅ Always test with const objects
- ✅ Test operator chaining
- ✅ Use references to avoid unnecessary copies

