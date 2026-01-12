# Polyset Exercise - Study Guide

## How to Study This Exercise

This guide will help you understand the Polyset exercise step by step. Work through each section, and make sure you understand each concept before moving to the next.

---

## Study Plan (5 Sessions)

### Session 1: Understanding the Foundation (30-45 minutes)
### Session 2: Virtual Inheritance Deep Dive (45-60 minutes)
### Session 3: Implementing searchable_array_bag (30-45 minutes)
### Session 4: Implementing searchable_tree_bag (45-60 minutes)
### Session 5: The Set Wrapper Pattern (30-45 minutes)

---

## SESSION 1: Understanding the Foundation

### What You Need to Know First

#### 1. What is a Bag?
A **bag** (or multiset) is a collection that:
- ✅ Allows duplicates (unlike a set)
- ✅ Has no specific order
- ✅ Supports insert, print, clear operations

**Example**:
```
Bag: [5, 3, 5, 7, 3]  ← duplicates allowed
Set: [3, 5, 7]        ← no duplicates
```

#### 2. What is a Set?
A **set** is a collection that:
- ❌ NO duplicates allowed
- ✅ Has no specific order
- ✅ Each element appears at most once

#### 3. Abstract Classes in C++

**Abstract Class** = A class with at least one pure virtual function

```cpp
class bag {
public:
    virtual void insert(int) = 0;  // = 0 makes it PURE VIRTUAL
    virtual void print() const = 0;
};
```

**Key Points**:
- Cannot create objects of abstract class: `bag b;` ← ❌ ERROR
- Must inherit and implement all pure virtual functions
- Used to define interfaces/contracts

#### 4. Review the Given Classes

**Read and understand these files in order**:

1. **bag.hpp** - The abstract interface
   ```cpp
   class bag {
   public:
       virtual void insert(int) = 0;           // Add one element
       virtual void insert(int *, int) = 0;    // Add array of elements
       virtual void print() const = 0;         // Display contents
       virtual void clear() = 0;               // Remove all elements
   };
   ```

2. **searchable_bag.hpp** - Adds searching capability
   ```cpp
   class searchable_bag : virtual public bag {
   public:
       virtual bool has(int) const = 0;  // Check if element exists
   };
   ```
   
   ❓ **Question**: Why `virtual public bag`?
   📝 **Answer**: To prevent the diamond problem (explained in Session 2)

3. **array_bag.hpp** - Dynamic array implementation
   ```cpp
   class array_bag : virtual public bag {
   protected:
       int *data;   // Dynamic array
       int size;    // Number of elements
   public:
       array_bag();
       array_bag(const array_bag &);
       array_bag &operator=(const array_bag &);
       ~array_bag();
       // Implement all virtual methods from bag
   };
   ```

4. **tree_bag.hpp** - Binary search tree implementation
   ```cpp
   class tree_bag : virtual public bag {
   protected:
       struct node {
           node *l;      // Left child
           node *r;      // Right child
           int value;
       };
       node *tree;  // Root of BST
   public:
       // Orthodox canonical form + virtual methods
   };
   ```

### 📚 Study Tasks for Session 1

1. ✅ Draw the class hierarchy on paper
2. ✅ Identify which classes are abstract and why
3. ✅ List all pure virtual functions
4. ✅ Trace how `array_bag::insert()` works (read the .cpp file)
5. ✅ Trace how `tree_bag::insert()` works
6. ✅ Understand the difference between bag and set

### 🧪 Self-Test Questions

1. Can you create `bag b;`? Why or why not?
2. What does `virtual` keyword before a function do?
3. What does `= 0` after a function declaration mean?
4. Why is `data` and `size` marked as `protected` in `array_bag`?
5. What is the time complexity of `array_bag::insert()`?

**Answers**:
1. No - bag is abstract (has pure virtual functions)
2. Enables polymorphism - derived classes can override
3. Makes the function pure virtual (abstract method)
4. So derived classes can access them, but external code cannot
5. O(n) - must allocate new array and copy all elements

