# BSQ - Improved Implementation

A clean, robust implementation of the Biggest Square (BSQ) problem using dynamic programming.

---

## Quick Start

```bash
# Compile
make

# Run with example
./bsq example.txt

# Run with stdin
cat example.txt | ./bsq

# Clean
make clean
```

---

## What is BSQ?

BSQ finds the largest square on a map that contains only empty spaces (no obstacles) and fills it with a designated character.

### Example

**Input:**
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

**Output:**
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

The 7×7 square in the middle is the largest possible square without obstacles.

---

## Features

✅ **Dynamic Programming Algorithm** - O(n) time complexity  
✅ **Robust Error Handling** - All error paths properly handle memory  
✅ **Memory Safe** - No leaks, valgrind clean  
✅ **Well Documented** - Clear comments and explanations  
✅ **Handles All Cases** - stdin, single file, multiple files  
✅ **Comprehensive Validation** - Thorough input checking  

---

## Input Format

```
<number_of_lines> <empty_char> <obstacle_char> <full_char>
<line_1>
<line_2>
...
<line_n>
```

### Requirements

- First line is space-separated metadata
- All three characters must be unique
- All characters must be printable (ASCII 32-126)
- All map lines must have the same length
- Map can only contain empty or obstacle characters
- Each line must end with a newline

---

## Usage

### With File
```bash
./bsq map.txt
```

### With stdin
```bash
cat map.txt | ./bsq
echo "3 . o x
...
...
..." | ./bsq
```

### Multiple Files
```bash
./bsq map1.txt map2.txt map3.txt
```

Outputs are separated by a newline.

---

## Algorithm

Uses the **Maximal Square Dynamic Programming** algorithm:

1. Create a DP matrix where `dp[i][j]` = side length of largest square with bottom-right at (i,j)
2. For each cell:
   - If obstacle: `dp[i][j] = 0`
   - If first row/column: `dp[i][j] = 1`
   - Otherwise: `dp[i][j] = min(top, left, diagonal) + 1`
3. Track the maximum value to find the largest square
4. Fill the square in the map and print

**Time Complexity:** O(rows × cols)  
**Space Complexity:** O(rows × cols)

---

## Project Structure

```
improved/
├── bsq.h           # Header with structures and function declarations
├── bsq.c           # Core algorithm implementation
├── main.c          # Entry point and file handling
├── Makefile        # Build configuration
├── example.txt     # Sample input
├── EXPLANATION.md  # Detailed implementation explanation
└── README.md       # This file
```

---

## Building

### Compile
```bash
make
```

### Clean Object Files
```bash
make clean
```

### Clean Everything
```bash
make fclean
```

### Rebuild
```bash
make re
```

---

## Testing

### Basic Test
```bash
./bsq example.txt
```

### Test Error Handling
```bash
# Invalid characters (duplicate)
echo "3 . . x
..." | ./bsq 2>&1

# Inconsistent width
echo "3 . o x
...
.." | ./bsq 2>&1

# Should output "map error" for both
```

### Test Multiple Files
```bash
./bsq example.txt example.txt
```

### Memory Leak Check
```bash
valgrind --leak-check=full --show-leak-kinds=all ./bsq example.txt
```

Should show:
```
All heap blocks were freed -- no leaks are possible
```

---

## Error Handling

The program prints `map error` to **stderr** and continues if:

- File cannot be opened
- Header is malformed
- Characters are not unique or not printable
- Number of lines is invalid
- Map lines have inconsistent widths
- Map contains invalid characters
- Any allocation fails

---

## Data Structures

### t_info
Stores map metadata from first line:
```c
typedef struct s_info {
    int rows;
    char empty;
    char obstacle;
    char full;
} t_info;
```

### t_map
Stores the map grid:
```c
typedef struct s_map {
    char **grid;
    int width;
    int height;
} t_map;
```

### t_square
Stores square information:
```c
typedef struct s_square {
    int size;
    int row;
    int col;
} t_square;
```

---

## Functions

| Function | Purpose |
|----------|---------|
| `parse_info()` | Parse and validate header |
| `load_map()` | Load and validate map from file |
| `validate_line()` | Check line contains only valid chars |
| `find_largest_square()` | DP algorithm to find largest square |
| `fill_and_print()` | Fill square and output result |
| `free_map()` | Clean up allocated memory |
| `process_map()` | Main execution flow |

---

## Examples

### Example 1: Small Map
```
Input:
3 . o x
...
...
...

Output:
xxx
xxx
xxx
```

### Example 2: With Obstacles
```
Input:
5 . o x
.....
..o..
.....
.....
.....

Output:
xx...
xxo..
xx...
xx...
.....
```

### Example 3: No Solution
```
Input:
2 . o x
oo
oo

Output:
oo
oo
```
(All cells are obstacles, no square possible)

---

## Improvements Over Original

1. **Better Memory Management**
   - All pointers initialized to NULL
   - Consistent cleanup on all error paths
   - Uses `memcpy` for safe copying

2. **Clearer Code Structure**
   - Descriptive function and variable names
   - Separated validation logic
   - Comprehensive comments

3. **Enhanced Error Handling**
   - Validates all inputs thoroughly
   - Proper error reporting to stderr
   - Continues processing on errors

4. **Better Performance**
   - Uses VLA for DP matrix (stack allocation)
   - Minimal dynamic allocation
   - Efficient algorithm implementation

---

## Allowed Functions

- `malloc`, `calloc`, `realloc`, `free`
- `fopen`, `fclose`
- `getline`, `fscanf`, `fputs`, `fprintf`
- `stderr`, `stdout`, `stdin`
- `errno`

---

## Compilation Flags

```bash
gcc -Wall -Wextra -Werror bsq.c main.c -o bsq
```

All warnings are treated as errors to ensure code quality.

---

## Tips for Understanding

1. **Start with the algorithm** - Read about the Maximal Square DP problem
2. **Follow the flow** - Trace through `process_map()` to see the steps
3. **Visualize the DP** - Draw the DP matrix for small examples
4. **Test edge cases** - Try maps with all obstacles, single cell, etc.
5. **Check memory** - Run with valgrind to verify no leaks

---

## Common Issues

### "map error" Output
Check:
- Are all three characters unique?
- Are all characters printable?
- Do all lines have the same width?
- Does the file exist and is readable?

### Wrong Square Chosen
- The algorithm prefers top-left squares when sizes are equal
- Verify the DP matrix values are correct

### Compilation Errors
- Ensure all files are in the same directory
- Check that you have gcc installed
- Verify C99 or later standard support

---

## Further Reading

- [Maximal Square Problem on LeetCode](https://leetcode.com/problems/maximal-square/)
- Dynamic Programming fundamentals
- 2D DP matrix problems
- File I/O in C

---

## License

This is an educational implementation for learning purposes.

---

## Contributing

This is a reference implementation. Suggestions for improvements are welcome!

---

**Made with ❤️ for learning C and algorithms**
