# Polyset Exercise - Complete Reference Guide

## Overview
This exercise demonstrates advanced C++ concepts including:
- Multiple inheritance
- Virtual inheritance (diamond problem solution)
- Abstract classes and interfaces
- Polymorphism
- Orthodox Canonical Form
- Wrapper pattern (Decorator pattern)

---

## Problem Statement

### Given Classes (Provided)
1. **bag** - Abstract base class for a bag data structure
2. **searchable_bag** - Abstract class adding search capability
3. **array_bag** - Concrete implementation using dynamic array
4. **tree_bag** - Concrete implementation using binary search tree

### Task Requirements
1. **Part 1**: Create `searchable_array_bag` and `searchable_tree_bag` that:
   - Inherit from both their respective bag implementation AND `searchable_bag`
   - Implement the `has()` method for searching

2. **Part 2**: Create `set` class that:
   - Wraps a `searchable_bag` reference
   - Ensures uniqueness (no duplicates)
   - Delegates operations to the wrapped bag

---

## Architecture Diagram

```
                    ┌─────────┐
                    │   bag   │ (abstract)
                    └────┬────┘
                         │
         ┌───────────────┼───────────────┐
         │               │               │
    ┌────▼────┐    ┌────▼────┐    ┌─────▼──────────┐
    │array_bag│    │tree_bag │    │searchable_bag  │ (abstract)
    └────┬────┘    └────┬────┘    └─────┬──────────┘
         │              │               │
         │              │               │
         └──────┬───────┴───────┬───────┘
                │               │
      ┌─────────▼──────┐  ┌─────▼──────────────┐
      │searchable_     │  │searchable_         │
      │array_bag       │  │tree_bag            │
      └────────────────┘  └────────────────────┘
                │               │
                └───────┬───────┘
                        │
                   ┌────▼────┐
                   │   set   │ (wrapper)
                   └─────────┘
```

---

## Key Concept: Virtual Inheritance

### The Diamond Problem

Without virtual inheritance, this would happen:

```
         bag
        /   \
   array_bag searchable_bag
        \   /
    searchable_array_bag  <- TWO copies of bag!
```

**Problem**: `searchable_array_bag` would have TWO copies of the `bag` base class, causing ambiguity and wasted memory.

**Solution**: Use `virtual` inheritance:

```cpp
class array_bag : virtual public bag { ... }
class searchable_bag : virtual public bag { ... }
```

This ensures only ONE instance of `bag` exists in the final derived class.

---

## Detailed Implementation Analysis

### 1. searchable_array_bag

#### Header File (searchable_array_bag.hpp)
```cpp
#pragma once

#include "array_bag.hpp"
#include "searchable_bag.hpp"

class searchable_array_bag : public array_bag, public searchable_bag
{
    public:
        // Orthodox Canonical Form (Rule of 3)
        searchable_array_bag();                                          // Default constructor
        searchable_array_bag(const searchable_array_bag& source);       // Copy constructor
        searchable_array_bag& operator=(const searchable_array_bag& source); // Assignment operator
        ~searchable_array_bag();                                        // Destructor
        
        // Implement pure virtual from searchable_bag
        bool has(int) const;
};
```

**Key Points**:
- Multiple inheritance from `array_bag` and `searchable_bag`
- Must implement `has()` method (pure virtual from `searchable_bag`)
- Orthodox Canonical Form: Constructor, Copy Constructor, Assignment Operator, Destructor

#### Implementation File (searchable_array_bag.cpp)

**Default Constructor**:
```cpp
searchable_array_bag::searchable_array_bag()
{
    // Empty - array_bag's constructor handles initialization
}
```
- Delegates to `array_bag()` constructor automatically
- `array_bag()` sets `size = 0` and `data = nullptr`

**Copy Constructor**:
```cpp
searchable_array_bag::searchable_array_bag(const searchable_array_bag& source) 
    : array_bag(source)
{
    // array_bag copy constructor copies all data
}
```
- Explicitly calls `array_bag(source)` to copy the array data
- `array_bag` copy constructor allocates new memory and copies elements