---

## SESSION 2: Virtual Inheritance Deep Dive

### The Diamond Problem

This is THE MOST IMPORTANT concept for this exercise.

#### Scenario WITHOUT Virtual Inheritance

```
         bag
        /   \
   array_bag  searchable_bag
        \   /
  searchable_array_bag
```

**What happens?**

```cpp
class bag { int x; };
class array_bag : public bag { };
class searchable_bag : public bag { };
class searchable_array_bag : public array_bag, public searchable_bag { };

// searchable_array_bag contains:
// - one 'x' from array_bag's bag
// - one 'x' from searchable_bag's bag
// TWO COPIES OF bag!
```

**Problems**:
1. Memory waste (duplicate base class)
2. Ambiguity: Which `x` do you mean?
3. Constructor confusion

#### Solution: Virtual Inheritance

```cpp
class array_bag : virtual public bag { };
class searchable_bag : virtual public bag { };
class searchable_array_bag : public array_bag, public searchable_bag { };
```

**Result**: Only ONE instance of `bag` in `searchable_array_bag`

### How Virtual Inheritance Works

**Normal Inheritance**:
```
searchable_array_bag object in memory:
[bag part from array_bag]
[array_bag specific parts]
[bag part from searchable_bag]  ← DUPLICATE!
[searchable_bag specific parts]
[searchable_array_bag parts]
```

**Virtual Inheritance**:
```
searchable_array_bag object in memory:
[shared bag part]  ← Only ONE copy
[array_bag specific parts]
[searchable_bag specific parts]
[searchable_array_bag parts]
```

### 🎯 Visual Example

Let's trace what happens when you create `searchable_array_bag`:

```cpp
searchable_array_bag sab;
```

**Constructor Call Order** (with virtual inheritance):
1. `bag()` - called ONCE (shared base)
2. `array_bag()` - initializes data and size
3. `searchable_bag()` - no data members
4. `searchable_array_bag()` - final derived class

Without virtual inheritance, `bag()` would be called TWICE (once through each path).

### 📚 Study Tasks for Session 2

1. ✅ Draw the diamond diagram
2. ✅ Explain why we need `virtual public bag`
3. ✅ Create a small test program demonstrating the diamond problem
4. ✅ Understand vtable and virtual pointers (advanced)

### 🧪 Self-Test Questions

1. What problem does virtual inheritance solve?
2. Why does `array_bag` use `virtual public bag` instead of just `public bag`?
3. If we didn't use virtual inheritance, what error would we get?
4. Draw the memory layout with and without virtual inheritance

---

## SESSION 3: Implementing searchable_array_bag

### Step-by-Step Implementation

#### Step 1: Understand the Requirements

`searchable_array_bag` must:
- ✅ Be both an `array_bag` (has array operations)
- ✅ Be a `searchable_bag` (has search operations)
- ✅ Implement `has()` method
- ✅ Follow Orthodox Canonical Form

#### Step 2: Class Declaration

```cpp
class searchable_array_bag : public array_bag, public searchable_bag
{
    // No new data members! We inherit everything we need
    
public:
    // Orthodox Canonical Form
    searchable_array_bag();                               // 1. Default constructor
    searchable_array_bag(const searchable_array_bag&);   // 2. Copy constructor
    searchable_array_bag& operator=(const searchable_array_bag&); // 3. Assignment
    ~searchable_array_bag();                             // 4. Destructor
    
    // Implement pure virtual from searchable_bag
    bool has(int) const;
};
```

**Why no data members?**
- We inherit `data` and `size` from `array_bag`
- We don't need anything else!

#### Step 3: Default Constructor

```cpp
searchable_array_bag::searchable_array_bag()
{
    // Empty body - array_bag constructor is called automatically
}
```

**What happens automatically?**
1. `bag()` constructor (virtual base)
2. `array_bag()` constructor → sets `size = 0`, `data = nullptr`
3. `searchable_bag()` constructor (no-op)
4. Our constructor body executes (empty)

