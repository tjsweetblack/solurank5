# C++ Operator Overloading Reference

## Complete Operator Reference

### Operator Overloadability

#### Operators That CAN Be Overloaded
```
+    -    *    /    %    ^    &    |    ~    !    
=    <    >    +=   -=   *=   /=   %=   ^=   &=   |=  
<<   >>   >>=  <<=  ==   !=   <=   >=   <=>  &&   ||  
++   --   ,    ->*  ->   ()   []   new  delete
new[] delete[]
```

#### Operators That CANNOT Be Overloaded
```
::   .*   ?:   sizeof   typeid   alignof
```

---

## Operator Syntax Reference

### 1. Assignment Operator
```cpp
// Signature
Type& operator=(const Type& other);

// Implementation Template
Type& Type::operator=(const Type& other) {
    if (this != &other) {
        // Copy data from other to this
    }
    return *this;
}

// Usage
obj1 = obj2;
obj1 = obj2 = obj3;  // Chaining
```

---

### 2. Arithmetic Assignment Operators

#### Addition Assignment
```cpp
// Signature
Type& operator+=(const Type& other);

// Implementation
Type& Type::operator+=(const Type& other) {
    // Add other to this
    return *this;
}

// Usage
v1 += v2;
v1 += v2 += v3;  // Chaining
```

#### Subtraction Assignment
```cpp
Type& operator-=(const Type& other);
```

#### Multiplication Assignment (by scalar)
```cpp
Type& operator*=(int scalar);
```

#### Division Assignment
```cpp
Type& operator/=(int scalar);
```

---

### 3. Binary Arithmetic Operators

```cpp
// Signatures (as friend functions)
friend Type operator+(const Type& lhs, const Type& rhs);
friend Type operator-(const Type& lhs, const Type& rhs);
friend Type operator*(const Type& lhs, const Type& rhs);
friend Type operator/(const Type& lhs, const Type& rhs);

// Implementation Template
Type operator+(const Type& lhs, const Type& rhs) {
    Type result = lhs;
    result += rhs;
    return result;
}

// Usage
Type result = obj1 + obj2;
Type result = obj1 + obj2 + obj3;
```

---

### 4. Scalar Multiplication

```cpp
// Both directions needed for commutativity
friend Type operator*(const Type& vec, int scalar);
friend Type operator*(int scalar, const Type& vec);

// Implementation
Type operator*(const Type& vec, int scalar) {
    Type result = vec;
    result *= scalar;
    return result;
}

Type operator*(int scalar, const Type& vec) {
    return vec * scalar;  // Reuse above
}

// Usage
vect2 result1 = v * 3;   // Uses first overload
vect2 result2 = 3 * v;   // Uses second overload
```

---

### 5. Increment and Decrement

#### Prefix (++v, --v)
```cpp
// Signatures
Type& operator++();
Type& operator--();

// Implementation
Type& Type::operator++() {
    // Increment this
    return *this;
}

// Usage
++obj;
Type result = ++obj;  // result gets new value
```

#### Postfix (v++, v--)
```cpp
// Signatures (note the dummy int parameter)
Type operator++(int);
Type operator--(int);

// Implementation
Type Type::operator++(int) {
    Type temp(*this);  // Save old value
    ++(*this);         // Use prefix operator
    return temp;       // Return old value
}

// Usage
obj++;
Type result = obj++;  // result gets old value
```

---

### 6. Comparison Operators

```cpp
// Signatures
bool operator==(const Type& other) const;
bool operator!=(const Type& other) const;
bool operator<(const Type& other) const;
bool operator<=(const Type& other) const;
bool operator>(const Type& other) const;
bool operator>=(const Type& other) const;

// Implementation
bool Type::operator==(const Type& other) const {
    // Compare members
    return /* condition */;
}

bool Type::operator!=(const Type& other) const {
    return !(*this == other);
}

// Usage
if (obj1 == obj2) { /* ... */ }
if (obj1 != obj2) { /* ... */ }
```

