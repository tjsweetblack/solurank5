# BigInt Study Guide

## Overview
This study guide covers the **bigint** class implementation - a class representing arbitrary precision unsigned integers in C++.

---

## Table of Contents
1. [Core Concepts](#core-concepts)
2. [Data Representation](#data-representation)
3. [Required Operations](#required-operations)
4. [Implementation Strategies](#implementation-strategies)
5. [Common Pitfalls](#common-pitfalls)
6. [Testing Approach](#testing-approach)

---

## Core Concepts

### What is a BigInt?
- **BigInt (Bignum)**: An object that stores arbitrary precision numbers
- Allows storage of numbers larger than `SIZE_MAX` without precision loss
- Typically implemented using strings or arrays to store individual digits

### Why BigInt?
```cpp
// Standard unsigned int has limits (usually 4,294,967,295 for 32-bit)
unsigned int standard = 4294967295;  // MAX value
standard + 1;  // Overflow!

// BigInt can handle any size
bigint large("99999999999999999999999999");
large + bigint(1);  // No problem!
```

---

## Data Representation

### String-Based Storage
The most common approach stores the number as a string:
```cpp
class bigint {
private:
    std::string str;  // "1234" represents 1234
public:
    // ...
};
```

**Advantages:**
- Easy to understand and debug
- Simple to print
- No size limitations
- Direct conversion to/from strings

**Key Considerations:**
- Store digits as characters ('0'-'9')
- Handle leading zeros properly
- Efficient string manipulation required

---

## Required Operations

### 1. Constructors
```cpp
bigint();                    // Default: should initialize to 0
bigint(unsigned int num);    // From integer
bigint(const bigint& source); // Copy constructor
```

**Implementation Tips:**
- Default constructor: `str = "0"`
- From integer: use `stringstream` for conversion
- Copy constructor: use assignment operator

### 2. Addition (+, +=)
```cpp
bigint operator+(const bigint& other) const;
bigint& operator+=(const bigint& other);
```

**Algorithm (Elementary School Addition):**
1. Reverse both strings (to process from least significant digit)
2. Pad the shorter string with zeros
3. Add digit by digit from right to left
4. Handle carry at each position
5. Reverse result back

**Example:**
```
  1234
+  567
------
Step 1: Reverse: "4321" + "765"
Step 2: Align:   "4321" + "7650"
Step 3: Add with carry:
  4+7=11 → digit=1, carry=1
  3+6+1=10 → digit=0, carry=1
  2+5+1=8 → digit=8, carry=0
  1+0=1 → digit=1, carry=0
Result: "1801" → Reverse: "1801"
```

### 3. Increment (++x, x++)
```cpp
bigint& operator++();    // Prefix: ++x
bigint operator++(int);  // Postfix: x++
```

**Key Differences:**
- **Prefix (`++x`)**: Increment then return reference to self
- **Postfix (`x++`)**: Save copy, increment, return old copy

### 4. Digit Shift (<<, >>, <<=, >>=)
```cpp
bigint operator<<(unsigned int n) const;  // Shift left
bigint operator>>(unsigned int n) const;  // Shift right
```

**Concept:**
- **Left shift** (<<): Multiply by 10^n (append n zeros)
  - `42 << 3 = 42000`
- **Right shift** (>>): Divide by 10^n (remove n digits)
  - `1337 >> 2 = 13`

**Implementation:**
```cpp
// Left shift: append zeros
str.insert(str.end(), n, '0');

// Right shift: remove from end
if (n >= str.length())
    str = "0";
else
    str.erase(str.length() - n, n);
```

### 5. Shift with BigInt Operand
```cpp
bigint operator<<(const bigint& other) const;
bigint& operator>>=(const bigint& other);
```

**Strategy:**
- Convert bigint to unsigned int
- Call the unsigned int version
- Use `stringstream` for conversion

### 6. Comparison Operators
```cpp
bool operator==(const bigint& other) const;
bool operator!=(const bigint& other) const;
bool operator<(const bigint& other) const;
bool operator>(const bigint& other) const;
bool operator<=(const bigint& other) const;
bool operator>=(const bigint& other) const;
```

**Implementation Logic:**

**Equality (==):**
```cpp
return (str == other.str);
```

**Less Than (<):**
1. Compare lengths first (shorter = smaller)
2. If same length, use lexicographic comparison
```cpp
if (str.length() != other.str.length())
    return str.length() < other.str.length();
return str < other.str;  // Lexicographic
```

**Others:**
- `!=` : `!(a == b)`
- `>` : `!(a < b) && (a != b)` or `!(a <= b)`
- `<=` : `(a < b) || (a == b)`
- `>=` : `(a > b) || (a == b)`

### 7. Stream Output
```cpp
std::ostream& operator<<(std::ostream& output, const bigint& obj);
```

**Requirements:**
- Print in base 10
- No leading zeros (except for "0" itself)
- Must be a non-member function

---

## Implementation Strategies

### Helper Functions

#### 1. String Reversal
```cpp
std::string reverse(const std::string& str) {
    std::string result;
    for (size_t i = str.length(); i > 0; i--) {
        result.push_back(str[i - 1]);
    }
    return result;
}
```

#### 2. String to Unsigned Int
```cpp
unsigned int stringToUINT(const std::string& str) {
    std::stringstream ss(str);
    unsigned int result;
    ss >> result;
    return result;
}
```

#### 3. Addition Algorithm
```cpp
std::string addition(const bigint& a, const bigint& b) {
    std::string str1 = reverse(a.getStr());
    std::string str2 = reverse(b.getStr());
    
    // Pad to same length
    while (str1.length() < str2.length())
        str1.push_back('0');
    while (str2.length() < str1.length())
        str2.push_back('0');
    
    std::string result;
    int carry = 0;
    
    for (size_t i = 0; i < str1.length(); i++) {
        int digit1 = str1[i] - '0';
        int digit2 = str2[i] - '0';
        int sum = digit1 + digit2 + carry;
        
        result.push_back((sum % 10) + '0');
        carry = sum / 10;
    }
    
    if (carry != 0)
        result.push_back(carry + '0');
    
    return reverse(result);
}
```

---

## Common Pitfalls

### 1. Leading Zeros
❌ **Wrong:**
```cpp
bigint a(5);
a << 3;  // Should be "5000", not "05000"
```

✅ **Right:**
- Never store leading zeros in the internal string
- Only exception: "0" itself

### 2. Right Shift Edge Cases
❌ **Wrong:**
```cpp
bigint(42) >> 5;  // Should return 0, not crash or empty string
```

✅ **Right:**
```cpp
if (n >= str.length())
    str = "0";
```

### 3. Prefix vs Postfix Increment
❌ **Wrong:**
```cpp
bigint operator++(int) {  // Postfix
    *this = *this + bigint(1);
    return *this;  // Returns modified value!
}
```

✅ **Right:**
```cpp
bigint operator++(int) {
    bigint temp = *this;  // Save old value
    *this = *this + bigint(1);
    return temp;  // Return old value
}
```

### 4. Comparison Operator Logic
❌ **Wrong:**
```cpp
bool operator>(const bigint& other) const {
    return !(*this < other);  // Wrong! What about equality?
}
```

✅ **Right:**
```cpp
bool operator>(const bigint& other) const {
    return !(*this < other) && (*this != other);
    // Or: return !(*this <= other);
}
```

### 5. Self-Assignment
✅ **Always check:**
```cpp
bigint& operator=(const bigint& source) {
    if (this == &source)  // Important check!
        return *this;
    str = source.str;
    return *this;
}
```

---

## Testing Approach

### Test Cases to Consider

#### 1. Constructors
```cpp
bigint a;           // Default (should be 0)
bigint b(42);       // From int
bigint c(b);        // Copy
```

#### 2. Addition
```cpp
bigint(5) + bigint(3);          // Simple: 8
bigint(999) + bigint(1);        // Carry: 1000
bigint(123456) + bigint(789);   // Different lengths
```

#### 3. Shifts
```cpp
bigint(42) << 3;          // 42000
bigint(1337) >> 2;        // 13
bigint(42) >> 10;         // 0 (over-shift)
bigint(0) << 5;           // 0
```

#### 4. Comparisons
```cpp
bigint(42) < bigint(100);     // true
bigint(100) < bigint(42);     // false
bigint(42) == bigint(42);     // true
bigint(999) < bigint(1000);   // true (length differs)
```

#### 5. Increment
```cpp
bigint a(5);
cout << ++a;  // 6
cout << a;    // 6

bigint b(5);
cout << b++;  // 5
cout << b;    // 6
```

#### 6. Complex Expressions
```cpp
(bigint(10) << 2) + bigint(5);  // 1005
((bigint(23) << 10) + 42);      // 23000000042
```

---

## Memory Management

### No Dynamic Allocation Needed
Since `std::string` handles its own memory:
- No need for custom destructor
- No need for deep copy concerns
- String handles resizing automatically

---

## Performance Considerations

### Time Complexity
- **Addition**: O(max(n, m)) where n, m are digit counts
- **Comparison**: O(n) worst case
- **Shift**: O(n) for string manipulation
- **Print**: O(n)

### Space Complexity
- **Storage**: O(n) where n is number of digits
- **Addition temp**: O(max(n, m))

---

## Key Takeaways

1. **String representation** is simple and effective for bigint
2. **Addition** mimics elementary school algorithm with carry
3. **Digit shift** is trivial (add/remove zeros)
4. **Comparison** relies on length first, then lexicographic order
5. **Operator overloading** requires understanding return types and references
6. **No leading zeros** in stored representation
7. **Helper functions** keep code clean and reusable

---

## Study Checklist

- [ ] Understand why bigint is needed
- [ ] Know string-based storage approach
- [ ] Implement addition with carry algorithm
- [ ] Understand prefix vs postfix increment
- [ ] Master all comparison operators
- [ ] Handle edge cases (zero, over-shift)
- [ ] Implement all required operators
- [ ] Test with provided main function
- [ ] Verify no leading zeros in output

---

## Additional Practice

1. Try implementing subtraction (not required but good practice)
2. Consider how multiplication might work
3. Think about how to handle negative numbers
4. Research other bigint implementations (GMP library, Java's BigInteger)

Good luck with your bigint implementation! 🚀
