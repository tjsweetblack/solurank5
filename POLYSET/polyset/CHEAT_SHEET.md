# Polyset Exercise - Cheat Sheet

Quick reference for the Polyset exercise implementation.

---

## Class Hierarchy Quick View

```
                    bag (abstract)
                    │
        ┌───────────┼───────────┐
        │           │           │
   array_bag    tree_bag   searchable_bag (abstract)
   (virtual)    (virtual)   (virtual)
        │           │           │
        └─────┬─────┴─────┬─────┘
              │           │
     searchable_     searchable_
     array_bag       tree_bag
              │           │
              └─────┬─────┘
                    │
                   set
```

---

## Virtual Inheritance - The Diamond Solution

### ❌ Without Virtual Inheritance
```cpp
class array_bag : public bag { };
class searchable_bag : public bag { };
// Result: searchable_array_bag has TWO copies of bag!
```

### ✅ With Virtual Inheritance
```cpp
class array_bag : virtual public bag { };
class searchable_bag : virtual public bag { };
// Result: searchable_array_bag has ONE copy of bag!
```

**Rule**: Always use `virtual public` when multiple inheritance paths converge.

---

## searchable_array_bag - Complete Implementation

### Header (.hpp)
```cpp
#pragma once
#include "array_bag.hpp"
#include "searchable_bag.hpp"

class searchable_array_bag : public array_bag, public searchable_bag
{
public:
    searchable_array_bag();
    searchable_array_bag(const searchable_array_bag& source);
    searchable_array_bag& operator=(const searchable_array_bag& source);
    ~searchable_array_bag();
    bool has(int) const;
};
```

### Implementation (.cpp)
```cpp
#include "searchable_array_bag.hpp"

// Default constructor - delegate to base
searchable_array_bag::searchable_array_bag()
{
}

// Copy constructor - explicitly call base copy constructor
searchable_array_bag::searchable_array_bag(const searchable_array_bag& source) 
    : array_bag(source)
{
}

// Assignment operator - self-check + delegate
searchable_array_bag& searchable_array_bag::operator=(const searchable_array_bag& source)
{
    if(this != &source)
    {
        array_bag::operator=(source);
    }
    return(*this);
}

// Linear search - O(n)
bool searchable_array_bag::has(int value) const
{
    for(int i = 0; i < this->size; i++)
    {
        if(this->data[i] == value)
            return (true);
    }
    return(false);
}

// Destructor - delegate cleanup to base
searchable_array_bag::~searchable_array_bag()
{
}
```

**Key Points**:
- ✅ Access protected members `data` and `size` from `array_bag`
- ✅ Delegate all resource management to `array_bag`
- ✅ Only implement `has()` - the new functionality

---

## searchable_tree_bag - Complete Implementation

### Header (.hpp)
```cpp
#pragma once
#include "tree_bag.hpp"
#include "searchable_bag.hpp"

class searchable_tree_bag : public tree_bag, public searchable_bag
{
private:
    bool search(node* node, const int value) const;  // Recursive helper
    
public:
    searchable_tree_bag();
    searchable_tree_bag(const searchable_tree_bag& source);
    searchable_tree_bag& operator=(const searchable_tree_bag& source);
    ~searchable_tree_bag();
    bool has(int) const;
};
```

### Implementation (.cpp)
```cpp
#include "searchable_tree_bag.hpp"

// Default constructor
searchable_tree_bag::searchable_tree_bag()
{
}

// Copy constructor
searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& source) 
    : tree_bag(source)
{
}

// Assignment operator
searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag& source)
{
    if(this != &source)
    {
        tree_bag::operator=(source);
    }
    return(*this);
}

// Recursive BST search - O(log n) average
bool searchable_tree_bag::search(node* node, const int value) const
{
    if(node == nullptr)              // Base case: not found
        return(false);
    if(node->value == value)         // Base case: found!
        return(true);
    else if(value < node->value)     // Search left
        return(search(node->l, value));
    else                             // Search right
        return(search(node->r, value));
}

// Public interface
bool searchable_tree_bag::has(int value) const
{
    return(search(this->tree, value));  // Start from root
}

// Destructor
searchable_tree_bag::~searchable_tree_bag()
{
}
```

**Key Points**:
- ✅ Use recursive helper for BST traversal
- ✅ Access protected member `tree` from `tree_bag`
- ✅ Leverage BST property: left < parent < right

---

## set - Complete Implementation