**Assignment Operator**:
```cpp
searchable_array_bag& searchable_array_bag::operator=(const searchable_array_bag& source)
{
    if(this != &source)  // Self-assignment check
    {
        array_bag::operator=(source);  // Delegate to base class
    }
    return(*this);
}
```
- Self-assignment guard prevents issues when `obj = obj`
- Delegates to `array_bag::operator=()` which handles deep copy

**has() Implementation**:
```cpp
bool searchable_array_bag::has(int value) const
{
    for(int i = 0; i < this->size; i++)
    {
        if(this->data[i] == value)
            return (true);
    }
    return(false);
}
```
- Linear search through array: O(n) time complexity
- Accesses protected members `size` and `data` from `array_bag`
- Const method - doesn't modify the object

**Destructor**:
```cpp
searchable_array_bag::~searchable_array_bag()
{
    // Empty - array_bag's destructor handles cleanup
}
```
- `array_bag` destructor automatically called, which `delete[]` the data

---

### 2. searchable_tree_bag

#### Header File (searchable_tree_bag.hpp)
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

**Key Points**:
- Private helper method `search()` for recursive BST traversal
- Uses `node` type from `tree_bag` (protected struct)

#### Implementation File (searchable_tree_bag.cpp)

**Constructors and Assignment** (similar pattern to array version):
```cpp
searchable_tree_bag::searchable_tree_bag() { }

searchable_tree_bag::searchable_tree_bag(const searchable_tree_bag& source) 
    : tree_bag(source) { }

searchable_tree_bag& searchable_tree_bag::operator=(const searchable_tree_bag& source)
{
    if(this != &source)
        tree_bag::operator=(source);
    return(*this);
}

searchable_tree_bag::~searchable_tree_bag() { }
```

**Recursive Search Helper**:
```cpp
bool searchable_tree_bag::search(node* node, const int value) const
{
    if(node == nullptr)           // Base case: not found
        return(false);
        
    if(node->value == value)      // Base case: found!
        return(true);
        
    else if(value < node->value)  // Search left subtree
        return(search(node->l, value));
    else                          // Search right subtree
        return(search(node->r, value));
}
```
- Binary Search Tree property: left < parent < right
- O(log n) average case, O(n) worst case (unbalanced tree)
- Recursive approach is elegant and matches tree structure

**has() Implementation**:
```cpp
bool searchable_tree_bag::has(int value) const
{
    return(search(this->tree, value));  // Start from root
}
```
- Public interface delegates to private recursive helper
- Accesses protected member `tree` from `tree_bag`

---

### 3. set Class (Wrapper Pattern)

#### Header File (set.hpp)
```cpp
#include "searchable_bag.hpp"

class set
{
    private:
        searchable_bag& bag;  // Reference to wrapped object
        
    public:
        // Deleted functions prevent copying
        set() = delete;                         // No default constructor
        set(const set& source) = delete;        // No copy constructor
        set& operator=(const set& source) = delete; // No assignment
        
        set(searchable_bag& s_bag);  // Must provide a bag
        
        // Delegate methods
        bool has(int) const;
        void insert(int);
        void insert(int*, int);
        void print() const;
        void clear();
        
        const searchable_bag& get_bag();
        
        ~set();
};
```

