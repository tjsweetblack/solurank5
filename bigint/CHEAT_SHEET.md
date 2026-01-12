# BigInt Cheat Sheet

Quick reference for implementing the **bigint** class - arbitrary precision unsigned integers.

---

## Class Definition

```cpp
class bigint {
private:
    std::string str;  // Store number as string
public:
    bigint();
    bigint(unsigned int num);
    bigint(const bigint& source);
    
    std::string getStr() const;
    bigint& operator=(const bigint& source);
    
    // Addition
    bigint operator+(const bigint& other) const;
    bigint& operator+=(const bigint& other);
    
    // Increment
    bigint& operator++();     // Prefix
    bigint operator++(int);   // Postfix
    
    // Shift (unsigned int)
    bigint operator<<(unsigned int n) const;
    bigint operator>>(unsigned int n) const;
    bigint& operator<<=(unsigned int n);
    bigint& operator>>=(unsigned int n);
    
    // Shift (bigint)
    bigint operator<<(const bigint& other) const;
    bigint operator>>(const bigint& other) const;
    bigint& operator<<=(const bigint& other);
    bigint& operator>>=(const bigint& other);
    
    // Comparison
    bool operator==(const bigint& other) const;
    bool operator!=(const bigint& other) const;
    bool operator<(const bigint& other) const;
    bool operator>(const bigint& other) const;
    bool operator<=(const bigint& other) const;
    bool operator>=(const bigint& other) const;
};

std::ostream& operator<<(std::ostream& output, const bigint& obj);
```

---

## Essential Code Snippets

### Constructors

```cpp
// Default
bigint::bigint() {
    str = "0";
}

// From unsigned int
bigint::bigint(unsigned int num) {
    std::stringstream ss;
    ss << num;
    str = ss.str();
}

// Copy
bigint::bigint(const bigint& source) {
    *this = source;
}

// Assignment
bigint& bigint::operator=(const bigint& source) {
    if (this == &source)
        return *this;
    str = source.str;
    return *this;
}

// Getter
std::string bigint::getStr() const {
    return str;
}
```

---

### Helper Functions

```cpp
// Reverse string
std::string reverse(const std::string& str) {
    std::string result;
    for (size_t i = str.length(); i > 0; i--)
        result.push_back(str[i - 1]);
    return result;
}

// String to unsigned int
unsigned int stringToUINT(std::string str) {
    std::stringstream ss(str);
    unsigned int res;
    ss >> res;
    return res;
}

// Addition algorithm
std::string addition(const bigint& obj1, const bigint& obj2) {
    std::string str1 = reverse(obj1.getStr());
    std::string str2 = reverse(obj2.getStr());
    
    // Pad to same length
    while (str1.length() < str2.length())
        str1.push_back('0');
    while (str2.length() < str1.length())
        str2.push_back('0');
    
    std::string result;
    int carry = 0;
    
    for (size_t i = 0; i < str1.length(); i++) {
        int sum = (str1[i] - '0') + (str2[i] - '0') + carry;
        result.push_back((sum % 10) + '0');
        carry = sum / 10;
    }
    
    if (carry)
        result.push_back(carry + '0');
    
    return reverse(result);
}
```

---

### Addition

```cpp
// Binary addition
bigint bigint::operator+(const bigint& other) const {
    bigint temp;
    temp.str = addition(*this, other);
    return temp;
}

// Compound addition
bigint& bigint::operator+=(const bigint& other) {
    *this = *this + other;
    return *this;
}
```

---

### Increment

```cpp
// Prefix: increment then return
bigint& bigint::operator++() {
    *this = *this + bigint(1);
    return *this;
}

// Postfix: save, increment, return old
bigint bigint::operator++(int) {
    bigint temp = *this;
    *this = *this + bigint(1);
    return temp;
}
```

---

### Shift Operations (unsigned int)