#### Step 4: Copy Constructor

```cpp
searchable_array_bag::searchable_array_bag(const searchable_array_bag& source) 
    : array_bag(source)  // ← Explicitly call base copy constructor
{
    // array_bag(source) does the deep copy
}
```

**Why explicit call?**
- We need to copy the array data
- `array_bag(source)` already knows how to do this
- Don't reinvent the wheel!

**What `array_bag(source)` does**:
```cpp
array_bag::array_bag(const array_bag &src) {
    size = src.size;
    data = new int[size];           // Allocate NEW memory
    for (int i = 0; i < size; i++) {
        data[i] = src.data[i];      // Copy elements
    }
}
```

#### Step 5: Assignment Operator

```cpp
searchable_array_bag& searchable_array_bag::operator=(const searchable_array_bag& source)
{
    if(this != &source)  // ← CRITICAL: Self-assignment check
    {
        array_bag::operator=(source);  // Delegate to base
    }
    return(*this);
}
```

**Why self-assignment check?**
```cpp
searchable_array_bag a;
a = a;  // Without check, we'd delete our own data!
```

**What `array_bag::operator=` does**:
```cpp
array_bag& array_bag::operator=(const array_bag &src) {
    if (this != &src) {
        delete[] data;              // Free old memory
        size = src.size;
        data = new int[size];       // Allocate new memory
        for (int i = 0; i < size; i++) {
            data[i] = src.data[i];  // Copy elements
        }
    }
    return *this;
}
```

#### Step 6: Implementing has()

This is the ONLY new functionality we add!

```cpp
bool searchable_array_bag::has(int value) const
{
    for(int i = 0; i < this->size; i++)  // Access protected member from array_bag
    {
        if(this->data[i] == value)       // Found it!
            return (true);
    }
    return(false);  // Not found
}
```

**Algorithm**: Linear search
- Start at index 0
- Check each element
- Return true if found
- Return false if reach end

**Time Complexity**: O(n) - must check every element in worst case

**Why can we access `size` and `data`?**
- They're `protected` in `array_bag`
- We inherit from `array_bag`
- Protected members accessible to derived classes

#### Step 7: Destructor

```cpp
searchable_array_bag::~searchable_array_bag()
{
    // Empty - array_bag destructor handles cleanup
}
```

**What happens automatically?**
1. Our destructor body executes (empty)
2. `searchable_bag` destructor (no-op)
3. `array_bag` destructor → `delete[] data`
4. `bag` destructor (virtual base)

### 📚 Study Tasks for Session 3

1. ✅ Implement `searchable_array_bag` from scratch without looking
2. ✅ Trace what happens when you create, copy, and destroy an object
3. ✅ Draw memory diagrams for each operation
4. ✅ Explain why we can access `data` and `size`

### 🧪 Self-Test Questions

1. Why is the default constructor body empty?
2. What would happen without the self-assignment check?
3. Why do we call `array_bag(source)` in the copy constructor?
4. What is the time complexity of `has()`? Can we improve it?
5. Why is `has()` marked as `const`?

**Answers**:
1. Base class constructor handles all initialization
2. We'd delete our own data when doing `obj = obj`
3. To properly copy the array data (deep copy)
4. O(n) - No, array is unsorted, must check all elements
5. It doesn't modify the object, just searches

---

## SESSION 4: Implementing searchable_tree_bag

### Understanding Binary Search Trees First

#### BST Property
For every node:
- All values in LEFT subtree < node value
- All values in RIGHT subtree > node value

```
        20
       /  \
     10    30
    /  \
   5   15
```

Search for 15:
1. Start at 20: 15 < 20, go LEFT
2. At 10: 15 > 10, go RIGHT
3. At 15: Found! Return true

**Time Complexity**: O(log n) average, O(n) worst case

### Step-by-Step Implementation

#### Step 1: Class Declaration