### Header (.hpp)
```cpp
#include "searchable_bag.hpp"

class set
{
private:
    searchable_bag& bag;  // Reference to wrapped bag
    
public:
    // Delete copy/assignment
    set() = delete;
    set(const set& source) = delete;
    set& operator=(const set& source) = delete;
    
    // Constructor requires a bag
    set(searchable_bag& s_bag);
    
    // Public interface
    bool has(int) const;
    void insert(int);
    void insert(int*, int);
    void print() const;
    void clear();
    const searchable_bag& get_bag();
    
    ~set();
};
```

### Implementation (.cpp)
```cpp
#include "set.hpp"

// Constructor - MUST initialize reference in initializer list
set::set(searchable_bag& s_bag) : bag(s_bag)
{
}

// Delegate to wrapped bag
bool set::has(int value) const
{
    return(bag.has(value));
}

// THE KEY METHOD - prevent duplicates
void set::insert(int value)
{
    if(!(this->has(value)))  // Only insert if NOT present
        bag.insert(value);
}

// Insert array - call single insert for each
void set::insert(int *data, int size)
{
    for(int i = 0; i < size; i++)
    {
        this->insert(data[i]);  // Uses uniqueness check
    }
}

// Delegate methods
void set::print() const
{
    bag.print();
}

void set::clear()
{
    bag.clear();
}

const searchable_bag& set::get_bag()
{
    return(this->bag);
}

// Don't delete bag - we don't own it!
set::~set()
{
}
```

**Key Points**:
- ✅ Reference member MUST be initialized in initializer list
- ✅ Delete copy/assignment - sets wrap specific bags
- ✅ `insert()` checks for duplicates before adding
- ✅ Empty destructor - we don't own the bag

---

## Orthodox Canonical Form Template

```cpp
class MyClass
{
private:
    // Data members
    
public:
    // 1. Default Constructor
    MyClass();
    
    // 2. Copy Constructor
    MyClass(const MyClass& source);
    
    // 3. Assignment Operator
    MyClass& operator=(const MyClass& source);
    
    // 4. Destructor
    ~MyClass();
};
```

### Implementation Pattern
```cpp
// 1. Default Constructor
MyClass::MyClass()
{
    // Initialize members
}

// 2. Copy Constructor - deep copy
MyClass::MyClass(const MyClass& source)
{
    // Copy all members
    // Allocate new memory if needed
}

// 3. Assignment Operator
MyClass& MyClass::operator=(const MyClass& source)
{
    if(this != &source)  // ← ALWAYS check self-assignment
    {
        // Delete old resources
        // Copy new values
        // Allocate new memory if needed
    }
    return *this;
}

// 4. Destructor
MyClass::~MyClass()
{
    // Free all allocated resources
}
```

---

## Common Patterns Quick Reference

### Pattern 1: Delegating Constructor
```cpp
DerivedClass::DerivedClass() 
{
    // Base class constructor called automatically
}
```

### Pattern 2: Delegating Copy Constructor
```cpp
DerivedClass::DerivedClass(const DerivedClass& src) 
    : BaseClass(src)  // ← Explicitly call base copy
{
}
```

### Pattern 3: Delegating Assignment
```cpp
DerivedClass& DerivedClass::operator=(const DerivedClass& src)
{
    if(this != &src)
    {
        BaseClass::operator=(src);  // ← Delegate to base
    }
    return *this;
}
```

### Pattern 4: Initializing References
```cpp
MyClass::MyClass(SomeType& ref) : myRef(ref)  // ← Initializer list!
{
    // Can't do: myRef = ref; (won't compile)
}
```

### Pattern 5: Deleted Functions
```cpp
class MyClass
{
public:
    MyClass() = delete;                    // No default construction
    MyClass(const MyClass&) = delete;      // No copying
    MyClass& operator=(const MyClass&) = delete; // No assignment
};
```

---

## Time Complexity Cheat Sheet

| Operation | array_bag | tree_bag (balanced) | tree_bag (worst) |
|-----------|-----------|---------------------|------------------|
| insert    | O(n)      | O(log n)            | O(n)             |
| has       | O(n)      | O(log n)            | O(n)             |
| print     | O(n)      | O(n)                | O(n)             |
| clear     | O(n)      | O(n)                | O(n)             |

**set operations**: Same as underlying bag + O(n) check for duplicates during insert

---

## Memory Management Rules

