# BigInt Reference Guide

## Quick Reference

Complete implementation reference for the **bigint** class - arbitrary precision unsigned integer.

---

## Class Structure

```cpp
class bigint {
private:
    std::string str;  // Internal representation of the number
    
public:
    // Constructors
    bigint();
    bigint(unsigned int num);
    bigint(const bigint& source);
    
    // Accessor
    std::string getStr() const;
    
    // Assignment
    bigint& operator=(const bigint& source);
    
    // Addition
    bigint operator+(const bigint& other) const;
    bigint& operator+=(const bigint& other);
    
    // Increment
    bigint& operator++();      // Prefix
    bigint operator++(int);    // Postfix
    
    // Digit shift (with unsigned int)
    bigint operator<<(unsigned int n) const;
    bigint operator>>(unsigned int n) const;
    bigint& operator<<=(unsigned int n);
    bigint& operator>>=(unsigned int n);
    
    // Digit shift (with bigint)
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

// Non-member function
std::ostream& operator<<(std::ostream& output, const bigint& obj);
```

---

## Implementation Details

### Headers Required
```cpp
#include <iostream>   // For std::ostream
#include <string>     // For std::string
#include <sstream>    // For std::stringstream (conversions)
```

---

## Constructors & Assignment

### Default Constructor
```cpp
bigint::bigint() {
    str = "0";
}
```
**Purpose:** Initialize to zero  
**Usage:** `bigint a;` → a = 0

---

### Integer Constructor
```cpp
bigint::bigint(unsigned int num) {
    std::stringstream ss;
    ss << num;
    str = ss.str();
}
```
**Purpose:** Convert unsigned int to bigint  
**Usage:** `bigint a(42);` → a = 42  
**Method:** Uses stringstream for int-to-string conversion

---

### Copy Constructor
```cpp
bigint::bigint(const bigint& source) {
    *this = source;
}
```
**Purpose:** Create copy of existing bigint  
**Usage:** `bigint b(a);` or `bigint b = a;`  
**Note:** Delegates to assignment operator

---

### Assignment Operator
```cpp
bigint& bigint::operator=(const bigint& source) {
    if (this == &source)
        return *this;
    str = source.str;
    return *this;
}
```
**Purpose:** Assign one bigint to another  
**Returns:** Reference to self (enables chaining)  
**Self-assignment check:** Prevents copying to itself

---

## Accessor

### getStr()
```cpp
std::string bigint::getStr() const {
    return str;
}
```
**Purpose:** Access internal string representation  
**Returns:** Copy of the string  
**Const:** Doesn't modify object

---

## Helper Functions (Non-member)

### String Reversal
```cpp
std::string reverse(const std::string& str) {
    std::string revStr;
    for (size_t i = str.length(); i > 0; i--) {
        revStr.push_back(str[i - 1]);
    }
    return revStr;
}
```
**Purpose:** Reverse a string  
**Why needed:** Addition works from right to left

---

### Addition Helper
```cpp
std::string addition(const bigint& obj1, const bigint& obj2) {
    std::string str1 = reverse(obj1.getStr());
    std::string str2 = reverse(obj2.getStr());
    std::string result;
    
    // Equalize lengths
    size_t len1 = str1.length();
    size_t len2 = str2.length();
    
    if (len1 > len2) {
        int diff = len1 - len2;
        while (diff > 0) {
            str2.push_back('0');
            diff--;
        }
    } else if (len2 > len1) {
        int diff = len2 - len1;
        while (diff > 0) {
            str1.push_back('0');
            diff--;
        }
    }
    
    // Add digit by digit with carry
    int carry = 0;
    for (size_t i = 0; i < str1.length(); i++) {
        int digit1 = str1[i] - '0';
        int digit2 = str2[i] - '0';
        int sum = digit1 + digit2 + carry;
        
        if (sum > 9) {
            carry = sum / 10;
            result.push_back((sum % 10) + '0');
        } else {
            result.push_back(sum + '0');
            carry = 0;
        }
    }
    
    if (carry != 0)
        result.push_back(carry + '0');
    
    return reverse(result);
}
```
**Algorithm:**
1. Reverse both strings (process from least significant digit)
2. Pad shorter string with zeros
3. Add digit by digit with carry handling
4. Reverse result back to normal order

---

### String to Unsigned Int
```cpp
unsigned int stringToUINT(std::string str) {
    std::stringstream ss(str);
    unsigned int res;
    ss >> res;
    return res;
}
```
**Purpose:** Convert string to unsigned int  
**Used for:** bigint to int conversion for shift operations

---

## Addition Operators

### Binary Addition (+)
```cpp
bigint bigint::operator+(const bigint& other) const {
    bigint temp(other);
    temp.str.clear();
    std::string result = addition(*this, other);
    temp.str = result;
    return temp;
}
```
**Returns:** New bigint with sum  
**Const:** Doesn't modify either operand  
**Usage:** `c = a + b;`