```cpp
class searchable_tree_bag : public tree_bag, public searchable_bag
{
private:
    bool search(node* node, const int value) const;  // Recursive helper
    
public:
    searchable_tree_bag();
    searchable_tree_bag(const searchable_tree_bag&);
    searchable_tree_bag& operator=(const searchable_tree_bag&);
    ~searchable_tree_bag();
    
    bool has(int) const;
};
```

**Key difference from array version**:
- Private helper method `search()` for recursion
- Uses `node*` type from `tree_bag`

#### Step 2: Constructors and Destructor

Same pattern as `searchable_array_bag`:

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

All work delegated to `tree_bag`!

#### Step 3: Recursive Search Helper

This is the CORE algorithm:

```cpp
bool searchable_tree_bag::search(node* node, const int value) const
{
    // Base case 1: Reached null (not found)
    if(node == nullptr)
        return(false);
    
    // Base case 2: Found the value
    if(node->value == value)
        return(true);
    
    // Recursive case: Search left or right
    else if(value < node->value)
        return(search(node->l, value));  // Search left subtree
    else
        return(search(node->r, value));  // Search right subtree
}
```

**Understanding Recursion**:

Example: Search for 15 in tree
```
        20
       /  \
     10    30
       \
       15
```

Call trace:
```
search(root, 15)
  → node = 20, value = 15
  → 15 < 20, so search(node->l, 15)
    → node = 10, value = 15
    → 15 > 10, so search(node->r, 15)
      → node = 15, value = 15
      → 15 == 15, return TRUE
    → return TRUE
  → return TRUE
```

**Why recursion works**:
- Tree is a recursive structure (each subtree is also a tree)
- Recursion naturally matches the structure
- Each call handles one node and delegates to children

#### Step 4: Public has() Method

```cpp
bool searchable_tree_bag::has(int value) const
{
    return(search(this->tree, value));  // Start from root
}
```

**Design Pattern**: Public interface delegates to private implementation
- `has()` is the public API
- `search()` is the internal recursive helper
- Separates interface from implementation

### Comparing Array vs Tree Search

| Aspect | Array Search | Tree Search |
|--------|-------------|-------------|
| Algorithm | Linear scan | Binary search (recursive) |
| Complexity | O(n) always | O(log n) average, O(n) worst |
| Code | Simple loop | Recursive function |
| Data access | `data[i]` | `node->l`, `node->r` |

### 📚 Study Tasks for Session 4

1. ✅ Draw a BST and trace searching for various values
2. ✅ Implement `searchable_tree_bag` from scratch
3. ✅ Understand why recursion is natural for trees
4. ✅ Compare with `searchable_array_bag` implementation

### 🧪 Self-Test Questions

1. Why do we need a separate `search()` helper method?
2. What are the two base cases in the recursive search?
3. Why is tree search faster than array search (on average)?
4. What happens if the tree is completely unbalanced?
5. Could we implement `search()` iteratively instead of recursively?

**Answers**:
1. To recursively traverse the tree starting from any node
2. `node == nullptr` (not found) and `node->value == value` (found)
3. BST property allows eliminating half the tree each step
4. Degenerates to O(n) - like a linked list
5. Yes, using a loop and explicit stack/queue, but recursion is cleaner

---

## SESSION 5: The Set Wrapper Pattern

### Understanding the Wrapper Pattern

**Wrapper** (or **Decorator**) pattern:
- Take an existing object
- Add new behavior
- Delegate most work to wrapped object

```
┌─────────────────────┐
│       Set           │
│                     │
│  ┌───────────────┐  │
│  │ wrapped bag   │  │  ← Reference to searchable_bag
│  └───────────────┘  │
│                     │
│  + insert(int)      │  ← Adds uniqueness check
│  + has(int)         │  ← Delegates
│  + print()          │  ← Delegates
│  + clear()          │  ← Delegates
└─────────────────────┘
```

### The Key Difference: Bag vs Set

**Bag** allows duplicates:
```cpp
bag b;
b.insert(5);
b.insert(5);
b.insert(5);
b.print();  // Output: 5 5 5
```