### ✅ DO:
- Check `this != &source` in assignment operator
- Delete old memory before allocating new
- Initialize references in initializer list
- Use `const` for methods that don't modify
- Mark inherited base classes as `virtual` when diamond inheritance

### ❌ DON'T:
- Forget to implement destructor if you allocate memory
- Shallow copy when you need deep copy
- Forget self-assignment check
- Try to assign to references (initialize them instead)
- Delete memory you don't own

---

## Quick Debug Checklist

### Compilation Errors?
- [ ] Forgot `virtual public` inheritance?
- [ ] Missing `#include` directives?
- [ ] Trying to instantiate abstract class?
- [ ] Forgot to implement pure virtual function?
- [ ] Reference not initialized in initializer list?

### Crashes/Segfaults?
- [ ] Double delete (check assignment operator)?
- [ ] Accessing deleted memory?
- [ ] Nullptr dereference in tree traversal?
- [ ] Array index out of bounds?
- [ ] Self-assignment not handled?

### Wrong Results?
- [ ] Shallow copy instead of deep copy?
- [ ] Set not checking for duplicates?
- [ ] BST search logic incorrect?
- [ ] Not accessing correct protected members?

---

## Testing Template

```cpp
int main()
{
    // Test 1: Create objects
    searchable_array_bag sab;
    searchable_tree_bag stb;
    
    // Test 2: Insert elements
    sab.insert(5);
    stb.insert(5);
    
    // Test 3: Test has()
    assert(sab.has(5) == true);
    assert(stb.has(5) == true);
    assert(sab.has(10) == false);
    assert(stb.has(10) == false);
    
    // Test 4: Test set uniqueness
    set s1(sab);
    s1.insert(5);
    s1.insert(5);  // Should not add duplicate
    s1.insert(5);  // Should not add duplicate
    
    // Test 5: Copy constructor
    searchable_array_bag copy(sab);
    assert(copy.has(5) == true);
    
    // Test 6: Assignment operator
    searchable_array_bag other;
    other = sab;
    assert(other.has(5) == true);
    
    return 0;
}
```

---

## Polymorphism Example

```cpp
// Works with both implementations!
void test_bag(searchable_bag& bag)
{
    bag.insert(10);
    bag.insert(20);
    if(bag.has(10))
        std::cout << "Found 10!" << std::endl;
    bag.print();
}

int main()
{
    searchable_array_bag array_impl;
    searchable_tree_bag tree_impl;
    
    test_bag(array_impl);  // Uses array
    test_bag(tree_impl);   // Uses tree
    
    // Both work with set!
    set s1(array_impl);
    set s2(tree_impl);
}
```

---

## Key Takeaways

### 🎯 Core Concepts
1. **Virtual Inheritance** solves the diamond problem
2. **Multiple Inheritance** combines functionality from multiple bases
3. **Abstract Classes** define interfaces (pure virtual functions)
4. **Wrapper Pattern** adds behavior without inheritance
5. **Orthodox Canonical Form** ensures proper resource management

### 🔑 Critical Details
- Use `virtual public` for diamond inheritance
- Initialize references in initializer list
- Always check self-assignment in operator=
- Use `const` for read-only methods
- Protected members accessible to derived classes

### 💡 Design Patterns
- **Interface Pattern**: Abstract base classes (bag, searchable_bag)
- **Template Method**: Virtual functions + inheritance
- **Wrapper/Decorator**: Set wraps searchable_bag
- **Strategy Pattern**: Different bag implementations

---

## One-Line Solutions

```cpp
// searchable_array_bag::has()
for(int i=0; i<size; i++) if(data[i]==value) return true; return false;

// searchable_tree_bag::search()
return node ? (node->value==value ? true : search(value<node->value?node->l:node->r, value)) : false;

// set::insert()
if(!has(value)) bag.insert(value);
```

---

## Exam/Interview Quick Answers

**Q: Why virtual inheritance?**
A: Prevents diamond problem - ensures single copy of base class.

**Q: Difference between bag and set?**
A: Bag allows duplicates, set doesn't.

**Q: Why reference in set?**
A: Non-nullable, non-reassignable, safer than pointer.

**Q: Why = delete?**
A: Explicitly prevent copying/default construction.

**Q: Time complexity of array_bag::has()?**
A: O(n) - linear search.

**Q: Time complexity of tree_bag::has()?**
A: O(log n) average, O(n) worst case.

**Q: What is Orthodox Canonical Form?**
A: Constructor, copy constructor, assignment operator, destructor.

---

**END OF CHEAT SHEET** 📝