---

### Compound Addition (+=)
```cpp
bigint& bigint::operator+=(const bigint& other) {
    *this = *this + other;
    return *this;
}
```
**Returns:** Reference to self  
**Modifies:** The left operand  
**Usage:** `a += b;`

---

## Increment Operators

### Prefix Increment (++x)
```cpp
bigint& bigint::operator++() {
    *this = *this + bigint(1);
    return *this;
}
```
**Returns:** Reference to incremented value  
**Behavior:** Increment then return  
**Usage:** `++a;` or `b = ++a;`

---

### Postfix Increment (x++)
```cpp
bigint bigint::operator++(int) {
    bigint temp = *this;
    *this = *this + bigint(1);
    return temp;
}
```
**Parameter:** `int` is dummy parameter (distinguishes from prefix)  
**Returns:** Copy of original value  
**Behavior:** Save old value, increment, return old  
**Usage:** `a++;` or `b = a++;`

---

## Digit Shift Operators (unsigned int)

### Left Shift (<<)
```cpp
bigint bigint::operator<<(unsigned int n) const {
    bigint temp = *this;
    temp.str.insert(temp.str.end(), n, '0');
    return temp;
}
```
**Effect:** Multiply by 10^n  
**Method:** Append n zeros to the end  
**Example:** `42 << 3` → `42000`

---

### Right Shift (>>)
```cpp
bigint bigint::operator>>(unsigned int n) const {
    bigint temp = *this;
    size_t len = temp.str.length();
    
    if (n >= len)
        temp.str = "0";
    else
        temp.str.erase(temp.str.length() - n, n);
    
    return temp;
}
```
**Effect:** Divide by 10^n (integer division)  
**Method:** Remove n digits from the end  
**Example:** `1337 >> 2` → `13`  
**Edge case:** If shifting more than digits available, result is 0

---

### Left Shift Assignment (<<=)
```cpp
bigint& bigint::operator<<=(unsigned int n) {
    *this = *this << n;
    return *this;
}
```
**Returns:** Reference to self  
**Modifies:** The operand  
**Usage:** `a <<= 3;`

---

### Right Shift Assignment (>>=)
```cpp
bigint& bigint::operator>>=(unsigned int n) {
    *this = *this >> n;
    return *this;
}
```
**Returns:** Reference to self  
**Modifies:** The operand  
**Usage:** `a >>= 2;`

---

## Digit Shift Operators (bigint)

### Left Shift (<<)
```cpp
bigint bigint::operator<<(const bigint& other) const {
    bigint temp;
    temp = *this << stringToUINT(other.str);
    return temp;
}
```
**Strategy:** Convert bigint to unsigned int, then use int version

---

### Right Shift (>>)
```cpp
bigint bigint::operator>>(const bigint& other) const {
    bigint temp;
    temp = *this >> stringToUINT(other.str);
    return temp;
}
```
**Strategy:** Convert bigint to unsigned int, then use int version

---

### Left Shift Assignment (<<=)
```cpp
bigint& bigint::operator<<=(const bigint& other) {
    *this = *this << stringToUINT(other.str);
    return *this;
}
```
**Returns:** Reference to self

---

### Right Shift Assignment (>>=)
```cpp
bigint& bigint::operator>>=(const bigint& other) {
    *this = *this >> stringToUINT(other.str);
    return *this;
}
```
**Returns:** Reference to self

---

## Comparison Operators

### Equality (==)
```cpp
bool bigint::operator==(const bigint& other) const {
    if (str == other.str)
        return true;
    return false;
}
```
**Logic:** Direct string comparison  
**Returns:** `true` if identical, `false` otherwise

---

### Inequality (!=)
```cpp
bool bigint::operator!=(const bigint& other) const {
    return !(*this == other);
}
```
**Logic:** Negation of equality  
**Reuses:** `operator==`

---

### Less Than (<)
```cpp
bool bigint::operator<(const bigint& other) const {
    std::string str1 = str;
    std::string str2 = other.getStr();
    size_t len1 = str1.length();
    size_t len2 = str2.length();
    
    if (len1 != len2)
        return len1 < len2;
    return str1 < str2;
}
```
**Logic:**
1. If different lengths, shorter is smaller
2. If same length, lexicographic comparison
**Example:** `"99" < "100"` (2 digits < 3 digits)

---

### Greater Than (>)
```cpp
bool bigint::operator>(const bigint& other) const {
    return !(*this < other) && (*this != other);
}
```
**Logic:** Not less than AND not equal  
**Alternative:** `return !(*this <= other);`

---

### Less Than or Equal (<=)
```cpp
bool bigint::operator<=(const bigint& other) const {
    return (*this < other) || (*this == other);
}
```
**Logic:** Less than OR equal

---

### Greater Than or Equal (>=)
```cpp
bool bigint::operator>=(const bigint& other) const {
    return (*this > other) || (*this == other);
}
```
**Logic:** Greater than OR equal