**Set** prevents duplicates:
```cpp
set s(some_searchable_bag);
s.insert(5);
s.insert(5);  // ← Will NOT add (already exists)
s.insert(5);  // ← Will NOT add (already exists)
s.print();    // Output: 5
```

### Step-by-Step Implementation

#### Step 1: Class Declaration

```cpp
class set
{
private:
    searchable_bag& bag;  // ← Reference to wrapped object
    
public:
    // Delete copy/assignment - sets can't be copied
    set() = delete;
    set(const set& source) = delete;
    set& operator=(const set& source) = delete;
    
    // Must provide a bag
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

**Design Decisions**:

1. **Reference member** (`searchable_bag& bag`):
   - Not a pointer (`searchable_bag*`)
   - Not by value (`searchable_bag`)
   - Must be initialized in constructor
   - Cannot be changed after construction

2. **Deleted functions** (`= delete`):
   - Prevents default construction
   - Prevents copying
   - More explicit than making private
   - Compiler error instead of linker error

3. **Why reference, not pointer?**
   ```cpp
   // With reference:
   searchable_bag& bag;  // Cannot be null, cannot be changed
   
   // With pointer:
   searchable_bag* bag;  // Could be null, could be changed
   ```

#### Step 2: Constructor

```cpp
set::set(searchable_bag& s_bag) : bag(s_bag)  // ← Initializer list!
{
    // Body is empty
}
```

**CRITICAL**: References MUST be initialized in initializer list!

```cpp
// WRONG - Won't compile
set::set(searchable_bag& s_bag) {
    bag = s_bag;  // ❌ Can't assign to reference!
}

// RIGHT
set::set(searchable_bag& s_bag) : bag(s_bag)  // ✅ Initialize in list
{
}
```

#### Step 3: The Magic Method - Unique Insert

This is what makes a set a SET!

```cpp
void set::insert(int value)
{
    if(!(this->has(value)))  // ← Only insert if NOT already present
        bag.insert(value);
}
```

**Trace Example**:
```cpp
set s(some_bag);
s.insert(5);  // has(5)? No → insert
s.insert(3);  // has(3)? No → insert
s.insert(5);  // has(5)? YES → DON'T insert
```

**Result**: Each element appears only once!

#### Step 4: Array Insert

```cpp
void set::insert(int *data, int size)
{
    for(int i = 0; i < size; i++)
    {
        this->insert(data[i]);  // ← Call single insert (with check)
    }
}
```

**Example**:
```cpp
int arr[] = {5, 3, 5, 7, 3};
s.insert(arr, 5);
// Calls: insert(5), insert(3), insert(5), insert(7), insert(3)
// Result: Only 5, 3, 7 are added (no duplicates)
```

#### Step 5: Delegation Methods

```cpp
bool set::has(int value) const 
{ 
    return(bag.has(value)); 
}

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
```

**Pure delegation** - just forward to the wrapped bag!

#### Step 6: Destructor

```cpp
set::~set() 
{
    // Empty - we don't OWN the bag
}
```

**Why empty?**
- `bag` is a reference, not owned by us
- Whoever created the bag owns it
- They're responsible for deleting it

### Polymorphism in Action

```cpp
searchable_array_bag array_impl;  // Array-based
searchable_tree_bag tree_impl;    // Tree-based

set s1(array_impl);  // Set using array
set s2(tree_impl);   // Set using tree