---

### 7. Subscript Operator

```cpp
// Signatures (both needed)
ElementType& operator[](size_t index);
const ElementType& operator[](size_t index) const;

// Implementation
ElementType& Type::operator[](size_t index) {
    return data[index];
}

const ElementType& Type::operator[](size_t index) const {
    return data[index];
}

// Usage
obj[0] = 42;           // Non-const version
int x = obj[0];        // Either version
int y = constObj[0];   // Const version only
```

---

### 8. Unary Operators

#### Unary Minus
```cpp
// Signature
Type operator-() const;

// Implementation
Type Type::operator-() const {
    Type result;
    // Negate members
    return result;
}

// Usage
Type negated = -obj;
```

#### Logical NOT
```cpp
bool operator!() const;
```

#### Bitwise NOT
```cpp
Type operator~() const;
```

---

### 9. Stream Operators

#### Output (<<)
```cpp
// Signature
friend std::ostream& operator<<(std::ostream& os, const Type& obj);

// Implementation
std::ostream& operator<<(std::ostream& os, const Type& obj) {
    os << /* formatted output */;
    return os;
}

// Usage
std::cout << obj;
std::cout << obj1 << " " << obj2 << std::endl;
```

#### Input (>>)
```cpp
// Signature
friend std::istream& operator>>(std::istream& is, Type& obj);

// Implementation
std::istream& operator>>(std::istream& is, Type& obj) {
    is >> /* read into obj */;
    return is;
}

// Usage
std::cin >> obj;
```

---

### 10. Function Call Operator

```cpp
// Signature
ReturnType operator()(ParamType1 param1, ParamType2 param2);

// Implementation
ReturnType Type::operator()(ParamType1 param1, ParamType2 param2) {
    // Function body
    return result;
}

// Usage
Type obj;
obj(arg1, arg2);  // Calls operator()
```

---

### 11. Type Conversion Operators

#### Explicit Conversion
```cpp
// Signature
explicit operator TargetType() const;

// Implementation
explicit Type::operator int() const {
    return /* conversion logic */;
}

// Usage
int x = static_cast<int>(obj);
```

#### Implicit Conversion
```cpp
// Signature (without explicit)
operator TargetType() const;

// Implementation
Type::operator int() const {
    return /* conversion logic */;
}

// Usage
int x = obj;  // Implicit conversion
```

---

## Member vs Non-Member Decision Table

| Operator | Member | Non-Member | Notes |
|----------|--------|------------|-------|
| `=` | ✓ | ✗ | Must be member |
| `[]` | ✓ | ✗ | Must be member |
| `()` | ✓ | ✗ | Must be member |
| `->` | ✓ | ✗ | Must be member |
| `+=`, `-=`, etc. | ✓ | Possible | Usually member |
| `++`, `--` (unary) | ✓ | Possible | Usually member |
| `+`, `-`, etc. (binary) | Possible | ✓ | Usually non-member |
| `<<`, `>>` (stream) | ✗ | ✓ | Must be non-member |
| `==`, `!=`, `<`, etc. | ✓ | ✓ | Either works |
| Unary `-`, `!`, `~` | ✓ | Possible | Usually member |

---

## Return Type Guidelines

| Operator Category | Return Type | Reason |
|-------------------|-------------|---------|
| Assignment `=` | `Type&` | Enable chaining, return modified object |
| Compound Assignment `+=`, `-=`, etc. | `Type&` | Enable chaining, return modified object |
| Binary Arithmetic `+`, `-`, etc. | `Type` | Create new object, don't modify operands |
| Prefix Increment/Decrement | `Type&` | Return modified object |
| Postfix Increment/Decrement | `Type` | Return old value before modification |
| Comparison `==`, `<`, etc. | `bool` | Boolean result |
| Subscript `[]` | `ElementType&` | Allow modification |
| Subscript `[]` (const) | `const ElementType&` | Read-only access |
| Stream `<<`, `>>` | `stream&` | Enable chaining |
| Unary `-`, `!` | `Type` or `bool` | Create new object or boolean |