**Key Design Decisions**:
1. **Reference member**: `searchable_bag& bag` (not a pointer)
   - Must be initialized in constructor
   - Cannot be reassigned
   - Cannot be copied (reference can't be reseated)

2. **Deleted functions**: Prevents copying/default construction
   - `= delete` explicitly removes these operations
   - Enforces that set MUST wrap an existing bag

3. **Polymorphism**: Accepts any `searchable_bag` implementation
   - Can wrap `searchable_array_bag` or `searchable_tree_bag`
   - Virtual methods enable dynamic dispatch

#### Implementation File (set.cpp)

**Constructor**:
```cpp
set::set(searchable_bag& s_bag) : bag(s_bag)
{
    // Initialize reference in member initializer list
}
```
- References MUST be initialized in initializer list
- Cannot assign to reference in constructor body

**Unique Insert - The Key Method**:
```cpp
void set::insert(int value)
{
    if(!(this->has(value)))  // Only insert if not already present
        bag.insert(value);
}
```
- Enforces set property: no duplicates
- Uses `has()` to check existence before inserting
- This is what makes it a SET vs a BAG

**Array Insert**:
```cpp
void set::insert(int *data, int size)
{
    for(int i = 0; i < size; i++)
    {
        this->insert(data[i]);  // Calls single insert (with uniqueness check)
    }
}
```
- Iterates and calls single insert for each element
- Each element individually checked for uniqueness

**Delegation Methods**:
```cpp
bool set::has(int value) const { return(bag.has(value)); }
void set::print() const { bag.print(); }
void set::clear() { bag.clear(); }
const searchable_bag& set::get_bag() { return(this->bag); }
```
- Simple forwarding to wrapped bag
- Demonstrates wrapper/decorator pattern

**Destructor**:
```cpp
set::~set() { }
```
- Empty because we don't own the bag (it's a reference)
- Bag is owned by whoever created it and passed it to set

---

## Important C++ Concepts Demonstrated

### 1. Virtual Inheritance
**Purpose**: Solve the diamond problem in multiple inheritance

```cpp
class array_bag : virtual public bag { }
class searchable_bag : virtual public bag { }
```

Without `virtual`, `searchable_array_bag` would inherit `bag` twice, causing:
- Ambiguous member access
- Memory waste (two copies)
- Constructor confusion

### 2. Pure Virtual Functions (Abstract Methods)
```cpp
virtual bool has(int) const = 0;  // Must be implemented by derived class
```
- `= 0` makes it pure virtual
- Class becomes abstract (cannot instantiate)
- Derived classes MUST implement it

### 3. Protected Members
```cpp
class array_bag {
protected:
    int *data;
    int size;
};
```
- Accessible to derived classes but not external code
- `searchable_array_bag` can access `data` and `size`

### 4. Orthodox Canonical Form
Every class should have:
1. Default constructor
2. Copy constructor
3. Assignment operator
4. Destructor

**Why?**
- Manages resources properly (memory, file handles, etc.)
- Prevents memory leaks and double-frees
- Enables proper copying and assignment

### 5. Deleted Functions (C++11)
```cpp
set() = delete;
```
- Explicitly removes compiler-generated functions
- More clear than making them private
- Compilation error if called (not link error)

### 6. Member Initializer List
```cpp
set::set(searchable_bag& s_bag) : bag(s_bag) { }
```
- **Required** for references and const members
- More efficient than assignment in body
- Initialization vs assignment

### 7. Polymorphism
```cpp
searchable_bag *t = new searchable_tree_bag;
t->has(5);  // Calls searchable_tree_bag::has()
```
- Virtual functions enable runtime polymorphism
- Base class pointer → derived class object
- Dynamic dispatch to correct implementation

---

## Memory Management Analysis

### array_bag Memory Model
```
array_bag object:
┌─────────────┐
│ size: 3     │
│ data: ───┐  │
└──────────│──┘
           │
           └──→ Heap: [10][20][30]
```

**Operations**:
- `insert()`: Allocates new array, copies old + new, deletes old
- Destructor: `delete[] data`
- Copy constructor: Deep copy (new allocation)

### tree_bag Memory Model
```
tree_bag object:
┌─────────────┐
│ tree: ───┐  │
└──────────│──┘
           │
           └──→ Heap:      [20]
                          /    \
                       [10]    [30]
```

**Operations**:
- `insert()`: Creates new nodes, links into BST
- `destroy_tree()`: Recursive post-order deletion
- `copy_node()`: Recursive deep copy

### set Memory Model
```
set object:
┌─────────────┐
│ bag: ───┐   │  (reference, not ownership)
└─────────│───┘
          │
          └──→ Points to existing searchable_bag object
```

**Critical**: Set does NOT own the bag, just references it!