s1.insert(5);  // Calls array_impl.insert(5)
s2.insert(5);  // Calls tree_impl.insert(5)
```

**Same set interface**, different underlying implementations!

### 📚 Study Tasks for Session 5

1. ✅ Explain the wrapper pattern in your own words
2. ✅ Implement `set` from scratch
3. ✅ Understand why references must be initialized in initializer list
4. ✅ Trace what happens when inserting duplicates
5. ✅ Understand the difference between wrapping vs inheriting

### 🧪 Self-Test Questions

1. Why does set use a reference instead of a pointer?
2. Why are copy constructor and assignment deleted?
3. What's the difference between `bag.insert()` and `set.insert()`?
4. Who is responsible for deleting the wrapped bag?
5. Why can set work with both array and tree implementations?

**Answers**:
1. References can't be null and can't be reassigned - safer
2. Set wraps a specific bag reference - copying doesn't make sense
3. `bag.insert()` always adds; `set.insert()` checks for duplicates first
4. Whoever created the bag (set just holds a reference)
5. Polymorphism - both implement `searchable_bag` interface

---

## Practice Exercises

### Exercise 1: Trace Object Creation
```cpp
searchable_array_bag sab;
```
List every constructor called, in order.

### Exercise 2: Memory Diagram
Draw the memory layout for:
```cpp
searchable_tree_bag stb;
stb.insert(20);
stb.insert(10);
stb.insert(30);
```

### Exercise 3: Implement from Scratch
Without looking at the solution, implement:
1. `searchable_array_bag`
2. `searchable_tree_bag`
3. `set`

### Exercise 4: Test Uniqueness
Write a test program that demonstrates set prevents duplicates.

### Exercise 5: Complexity Analysis
For each operation, give the time complexity:
- `searchable_array_bag::has()`
- `searchable_tree_bag::has()`
- `set::insert()` with array backend
- `set::insert()` with tree backend

---

## Common Mistakes and How to Avoid Them

### Mistake 1: Forgetting Virtual Inheritance
```cpp
// ❌ WRONG
class array_bag : public bag { }
```
**Fix**: `class array_bag : virtual public bag { }`

### Mistake 2: Not Using Initializer List for References
```cpp
// ❌ WRONG
set::set(searchable_bag& s_bag) {
    bag = s_bag;
}
```
**Fix**: `set::set(searchable_bag& s_bag) : bag(s_bag) { }`

### Mistake 3: Forgetting Self-Assignment Check
```cpp
// ❌ WRONG
searchable_array_bag& operator=(const searchable_array_bag& src) {
    array_bag::operator=(src);  // What if this == &src?
    return *this;
}
```
**Fix**: Add `if(this != &src)` check

### Mistake 4: Shallow Copy
```cpp
// ❌ WRONG
searchable_array_bag(const searchable_array_bag& src) {
    data = src.data;  // Both point to same array!
}
```
**Fix**: Call `array_bag(src)` which does deep copy

### Mistake 5: Not Marking const Methods
```cpp
// ❌ WRONG
bool has(int value) {  // Missing const
```
**Fix**: `bool has(int value) const {`

---

## Final Checklist

Before you finish studying, make sure you can:

- [ ] Explain the diamond problem and why virtual inheritance solves it
- [ ] Draw the complete class hierarchy
- [ ] List all pure virtual functions and where they're implemented
- [ ] Explain the difference between bag and set
- [ ] Implement `searchable_array_bag` from scratch
- [ ] Implement `searchable_tree_bag` from scratch
- [ ] Implement `set` from scratch
- [ ] Explain why references must be initialized in initializer list
- [ ] Trace the recursive tree search algorithm
- [ ] Explain the wrapper pattern used in set
- [ ] Understand when to use protected vs private
- [ ] Know the time complexity of each operation
- [ ] Explain Orthodox Canonical Form and why it's important

---

## Additional Resources for Deep Learning

### Topics to Research Further:
1. **Virtual Tables (vtables)** - How polymorphism works under the hood
2. **RAII** - Resource Acquisition Is Initialization
3. **Rule of 3/5/0** - Modern C++ resource management
4. **Smart Pointers** - `unique_ptr`, `shared_ptr`
5. **Template Pattern** vs Wrapper Pattern
6. **Strategy Pattern** - Another approach to this problem

### Recommended Practice:
1. Implement a `sorted_set` that keeps elements sorted
2. Add a `remove()` method to all classes
3. Implement iterators for traversing the collections
4. Add exception handling for edge cases
5. Write comprehensive unit tests

Good luck with your studies! 🚀
