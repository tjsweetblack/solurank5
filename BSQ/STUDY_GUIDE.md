# BSQ (Biggest Square) Study Guide

## Overview
The **BSQ** (Biggest Square) program finds the largest square possible on a map while avoiding obstacles. This is a classic dynamic programming problem that tests file I/O, parsing, memory management, and algorithm optimization.

---

## Table of Contents
1. [Problem Statement](#problem-statement)
2. [Core Concepts](#core-concepts)
3. [Algorithm Explanation](#algorithm-explanation)
4. [Implementation Strategy](#implementation-strategy)
5. [Input Validation](#input-validation)
6. [Edge Cases](#edge-cases)
7. [Common Pitfalls](#common-pitfalls)
8. [Testing Approach](#testing-approach)

---

## Problem Statement

### Goal
Find the biggest square on a map that contains only empty spaces (no obstacles), and fill it with a designated character.

### Input Format
```
<number_of_lines> <empty_char> <obstacle_char> <full_char>
<map_line_1>
<map_line_2>
...
<map_line_n>
```

### Example
```
9 . o x
...........................
....o......................
............o..............
...........................
....o......................
...............o...........
...........................
......o..............o.....
..o.......o................
```

### Output
The same map with the largest square filled with the "full" character:
```
.....xxxxxxx...............
....oxxxxxxx...............
.....xxxxxxxo..............
.....xxxxxxx...............
....oxxxxxxx...............
.....xxxxxxx...o...........
.....xxxxxxx...............
......o..............o.....
..o.......o................
```

---

## Core Concepts

### 1. Dynamic Programming (DP)
The key to solving BSQ efficiently is using **Dynamic Programming** with a technique often called the **Maximal Square Algorithm**.

**Intuition:**
- For each cell, calculate the maximum square size that can end at that position
- Use previously computed values to build larger squares

### 2. The DP Matrix
Create an auxiliary matrix where `dp[i][j]` represents:
> The side length of the largest square whose **bottom-right corner** is at position `(i, j)`

### 3. Recurrence Relation
```
If grid[i][j] is obstacle:
    dp[i][j] = 0

If grid[i][j] is empty:
    If i == 0 OR j == 0:
        dp[i][j] = 1
    Else:
        dp[i][j] = min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1
```

**Why this works:**
- `dp[i-1][j]`: Square above
- `dp[i][j-1]`: Square to the left
- `dp[i-1][j-1]`: Square diagonally above-left
- The minimum of these three determines the largest square we can form

### Visual Example
```
Map:          DP Matrix:
. . . .       1 1 1 1
. . . .       1 2 2 2
. . . .       1 2 3 3
. . . .       1 2 3 4

The value 4 at position (3,3) means we can form a 4×4 square 
with bottom-right corner at (3,3)
```

---

## Algorithm Explanation

### Step-by-Step Process

#### Step 1: Parse the Header
```c
// Read first line: "9 . o x"
int n_lines;
char empty, obstacle, full;
fscanf(file, "%d %c %c %c", &n_lines, &empty, &obstacle, &full);
```

**Validation:**
- `n_lines > 0`
- All three characters are different
- All characters are printable (ASCII 32-126)

#### Step 2: Load the Map
```c
char** grid = malloc(height * sizeof(char*));
for (int i = 0; i < height; i++) {
    getline(&line, &len, file);  // Read each line
    grid[i] = strdup(line);       // Store it
}
```

**Validation:**
- All lines have the same width
- Only contains empty or obstacle characters
- Each line ends with newline

#### Step 3: Build DP Matrix
```c
int dp[height][width];

for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
        if (grid[i][j] == obstacle) {
            dp[i][j] = 0;
        } else if (i == 0 || j == 0) {
            dp[i][j] = 1;
        } else {
            int min = find_min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]);
            dp[i][j] = min + 1;
        }
        
        // Track the largest square found
        if (dp[i][j] > max_size) {
            max_size = dp[i][j];
            max_i = i;
            max_j = j;
        }
    }
}
```

#### Step 4: Calculate Square Position
The DP value gives us the **bottom-right corner** and **size**.

To find the **top-left corner**:
```c
int top_left_i = max_i - max_size + 1;
int top_left_j = max_j - max_size + 1;
```

#### Step 5: Fill the Square
```c
for (int i = top_left_i; i < top_left_i + max_size; i++) {
    for (int j = top_left_j; j < top_left_j + max_size; j++) {
        grid[i][j] = full;
    }
}
```

#### Step 6: Print Result
```c
for (int i = 0; i < height; i++) {
    printf("%s\n", grid[i]);
}
```

---

## Implementation Strategy

### Data Structures

#### 1. Map Elements Structure
```c
typedef struct s_elements {
    int n_lines;
    char empty;
    char obstacle;
    char full;
} t_elements;
```
Stores the metadata from the first line.

#### 2. Map Structure
```c
typedef struct s_map {
    char** grid;
    int width;
    int height;
} t_map;
```
Stores the 2D character grid.

#### 3. Square Information
```c
typedef struct s_square {
    int size;      // Side length
    int i;         // Top-left row
    int j;         // Top-left column
} t_square;
```
Stores the result of the algorithm.

### Function Architecture

```
main()
  ├─ execute_bsq(FILE*)
  │   ├─ loadElements()       // Parse header
  │   ├─ loadMap()            // Load grid
  │   ├─ find_big_square()    // DP algorithm
  │   └─ print_filled_square() // Output
  └─ convert_file_pointer()   // File handling
```

---

## Input Validation

### Critical Validations

#### 1. Header Validation
```c
// Check return value of fscanf
if (fscanf(file, "%d %c %c %c", ...) != 4)
    return ERROR;

// Check n_lines is positive
if (n_lines <= 0)
    return ERROR;

// Check characters are unique
if (empty == obstacle || empty == full || obstacle == full)
    return ERROR;

// Check printable (ASCII 32-126)
if (empty < 32 || empty > 126)
    return ERROR;
```

#### 2. Map Validation
```c
// Check all lines same width
if (current_width != expected_width)
    return ERROR;

// Check each line ends with newline
if (line[read - 1] != '\n')
    return ERROR;

// Check only valid characters
for each character in map:
    if (char != empty && char != obstacle)
        return ERROR;
```

#### 3. Structure Validation
- At least one line
- At least one character per line
- Exactly `n_lines` lines (no more, no less)

---

## Edge Cases

### 1. Single Cell Map
```
1 . o x
.
```
**Result:** The single cell is filled.
```
x
```

### 2. All Obstacles
```
3 . o x
ooo
ooo
ooo
```
**Result:** No square possible, map stays the same.
```
ooo
ooo
ooo
```

### 3. No Obstacles
```
3 . o x
...
...
...
```
**Result:** Entire map is the square.
```
xxx
xxx
xxx
```

### 4. Multiple Same-Size Squares
```
5 . o x
.....
.....
ooo..
.....
.....
```
**Rule:** Choose the one closest to the **top**, then **leftmost**.

### 5. Long Horizontal or Vertical Map
```
1 . o x
....................
```
Square size is 1 (only 1 row).

### 6. Invalid Maps
```c
// Missing character
"9 . o"              // ERROR: missing full char

// Duplicate characters  
"9 . . x"            // ERROR: empty == obstacle

// Inconsistent line lengths
5 . o x
.....
...              // ERROR: different length
.....

// Wrong number of lines
3 . o x
.....
.....            // ERROR: only 2 lines, expected 3
```

---

## Common Pitfalls

### 1. Off-by-One Errors
❌ **Wrong:**
```c
top_left_i = max_i - max_size;  // Missing +1
```

✅ **Right:**
```c
top_left_i = max_i - max_size + 1;
```

### 2. Memory Leaks
❌ **Wrong:**
```c
if (error) {
    return -1;  // Forgot to free allocated memory!
}
```

✅ **Right:**
```c
if (error) {
    free_map(map->grid);
    free(line);
    return -1;
}
```

### 3. Newline Handling
❌ **Wrong:**
```c
grid[i] = strdup(line);  // Includes the newline!
```

✅ **Right:**
```c
if (line[read - 1] == '\n')
    read--;
grid[i] = ft_substr(line, 0, read);  // Exclude newline
```

### 4. File vs stdin Handling
❌ **Wrong:**
```c
int main(int argc, char** argv) {
    FILE* file = fopen(argv[1], "r");  // Crashes if argc == 1
}
```

✅ **Right:**
```c
if (argc == 1) {
    execute_bsq(stdin);
} else {
    FILE* file = fopen(argv[1], "r");
}
```

### 5. Multiple File Arguments
When processing multiple files, separate outputs with newline:
```c
for (int i = 1; i < argc; i++) {
    process_file(argv[i]);
    if (i < argc - 1)
        printf("\n");
}
```

### 6. DP Initialization
❌ **Wrong:**
```c
int dp[height][width];  // Uninitialized!
```

✅ **Right:**
```c
int dp[height][width];
for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
        dp[i][j] = 0;
```

### 7. Tracking Maximum
❌ **Wrong:**
```c
// Updates on every cell
if (dp[i][j] >= max_size) {  // Equal case problematic
    max_size = dp[i][j];
}
```

✅ **Right:**
```c
// Strictly greater ensures top-left preference
if (dp[i][j] > max_size) {
    max_size = dp[i][j];
    max_i = i;
    max_j = j;
}
```

---

## Testing Approach

### Test Categories

#### 1. Basic Functionality
```bash
# Simple valid map
echo "3 . o x
...
...
..." | ./bsq
```

#### 2. Edge Cases
```bash
# Single cell
echo "1 . o x
." | ./bsq

# All obstacles
echo "2 . o x
oo
oo" | ./bsq

# Empty map (should error)
echo "0 . o x" | ./bsq
```

#### 3. Invalid Input
```bash
# Duplicate characters
echo "3 . . x
..." | ./bsq  # Should print "map error"

# Inconsistent widths
echo "2 . o x
...
.." | ./bsq  # Should print "map error"
```

#### 4. Multiple Files
```bash
./bsq map1.txt map2.txt map3.txt
# Should process all, print newline between each
```

#### 5. Large Maps
```bash
# Generate large map
python3 -c "
print('1000 . o x')
for i in range(1000):
    print('.' * 1000)
" | ./bsq
```

#### 6. Memory Tests
```bash
# Check for leaks
valgrind --leak-check=full ./bsq map.txt
```

---

## Performance Considerations

### Time Complexity
- **DP Algorithm:** O(height × width)
- **Filling Square:** O(max_size²)
- **Overall:** O(n) where n = total cells

### Space Complexity
- **Map Storage:** O(height × width)
- **DP Matrix:** O(height × width)
- **Overall:** O(n)

### Optimization Tips
1. Use VLA (Variable Length Arrays) for DP matrix on stack
2. Free memory immediately after use
3. Early exit on invalid input
4. Reuse buffers where possible

---

## Debugging Strategies

### 1. Print DP Matrix
```c
// Visualize the DP values
for (int i = 0; i < height; i++) {
    for (int j = 0; j < width; j++) {
        printf("%d ", dp[i][j]);
    }
    printf("\n");
}
```

### 2. Verify Square Position
```c
printf("Square: size=%d, top_left=(%d,%d)\n", 
       square.size, square.i, square.j);
```

### 3. Check Character Values
```c
printf("empty='%c'(%d) obstacle='%c'(%d) full='%c'(%d)\n",
       empty, empty, obstacle, obstacle, full, full);
```

### 4. Trace Execution
Add debug flags:
```c
#ifdef DEBUG
    printf("Loading map: height=%d, width=%d\n", height, width);
#endif
```

---

## Key Takeaways

1. **DP is Essential:** The maximal square algorithm is the efficient solution
2. **Validation is Critical:** Most of the code is input validation
3. **Memory Management:** Always free on error paths
4. **Edge Cases Matter:** Test with minimal and extreme inputs
5. **File I/O:** Handle both files and stdin correctly
6. **Error Reporting:** Use stderr for errors, stdout for output

---

## Study Checklist

- [ ] Understand the maximal square DP algorithm
- [ ] Know how to parse space-separated values with fscanf
- [ ] Master getline for dynamic line reading
- [ ] Implement robust input validation
- [ ] Handle memory allocation and freeing correctly
- [ ] Process multiple files with proper separation
- [ ] Test with stdin and file inputs
- [ ] Check for memory leaks
- [ ] Handle all edge cases
- [ ] Print errors to stderr

---

## Additional Resources

### Related Algorithms
- **2D Kadane's Algorithm** (Maximum sum rectangle)
- **Histogram Largest Rectangle** (Related problem)

### Practice Variations
1. Find the largest rectangle (not just square)
2. Count all possible squares
3. Find k largest squares
4. Handle weighted cells

Good luck with your BSQ implementation! 🎯