```cpp
// Left shift: append zeros
bigint bigint::operator<<(unsigned int n) const {
    bigint temp = *this;
    temp.str.insert(temp.str.end(), n, '0');
    return temp;
}

// Right shift: remove digits
bigint bigint::operator>>(unsigned int n) const {
    bigint temp = *this;
    if (n >= temp.str.length())
        temp.str = "0";
    else
        temp.str.erase(temp.str.length() - n, n);
    return temp;
}

// Compound shifts
bigint& bigint::operator<<=(unsigned int n) {
    *this = *this << n;
    return *this;
}

bigint& bigint::operator>>=(unsigned int n) {
    *this = *this >> n;
    return *this;
}
```

---

### Shift Operations (bigint)

```cpp
// Convert bigint to uint and use uint version
bigint bigint::operator<<(const bigint& other) const {
    return *this << stringToUINT(other.str);
}

bigint bigint::operator>>(const bigint& other) const {
    return *this >> stringToUINT(other.str);
}

bigint& bigint::operator<<=(const bigint& other) {
    *this = *this << stringToUINT(other.str);
    return *this;
}

bigint& bigint::operator>>=(const bigint& other) {
    *this = *this >> stringToUINT(other.str);
    return *this;
}
```

---

### Comparison Operators

```cpp
// Equality
bool bigint::operator==(const bigint& other) const {
    return str == other.str;
}

// Inequality
bool bigint::operator!=(const bigint& other) const {
    return !(*this == other);
}

// Less than
bool bigint::operator<(const bigint& other) const {
    if (str.length() != other.str.length())
        return str.length() < other.str.length();
    return str < other.str;  // Lexicographic
}

// Greater than
bool bigint::operator>(const bigint& other) const {
    return !(*this < other) && (*this != other);
}

// Less than or equal
bool bigint::operator<=(const bigint& other) const {
    return (*this < other) || (*this == other);
}

// Greater than or equal
bool bigint::operator>=(const bigint& other) const {
    return (*this > other) || (*this == other);
}
```

---

### Stream Output

```cpp
// Must be non-member
std::ostream& operator<<(std::ostream& output, const bigint& obj) {
    output << obj.getStr();
    return output;
}
```

---

## Quick Tips

### Storage
- **Use `std::string`** for internal representation
- **No leading zeros** (except "0" itself)
- Store as: `"12345"` for the number 12345

### Addition
- Reverse strings → Add from right → Handle carry → Reverse back
- Pad shorter string with zeros

### Digit Shift
- **Left shift (<<)**: Append zeros → `42 << 3 = 42000`
- **Right shift (>>)**: Remove digits → `1337 >> 2 = 13`
- **Over-shift**: Return "0"

### Comparison
1. Compare length first (shorter = smaller)
2. If same length, use lexicographic comparison

### Increment
- **Prefix (`++x`)**: Return reference to self
- **Postfix (`x++`)**: Return copy of old value

### Return Types
| Operator | Return |
|----------|--------|
| `+`, `<<`, `>>` | `bigint` (new value) |
| `+=`, `<<=`, `>>=`, `++x` | `bigint&` (reference) |
| `x++` | `bigint` (old copy) |
| `==`, `<`, etc. | `bool` |

---

## Common Patterns

```cpp
// Delegation pattern
bigint& operator+=(const bigint& other) {
    *this = *this + other;  // Use operator+
    return *this;
}

// Reuse pattern
bool operator!=(const bigint& other) const {
    return !(*this == other);  // Use operator==
}

// Conversion pattern
bigint operator<<(const bigint& other) const {
    return *this << stringToUINT(other.str);  // Convert then use
}
```

---

## Essential String Operations

```cpp
// Append character
str.push_back('0');

// Insert n copies of character at end
str.insert(str.end(), n, '0');

// Erase n characters from position pos
str.erase(pos, n);

// Get length
str.length()  // or str.size()

// Clear string
str.clear();

// Compare strings (lexicographic)
str1 < str2
str1 == str2
```

---

## Testing Examples