---

## Complexity Analysis

| Operation | array_bag | tree_bag (balanced) | tree_bag (worst) |
|-----------|-----------|---------------------|------------------|
| insert    | O(n)      | O(log n)            | O(n)             |
| has       | O(n)      | O(log n)            | O(n)             |
| print     | O(n)      | O(n)                | O(n)             |
| clear     | O(n)      | O(n)                | O(n)             |

**Why array insert is O(n)**:
- Must allocate new array
- Copy all existing elements
- Insert new element
- Delete old array

**Why tree insert is O(log n)**:
- Traverse height of tree
- Balanced tree height = log n
- Worst case (unbalanced) = O(n)

---

## Testing (from main.cpp)

### Test 1: Basic Operations
```cpp
searchable_bag *t = new searchable_tree_bag;
searchable_bag *a = new searchable_array_bag;

// Insert from command line arguments
for (int i = 1; i < argc; i++) {
    t->insert(atoi(argv[i]));
    a->insert(atoi(argv[i]));
}
```
- Tests polymorphism (base class pointers)
- Tests insert operations

### Test 2: Search Functionality
```cpp
std::cout << t->has(atoi(argv[i])) << std::endl;      // Should be 1
std::cout << a->has(atoi(argv[i])) << std::endl;      // Should be 1
std::cout << t->has(atoi(argv[i]) - 1) << std::endl;  // Likely 0
```
- Verifies `has()` works correctly
- Tests both present and absent values

### Test 3: Copy Constructor
```cpp
const searchable_array_bag tmp(static_cast<searchable_array_bag &>(*a));
tmp.print();
tmp.has(1);
```
- Tests copy constructor
- Tests const correctness

### Test 4: Set Uniqueness
```cpp
set sa(*a);
for (int i = 1; i < argc; i++) {
    sa.insert(atoi(argv[i]));  // Duplicate inserts
}
sa.print();  // Should show no duplicates
```
- Verifies set prevents duplicates
- Tests wrapper pattern

---

## Common Pitfalls and Solutions

### Pitfall 1: Forgetting Virtual Inheritance
```cpp
// WRONG
class array_bag : public bag { }
class searchable_bag : public bag { }
```
**Result**: Diamond problem, compilation errors, ambiguous calls

**Solution**: Use `virtual public`

### Pitfall 2: Not Initializing References
```cpp
// WRONG
set::set(searchable_bag& s_bag) {
    bag = s_bag;  // Error! Can't assign to reference
}
```
**Solution**: Use initializer list: `: bag(s_bag)`

### Pitfall 3: Memory Leaks in Assignment
```cpp
// WRONG
array_bag& operator=(const array_bag& src) {
    data = new int[src.size];  // Leak! Old data not freed
    // ...
}
```
**Solution**: Delete old data first (see actual implementation)

### Pitfall 4: Shallow Copy
```cpp
// WRONG
array_bag(const array_bag& src) {
    data = src.data;  // Both point to same array!
}
```
**Solution**: Allocate new memory and copy elements (deep copy)

### Pitfall 5: Forgetting Self-Assignment Check
```cpp
// WRONG
array_bag& operator=(const array_bag& src) {
    delete[] data;
    data = new int[src.size];
    // If this == &src, we just deleted src.data!
}
```
**Solution**: Always check `if (this != &src)`

---

## Summary

This exercise demonstrates:

1. **Multiple Inheritance**: Combining functionality from multiple base classes
2. **Virtual Inheritance**: Solving the diamond problem elegantly
3. **Abstract Classes**: Defining interfaces with pure virtual functions
4. **Polymorphism**: Runtime dispatch based on actual object type
5. **Wrapper Pattern**: Set wraps searchable_bag to add uniqueness constraint
6. **Resource Management**: Proper memory allocation/deallocation
7. **Orthodox Canonical Form**: Complete set of constructors/operators
8. **Const Correctness**: Methods that don't modify state are marked const

The solution is elegant and demonstrates deep understanding of C++ OOP principles.