---

## Stream Output

### Stream Insertion Operator (<<)
```cpp
std::ostream& operator<<(std::ostream& output, const bigint& obj) {
    output << obj.getStr();
    return output;
}
```
**Must be:** Non-member function  
**Returns:** Reference to ostream (enables chaining)  
**Usage:** `std::cout << bigint(42);`  
**Output:** Base 10, no leading zeros

---

## Key Algorithms

### Addition Algorithm
**Elementary School Addition:**
```
    1234
  +  567
  ------
   1801
```

**Steps:**
1. Align numbers by reversing
2. Add from right to left
3. Track carry
4. Handle final carry if exists

### Comparison Algorithm
**Length First:**
- Different lengths → shorter is smaller
- Same length → lexicographic (dictionary order)

**Why it works:**
- "999" (3 chars) < "1000" (4 chars)
- "100" < "200" (lexicographic at same length)

---

## Edge Cases

### Zero Handling
- Default constructor returns "0"
- Right shift beyond length returns "0"
- No leading zeros stored

### Overflow Protection
- String-based: no overflow possible
- Limited only by available memory

### Self-Assignment
```cpp
bigint a(5);
a = a;  // Handled by self-assignment check
```

---

## Operator Return Types Reference

| Operator | Return Type | Reason |
|----------|-------------|--------|
| `+` | `bigint` | New value, doesn't modify operands |
| `+=` | `bigint&` | Returns reference for chaining |
| `++x` | `bigint&` | Returns reference to modified value |
| `x++` | `bigint` | Returns copy of old value |
| `<<` | `bigint` | New value |
| `<<=` | `bigint&` | Reference for chaining |
| `==` | `bool` | True/false result |
| `=` | `bigint&` | Reference for chaining |

---

## Const Correctness

### Const Member Functions
```cpp
std::string getStr() const;           // Doesn't modify
bigint operator+(const bigint&) const; // Doesn't modify
bool operator<(const bigint&) const;   // Doesn't modify
```

### Non-Const Member Functions
```cpp
bigint& operator+=(const bigint&);     // Modifies
bigint& operator++();                  // Modifies
bigint& operator<<=(unsigned int);     // Modifies
```

---

## Usage Examples

### Basic Operations
```cpp
bigint a(42);
bigint b(21);
bigint c = a + b;          // 63
c += a;                     // 105
```

### Increment
```cpp
bigint d(10);
std::cout << ++d;          // 11
std::cout << d++;          // 11
std::cout << d;            // 12
```

### Shift Operations
```cpp
bigint e(42);
std::cout << (e << 3);     // 42000
std::cout << (e >> 1);     // 4
```

### Comparisons
```cpp
bigint f(100);
bigint g(200);
if (f < g)                 // true
    std::cout << "f is smaller";
```

### Complex Expressions
```cpp
bigint result = ((bigint(23) << 10) + 42);
// 23 << 10 = 2300000000
// 2300000000 + 42 = 2300000042
```

---

## Compilation

```bash
g++ -Wall -Wextra -Werror bigint.cpp main.cpp -o bigint
./bigint
```

---

## Testing Checklist

- [x] Default constructor creates 0
- [x] Integer constructor works
- [x] Copy constructor creates duplicate
- [x] Addition produces correct results
- [x] Addition with carry works
- [x] Prefix increment works
- [x] Postfix increment works
- [x] Left shift appends zeros
- [x] Right shift removes digits
- [x] Right shift handles over-shift (returns 0)
- [x] All comparison operators work
- [x] Stream output has no leading zeros
- [x] Compound operators return references
- [x] bigint-based shift operators work

---

## Common Patterns

### Pattern: Using Helper Functions
```cpp
// Separate logic from operator
std::string addition(const bigint&, const bigint&);  // Helper

bigint bigint::operator+(const bigint& other) const {
    // Use helper
    bigint temp;
    temp.str = addition(*this, other);
    return temp;
}
```

### Pattern: Delegation
```cpp
// Compound operator uses binary operator
bigint& operator+=(const bigint& other) {
    *this = *this + other;  // Delegate to operator+
    return *this;
}
```

### Pattern: Reuse in Comparisons
```cpp
bool operator!=(const bigint& other) const {
    return !(*this == other);  // Reuse operator==
}
```

---

## Performance Notes

- **Time Complexity:**
  - Addition: O(max(n, m))
  - Comparison: O(min(n, m)) average, O(n) worst
  - Shift: O(n)
  
- **Space Complexity:**
  - Storage: O(n) for n digits
  - Operations: O(n) temporary space

---

## Summary

The bigint class provides arbitrary precision arithmetic through:
- String-based storage
- Elementary school addition algorithm
- Digit manipulation for shifting
- Length-based comparison
- Complete operator overloading

**Core principle:** Treat the number as a string of digits, manipulate accordingly.

---

*This reference provides complete implementation details for the bigint class assignment.*