```cpp
// Construction
bigint a;              // 0
bigint b(42);          // 42
bigint c(b);           // 42 (copy)

// Addition
a + b                  // 0 + 42 = 42
bigint(999) + bigint(1)  // 1000 (carry)

// Increment
++a                    // a becomes 1, returns 1
a++                    // returns 1, a becomes 2

// Shift
bigint(42) << 3        // 42000
bigint(1337) >> 2      // 13
bigint(5) >> 10        // 0

// Comparison
bigint(100) < bigint(200)   // true
bigint(42) == bigint(42)    // true
bigint(999) < bigint(1000)  // true (length: 3 < 4)

// Complex
(bigint(23) << 10) + 42     // 2300000042
```

---

## Edge Cases to Handle

```cpp
// Zero
bigint();                   // Must be "0"
bigint(0);                  // "0"

// Over-shift
bigint(42) >> 100;          // "0"

// Self-assignment
a = a;                      // Check in operator=

// Leading zeros
// Never store "042", always "42"

// Carry propagation
bigint(999) + bigint(1);    // "1000"
```

---

## Compilation & Execution

```bash
# Compile
g++ -Wall -Wextra -Werror bigint.cpp main.cpp -o bigint

# Run
./bigint
```

---

## Include Headers

```cpp
#include <iostream>    // std::ostream, std::cout
#include <string>      // std::string
#include <sstream>     // std::stringstream
```

---

## Operator Overload Syntax

```cpp
// Member function (binary)
ReturnType operator@(const Type& param) const;

// Member function (unary)
ReturnType operator@() const;

// Non-member (stream)
std::ostream& operator<<(std::ostream& os, const Type& obj);
```

---

## Quick Reference Table

| Operation | Operator | Example | Result |
|-----------|----------|---------|--------|
| Add | `+` | `bigint(10) + bigint(5)` | `15` |
| Add-assign | `+=` | `a += bigint(5)` | `a = a + 5` |
| Pre-increment | `++x` | `++a` | `a+1`, returns new |
| Post-increment | `x++` | `a++` | returns old, a+1 |
| Left shift | `<<` | `bigint(42) << 3` | `42000` |
| Right shift | `>>` | `bigint(1337) >> 2` | `13` |
| Equal | `==` | `a == b` | `true/false` |
| Not equal | `!=` | `a != b` | `true/false` |
| Less than | `<` | `a < b` | `true/false` |
| Print | `<<` | `cout << a` | Prints value |

---

## Memory & Performance

- **No dynamic allocation needed** (string handles it)
- **Time complexity:**
  - Addition: O(n)
  - Shift: O(n)
  - Comparison: O(n)
- **Space:** O(n) where n = number of digits

---

## Common Mistakes to Avoid

❌ **Don't:**
- Store leading zeros
- Forget self-assignment check in `operator=`
- Return wrong type (value vs reference)
- Mix up prefix/postfix increment logic
- Forget to handle over-shift (return "0")

✅ **Do:**
- Use const for non-modifying functions
- Return reference for compound operators
- Check edge cases (zero, overflow)
- Test with provided main

---

## Algorithm Summary

### Addition
```
1. Reverse both strings
2. Pad to equal length
3. Add digit by digit with carry
4. Reverse result
```

### Comparison
```
1. Compare lengths
2. If equal, compare lexicographically
```

### Shift
```
Left: Append n zeros
Right: Remove n digits (or set to "0")
```

---

## Quick Implementation Checklist

- [ ] Private: `std::string str`
- [ ] Three constructors (default, int, copy)
- [ ] Assignment operator with self-check
- [ ] Getter: `getStr()`
- [ ] Addition: `+`, `+=`
- [ ] Increment: `++x`, `x++`
- [ ] Shift (uint): `<<`, `>>`, `<<=`, `>>=`
- [ ] Shift (bigint): `<<`, `>>`, `<<=`, `>>=`
- [ ] All 6 comparison operators
- [ ] Stream output (non-member)
- [ ] Helper: `reverse()`
- [ ] Helper: `addition()`
- [ ] Helper: `stringToUINT()`

---

**Remember:** Treat numbers as strings, manipulate digits directly!

🚀 **Good luck with your bigint implementation!**
