# C++ Operator Overloading Study Guide
## Based on vect2 Implementation

## Table of Contents
1. [Introduction to Operator Overloading](#introduction)
2. [vect2 Class Overview](#vect2-overview)
3. [Constructor Types](#constructors)
4. [Operator Categories](#operator-categories)
5. [Implementation Patterns](#implementation-patterns)
6. [Best Practices](#best-practices)
7. [Common Mistakes](#common-mistakes)

---

## Introduction to Operator Overloading

Operator overloading allows you to define custom behavior for operators when used with user-defined types (classes). This makes your classes behave like built-in types, improving code readability and intuitiveness.

### When to Use Operator Overloading
- When the operation has a clear, intuitive meaning
- When it makes code more readable than function calls
- When working with mathematical or container-like classes

### When NOT to Use
- When the operation's meaning is not obvious
- When it could confuse users of your class
- For operations that don't follow conventional semantics

---

## vect2 Class Overview

The `vect2` class represents a 2D mathematical vector with integer components (x, y).

### Class Structure
```cpp
class vect2 {
private:
    int x;
    int y;
public:
    // Constructors and operators...
};
```

### Key Features
- Two integer components: `x` and `y`
- Full arithmetic support (addition, subtraction, multiplication)
- Component-wise operations between vectors
- Scalar multiplication support
- Array-style access via `[]` operator
- Comparison operators
- Stream output support

---

## Constructors

### Default Constructor
```cpp
vect2::vect2() {
    this->x = 0;
    this->y = 0;
}
```
- **Purpose**: Creates object with default values (0, 0)
- **Called**: `vect2 v1;` (creates a zero vector)
- **Best Practice**: Initialize all members to sensible defaults
- **Uses `this` pointer**: Explicitly references current object's members

### Parameterized Constructor
```cpp
vect2::vect2(int num1, int num2) {
    this->x = num1;
    this->y = num2;
}
```
- **Purpose**: Creates object with specific values
- **Called**: `vect2 v2(1, 2);` creates vector {1, 2}
- **Parameters**: `num1` for x-component, `num2` for y-component
- **Note**: Direct member initialization using `this` pointer

### Copy Constructor
```cpp
vect2::vect2(const vect2& source) {
    *this = source;
}
```
- **Purpose**: Creates a new object as a copy of existing one
- **Called**: `vect2 v3(v2);` or `vect2 v4 = v2;` or when passing by value
- **Parameter**: `const vect2&` (reference to avoid infinite recursion)
- **Implementation**: Delegates to assignment operator (`operator=`)
- **Important Pattern**: Reusing existing code (assignment operator) for consistency

---

## Operator Categories

### 1. Assignment Operator
```cpp
vect2& vect2::operator=(const vect2& source) {
    if(this != &source) {  // Prevent self-assignment
        this->x = source.x;
        this->y = source.y;
    }
    return(*this);
}
```

**Key Points:**
- **Return Type**: `vect2&` (reference to current object)
- **Self-Assignment Check**: `if (this != &source)` prevents bugs when `v1 = v1`
- **Return**: `*this` to enable chaining (`a = b = c`)
- **Member Function**: Must be a member, cannot be non-member or friend
- **Explicit parentheses**: Uses `return(*this)` style

**Usage:**
```cpp
vect2 v1, v2(1, 2);
v1 = v2;           // Calls operator=
v1 = v2 = vect2(3, 4);  // Chaining works
```

**Why Self-Assignment Check Matters:**
```cpp
vect2 v(5, 10);
v = v;  // Without check, could cause issues with dynamic memory
```

---

### 2. Arithmetic Assignment Operators

These modify the current object and return a reference.

#### Addition Assignment (Component-wise)
```cpp
vect2& vect2::operator+=(const vect2& obj) {
    this->x += obj.x;
    this->y += obj.y;
    return(*this);
}
```
- **Component-wise operation**: Adds each component separately
- **Example**: `{2,3} += {1,2}` becomes `{3,5}`

#### Subtraction Assignment (Component-wise)
```cpp
vect2& vect2::operator-=(const vect2& obj) {
    this->x -= obj.x;
    this->y -= obj.y;
    return(*this);
}
```
- **Example**: `{5,7} -= {2,3}` becomes `{3,4}`

#### Scalar Multiplication Assignment
```cpp
vect2& vect2::operator*=(int num) {
    this->x *= num;
    this->y *= num;
    return(*this);
}
```
- **Scalar operation**: Multiplies both components by the same number
- **Example**: `{2,3} *= 5` becomes `{10,15}`

#### Vector Multiplication Assignment (Component-wise)
```cpp
vect2& vect2::operator*=(const vect2& obj) {
    this->x *= obj.x;
    this->y *= obj.y;
    return(*this);
}
```
- **Component-wise multiplication**: `{2,3} *= {4,5}` becomes `{8,15}`
- **Note**: This is element-wise multiplication, not dot or cross product

**Key Points:**
- **Return Type**: `vect2&` for chaining
- **Modify**: Current object (left operand) using `this` pointer
- **Return**: `*this` with parentheses style
- **Enables**: `v1 += v2 += v3;`
- **Direct member access**: Uses `this->x` and `this->y`

---

### 3. Binary Arithmetic Operators

These create and return new objects without modifying operands.

#### Binary Addition (Member Function)
```cpp
vect2 vect2::operator+(const vect2& obj) const {
    vect2 temp = *this;
    temp.x += obj.x;
    temp.y += obj.y;
    return(temp);
}
```
- **Member function**: Different from typical friend implementation
- **Creates temp copy**: Copies current object, modifies it, returns it
- **Const member**: Doesn't modify the original objects
- **Usage**: `vect2 result = v1 + v2;`

#### Binary Subtraction (Member Function)
```cpp
vect2 vect2::operator-(const vect2& obj) const {
    vect2 temp = *this;
    temp.x -= obj.x;
    temp.y -= obj.y;
    return(temp);
}
```

#### Binary Vector Multiplication (Member Function)
```cpp
vect2 vect2::operator*(const vect2& obj) const {
    vect2 temp = *this;
    temp.x *= obj.x;
    temp.y *= obj.y;
    return(temp);
}
```
- **Component-wise multiplication**: `{2,3} * {4,5}` = `{8,15}`

#### Scalar Multiplication (Member Function)
```cpp
vect2 vect2::operator*(int num) const {
    vect2 temp;
    temp.x = this->x * num;
    temp.y = this->y * num;
    return(temp);
}
```
- **For `v * 2` syntax**: Member function handles vector on left
- **Creates new temp object**: Returns modified copy

#### Scalar Multiplication Reversed (Non-Member Function)
```cpp
vect2 operator*(int num, const vect2& obj) {
    vect2 temp(obj);
    temp *= num;
    return(temp);
}
```
- **For `2 * v` syntax**: Non-member handles scalar on left
- **Uses copy constructor**: Creates temp from obj
- **Reuses `operator*=`**: Efficient code reuse pattern
- **Required for commutativity**: Allows both `v * 2` and `2 * v`

**Key Points:**
- **Return Type**: `vect2` (new object by value)
- **Member Functions**: Most are members, not friends (different from typical pattern)
- **Const members**: Mark with `const` since they don't modify the object
- **Pattern**: Create temp, modify temp, return temp

---

### 4. Increment and Decrement Operators

#### Prefix Increment/Decrement
```cpp
vect2& vect2::operator++() {
    this->x += 1;
    this->y += 1;
    return(*this);
}

vect2& vect2::operator--() {
    this->x -= 1;
    this->y -= 1;
    return(*this);
}
```

**Behavior:**
- **Modify both components**: Adds/subtracts 1 from both x and y
- **Return reference**: Returns modified object for chaining
- **`++v` returns new value**: The incremented value is returned
- **Example**: `{2,3}` becomes `{3,4}` after `++v`

#### Postfix Increment/Decrement
```cpp
vect2 vect2::operator++(int) {
    vect2 temp = *this;  // Save current state
    ++(*this);           // Use prefix operator
    return(temp);        // Return old state
}

vect2 vect2::operator--(int) {
    vect2 temp = *this;
    --(*this);
    return(temp);
}
```

**Behavior:**
- **Save current state**: Copy assignment creates temp
- **Modify the object**: Calls prefix version
- **Return old state**: Returns the copy before modification
- **`v++` returns old value**: The pre-increment value
- **Dummy `int` parameter**: Distinguishes from prefix (never used)

**Important Difference:**
```cpp
vect2 v(1, 2);
vect2 a = ++v;  // v = {2,3}, a = {2,3}  (both get new value)
vect2 b = v++;  // v = {3,4}, b = {2,3}  (b gets old, v has new)
```

**Code Reuse Pattern:**
- Postfix implementation calls prefix version
- Avoids code duplication
- Ensures consistent behavior

---

### 5. Subscript Operator

```cpp
int vect2::operator[](int index) const {
    if(index == 0)
        return(this->x);
    return(this->y);
}

int& vect2::operator[](int index) {
    if(index == 0)
        return(this->x);
    return(this->y);
}
```

**Key Points:**
- **Two Versions Required**: const and non-const
- **Non-const version**: Returns `int&` for modification: `v[0] = 5;`
- **Const version**: Returns `int` (by value) for const objects
- **Index logic**: 0 returns x, anything else returns y
- **No Bounds Checking**: Index 0 = x, index 1 (or any other) = y
- **Design choice**: Any non-zero index returns y component

**Usage:**
```cpp
vect2 v(10, 20);
std::cout << v[0];  // Prints 10 (calls const or non-const version)
v[1] = 30;          // Modifies y to 30 (calls non-const version)

const vect2 cv(5, 7);
int x = cv[0];      // Calls const version, returns 5
// cv[0] = 10;      // ERROR: cannot modify const object
```

**Important Difference from Typical Implementation:**
- **Const version returns by value**: `int` instead of `const int&`
- **Reason**: Simple int type, no performance penalty
- **Non-const returns reference**: Allows modification

**Why Two Versions:**
```cpp
void printVector(const vect2& v) {
    std::cout << v[0] << ", " << v[1];  // Needs const version
}

void modifyVector(vect2& v) {
    v[0] = 100;  // Needs non-const version returning reference
}
```

---

### 6. Comparison Operators

```cpp
bool vect2::operator==(const vect2& obj) const {
    if((this->x == obj.x) && (this->y == obj.y))
        return(true);
    return(false);
}

bool vect2::operator!=(const vect2& obj) const {
    return(!(obj == *this));
}
```

**Key Points:**
- **Return Type**: `bool`
- **Const Members**: Don't modify the objects being compared
- **Equality**: Compares both x and y components
- **Inequality**: Implemented using equality operator (code reuse)
- **Parameter order**: `obj == *this` instead of `*this == obj` (both work)

**Implementation Details:**
- **Explicit if-statement style** in `operator==`
- **Could be simplified** to: `return (this->x == obj.x) && (this->y == obj.y);`
- **Inequality uses negation**: `!(obj == *this)` delegates to `operator==`

**Usage:**
```cpp
vect2 v1(1, 2);
vect2 v2(1, 2);
vect2 v3(3, 4);

if (v1 == v2) {  // true - both components match
    std::cout << "Equal\n";
}

if (v1 != v3) {  // true - components differ
    std::cout << "Not equal\n";
}

std::cout << (v1 == v2);  // Prints 1 (true)
std::cout << (v1 != v3);  // Prints 1 (true)
```

**Pattern: Implementing != using ==**
- Avoids code duplication
- Changes to equality logic automatically apply to inequality
- Common best practice in C++ operator overloading

---

### 7. Unary Operators

```cpp
vect2 vect2::operator-() const {
    vect2 temp = *this;
    temp[0] = -temp[0];
    temp[1] = -temp[1];
    return(temp);
}
```

**Key Points:**
- **Return Type**: New `vect2` object (by value)
- **Const Member**: Doesn't modify original object
- **Creates copy**: Uses copy assignment `temp = *this`
- **Uses subscript operator**: Clever use of `operator[]` to access/modify
- **Returns negated copy**: Both components have opposite sign

**Usage:**
```cpp
vect2 v(3, -5);
vect2 negated = -v;  // negated = {-3, 5}
std::cout << -v;     // Prints {-3, 5}
// v is still {3, -5} - original unchanged
```

**Implementation Pattern:**
1. Create a copy of current object
2. Negate both components using subscript operator
3. Return the negated copy

**Why Use Subscript Operator:**
- **Code reuse**: Leverages existing `operator[]`
- **Could also write**: `temp.x = -temp.x; temp.y = -temp.y;`
- **Demonstrates operator composition**: One operator using another

---

### 8. Stream Insertion Operator

```cpp
std::ostream& operator<<(std::ostream& os, const vect2& obj) {
    std::cout << "{" << obj[0] << ", " << obj[1] << "}";
    return(os);
}
```

**Key Points:**
- **Non-Member Function**: Not a member of vect2 class
- **First Parameter**: `std::ostream&` (the output stream)
- **Second Parameter**: `const vect2&` (object to print)
- **Return Type**: `std::ostream&` for chaining
- **Uses subscript operator**: Calls `obj[0]` and `obj[1]` to access components
- **Accesses via public interface**: Uses `operator[]` instead of direct member access
- **Output to std::cout**: Directly uses cout, not the parameter `os`

**Enables Chaining:**
```cpp
vect2 v1(1, 2), v2(3, 4);
std::cout << v1 << " " << v2 << std::endl;
// Prints: {1, 2} {3, 4}
```

**Format:**
- Opening brace `{`
- First component (x)
- Comma and space `, `
- Second component (y)
- Closing brace `}`

**Usage Examples:**
```cpp
vect2 v(10, 20);
std::cout << "v: " << v << std::endl;
// Output: v: {10, 20}

std::cout << v + v << std::endl;
// Output: {20, 40}

std::cout << -v << std::endl;
// Output: {-10, -20}
```

**Design Note:**
- **Uses `std::cout` directly** instead of parameter `os`
- **More typical**: `os << "{" << obj[0] << ", " << obj[1] << "}";`
- **Still works**: Because function returns `os`, and cout is the typical stream passed

---

## Implementation Patterns

### Pattern 1: Implement Binary in Terms of Compound
```cpp
// First implement +=
vect2& vect2::operator+=(const vect2& other) {
    components[0] += other.components[0];
    components[1] += other.components[1];
    return *this;
}

// Then implement + using +=
vect2 operator+(const vect2& lhs, const vect2& rhs) {
    vect2 result = lhs;
    result += rhs;
    return result;
}
```

**Benefits:**
- Reduces code duplication
- Easier to maintain
- Less chance of bugs

### Pattern 2: Implement Postfix in Terms of Prefix
```cpp
// First implement prefix
vect2& vect2::operator++() {
    ++components[0];
    ++components[1];
    return *this;
}

// Then implement postfix using prefix
vect2 vect2::operator++(int) {
    vect2 temp(*this);
    ++(*this);  // Use prefix
    return temp;
}
```

### Pattern 3: Implement != in Terms of ==
```cpp
bool vect2::operator==(const vect2& other) const {
    return components[0] == other.components[0] && 
           components[1] == other.components[1];
}

bool vect2::operator!=(const vect2& other) const {
    return !(*this == other);  // Reuse ==
}
```

---

## Best Practices

### 1. Return Types
- **Compound Assignment** (`+=`, `-=`, `*=`): Return `Type&`
- **Binary Operators** (`+`, `-`, `*`): Return `Type` (by value)
- **Increment/Decrement Prefix**: Return `Type&`
- **Increment/Decrement Postfix**: Return `Type` (by value)
- **Comparison**: Return `bool`
- **Stream Operators**: Return `std::ostream&` or `std::istream&`

### 2. Parameter Passing
- **Pass by const reference** when not modifying: `const Type&`
- **Avoid copies**: Use references unless you need a copy
- **Const correctness**: Mark methods that don't modify as `const`

### 3. Member vs Non-Member
**Must be Members:**
- Assignment `=`
- Subscript `[]`
- Function call `()`
- Member access `->`
- Type conversion operators

**Typically Non-Members (Friends):**
- Binary arithmetic `+`, `-`, `*`, `/`
- Stream operators `<<`, `>>`
- When left operand is not your class (e.g., `2 * vector`)

### 4. Const Correctness
```cpp
// Good: const member function
bool operator==(const vect2& other) const;

// Good: const reference parameter
vect2& operator+=(const vect2& other);

// Good: both versions for subscript
int& operator[](int index);
const int& operator[](int index) const;
```

### 5. Enable Chaining
```cpp
vect2& operator=(const vect2& other) {
    // ... assignment logic ...
    return *this;  // Enables a = b = c
}
```

---

## Common Mistakes

### ❌ Mistake 1: Wrong Return Type for Compound Assignment
```cpp
// WRONG
vect2 operator+=(const vect2& other);

// RIGHT
vect2& operator+=(const vect2& other);
```

### ❌ Mistake 2: Forgetting Self-Assignment Check
```cpp
// WRONG
vect2& operator=(const vect2& other) {
    delete[] data;
    data = new int[other.size];
    // ... if other is this, we just deleted our source!
}

// RIGHT
vect2& operator=(const vect2& other) {
    if (this != &other) {
        // ... safe assignment
    }
    return *this;
}
```

### ❌ Mistake 3: Missing const Version of operator[]
```cpp
// WRONG - only non-const version
int& operator[](int index);

// RIGHT - both versions
int& operator[](int index);
const int& operator[](int index) const;
```

### ❌ Mistake 4: Only One Direction for Scalar Multiplication
```cpp
// WRONG - only v * 2 works, not 2 * v
vect2 operator*(const vect2& vec, int scalar);

// RIGHT - both directions
vect2 operator*(const vect2& vec, int scalar);
vect2 operator*(int scalar, const vect2& vec);
```

### ❌ Mistake 5: Not Returning *this
```cpp
// WRONG
vect2& operator+=(const vect2& other) {
    components[0] += other.components[0];
    components[1] += other.components[1];
    // Missing return!
}

// RIGHT
vect2& operator+=(const vect2& other) {
    components[0] += other.components[0];
    components[1] += other.components[1];
    return *this;
}
```

### ❌ Mistake 6: Prefix/Postfix Confusion
```cpp
// WRONG - both have same signature
vect2& operator++();
vect2& operator++();

// RIGHT - postfix has dummy int parameter
vect2& operator++();     // prefix
vect2 operator++(int);   // postfix
```

---

## Summary

**Key Takeaways:**
1. **Operator overloading** makes custom types behave like built-in types
2. **Return references** (`Type&`) for operators that modify and support chaining
3. **Return by value** (`Type`) for operators that create new objects
4. **Const correctness** is crucial for usability and correctness
5. **Implement binary operators** in terms of compound assignment
6. **Both versions needed**: const and non-const for subscript, both orders for commutative operations
7. **Friend functions** when left operand is not your class type

**Testing Your Understanding:**
- Can you explain why `operator+=` returns a reference?
- Why do we need both `v * 2` and `2 * v` overloads?
- What's the difference between prefix and postfix increment?
- Why does `operator[]` need two versions?
- When should an operator be a member vs a friend?