---

## Parameter Passing Guidelines

| Parameter Type | When to Use | Example |
|----------------|-------------|---------|
| `const Type&` | Passing large objects, not modifying | `operator+(const Type& other)` |
| `Type&` | Modifying the parameter | `operator>>(std::istream& is, Type& obj)` |
| `Type` | Small objects, need a copy | Scalars: `operator*(int scalar)` |
| `const Type` | Small objects, const semantics | Rarely used |

---

## Special Considerations

### 1. Symmetric Operators
For commutative operations, implement both orders:
```cpp
friend Type operator*(const Type& vec, int scalar);
friend Type operator*(int scalar, const Type& vec);
```

### 2. Const Overloads
For operators that should work with const objects:
```cpp
int& operator[](size_t index);              // Non-const version
const int& operator[](size_t index) const;  // Const version
```

### 3. Friend Functions
Declare as friend when needing access to private members:
```cpp
class Type {
    friend std::ostream& operator<<(std::ostream& os, const Type& obj);
    friend Type operator+(const Type& lhs, const Type& rhs);
};
```

### 4. Chaining Support
Always return reference for operators that should support chaining:
```cpp
Type& operator=(const Type& other) { /* ... */ return *this; }
Type& operator+=(const Type& other) { /* ... */ return *this; }
std::ostream& operator<<(std::ostream& os, const Type& obj) { /* ... */ return os; }
```

---

## Common Patterns

### Pattern 1: Canonical Form
```cpp
class Type {
public:
    // Constructor
    Type();
    Type(/* params */);
    
    // Copy constructor
    Type(const Type& other);
    
    // Assignment operator
    Type& operator=(const Type& other);
    
    // Destructor
    ~Type();
};
```

### Pattern 2: Implementing Binary via Compound
```cpp
// Step 1: Implement compound assignment
Type& Type::operator+=(const Type& other) {
    // Modify this
    return *this;
}

// Step 2: Implement binary using compound
Type operator+(const Type& lhs, const Type& rhs) {
    Type result = lhs;
    result += rhs;
    return result;
}
```

### Pattern 3: Implementing Postfix via Prefix
```cpp
// Step 1: Implement prefix
Type& Type::operator++() {
    // Modify this
    return *this;
}

// Step 2: Implement postfix using prefix
Type Type::operator++(int) {
    Type temp(*this);
    ++(*this);
    return temp;
}
```

---

## Quick Syntax Reference

```cpp
// Member function operators
ReturnType operator@(parameters) [const];

// Non-member function operators
ReturnType operator@(Type& lhs, Type& rhs);

// Friend declaration
friend ReturnType operator@(parameters);

// Examples:
vect2& operator+=(const vect2& other);                    // Member
friend vect2 operator+(const vect2& a, const vect2& b);   // Friend
bool operator==(const vect2& other) const;                // Const member
```

---

## C++20 Three-Way Comparison (Spaceship Operator)

```cpp
#include <compare>

// Signature
std::strong_ordering operator<=>(const Type& other) const = default;

// Automatically generates: ==, !=, <, <=, >, >=

// Manual implementation
std::strong_ordering operator<=>(const Type& other) const {
    if (auto cmp = member1 <=> other.member1; cmp != 0)
        return cmp;
    return member2 <=> other.member2;
}
```

---

## Compilation and Linking

### Header File (.hpp)
```cpp
#ifndef TYPE_HPP
#define TYPE_HPP

class Type {
    // Declarations only
    Type& operator+=(const Type& other);
    friend Type operator+(const Type& lhs, const Type& rhs);
};

#endif
```

### Implementation File (.cpp)
```cpp
#include "Type.hpp"

// Definitions
Type& Type::operator+=(const Type& other) {
    // Implementation
    return *this;
}

Type operator+(const Type& lhs, const Type& rhs) {
    // Implementation
    return result;
}
```

