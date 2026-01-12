# BSQ Reference Guide

Complete implementation reference for the **BSQ** (Biggest Square) program.

---

## Program Structure

```
bsq.h         - Header with structures and function declarations
bsq.c         - Core algorithm implementation
main.c        - Entry point and file handling
```

---

## Headers Required

```c
#include <stdio.h>    // FILE, fprintf, fscanf, getline
#include <stdlib.h>   // malloc, calloc, realloc, free
#include <unistd.h>   // Optional for POSIX functions
```

---

## Data Structures

### Elements Structure
```c
typedef struct s_elements {
    int n_lines;      // Number of lines in the map
    char empty;       // Empty cell character
    char obstacle;    // Obstacle character
    char full;        // Character to fill the square
} t_elements;
```
**Purpose:** Store metadata from the first line of input.

---

### Map Structure
```c
typedef struct s_map {
    char** grid;      // 2D array of characters
    int width;        // Number of columns
    int height;       // Number of rows
} t_map;
```
**Purpose:** Store the entire map grid.

---

### Square Structure
```c
typedef struct s_square {
    int size;         // Side length of the square
    int i;            // Top-left row coordinate
    int j;            // Top-left column coordinate
} t_square;
```
**Purpose:** Store information about the largest square found.

---

## Function Reference

### 1. loadElements()

```c
int loadElements(FILE* file, t_elements* elements)
{
    int ret = fscanf(file, "%d %c %c %c", 
                     &(elements->n_lines), 
                     &(elements->empty), 
                     &(elements->obstacle), 
                     &(elements->full));
    
    // Validation: check fscanf success
    if (ret != 4)
        return -1;
    
    // Validation: positive number of lines
    if (elements->n_lines <= 0)
        return -1;
    
    // Validation: unique characters
    if (elements->empty == elements->obstacle || 
        elements->empty == elements->full || 
        elements->obstacle == elements->full)
        return -1;
    
    // Validation: printable characters (ASCII 32-126)
    if (elements->empty < 32 || elements->empty > 126)
        return -1;
    if (elements->obstacle < 32 || elements->obstacle > 126)
        return -1;
    if (elements->full < 32 || elements->full > 126)
        return -1;
    
    return 0;
}
```

**Purpose:** Parse and validate the first line of input  
**Returns:** 0 on success, -1 on error  
**Validations:**
- Exactly 4 values read
- Number of lines > 0
- All three characters are unique
- All characters are printable

---

### 2. ft_substr()

```c
char* ft_substr(char* arr, int start, int len)
{
    char* str = (char*)malloc(len + 1);
    if (!str)
        return NULL;
    
    int i = 0;
    int j = 0;
    while (arr[i]) {
        if ((i >= start) && (j < len)) {
            str[j] = arr[i];
            j++;
        }
        i++;
    }
    str[j] = '\0';
    return str;
}
```

**Purpose:** Extract a substring (similar to standard substr)  
**Parameters:**
- `arr`: Source string
- `start`: Starting index
- `len`: Length to extract
**Returns:** Newly allocated substring, or NULL on malloc failure  
**Note:** Caller must free the returned string

---

### 3. free_map()

```c
void free_map(char** arr)
{
    int i = 0;
    if (arr) {
        while (arr[i] != NULL) {
            if (arr[i])
                free(arr[i]);
            i++;
        }
        free(arr);
    }
}
```

**Purpose:** Free a 2D array of strings  
**Process:**
1. Free each row
2. Free the array of pointers
**Important:** Always call on error paths to prevent leaks

---

### 4. element_control()

```c
int element_control(char** map, char c1, char c2)
{
    int i = 0;
    while (map[i]) {
        int j = 0;
        while (map[i][j] != '\0') {
            if ((map[i][j] != c1) && (map[i][j] != c2))
                return -1;
            j++;
        }
        i++;
    }
    return 0;
}
```

**Purpose:** Validate that map contains only allowed characters  
**Parameters:**
- `map`: The 2D grid
- `c1`: First allowed character (empty)
- `c2`: Second allowed character (obstacle)
**Returns:** 0 if valid, -1 if invalid character found

---

### 5. loadMap()

```c
int loadMap(FILE* file, t_map* map, t_elements* elements)
{
    map->height = elements->n_lines;
    map->grid = (char**)malloc((map->height + 1) * sizeof(char*));
    map->grid[map->height] = NULL;
    
    char* line = NULL;
    size_t len = 0;
    
    // Skip the newline after header
    if (getline(&line, &len, file) == -1) {
        free_map(map->grid);
        return -1;
    }
    
    // Read each line of the map
    for (int i = 0; i < map->height; i++) {
        int read = getline(&line, &len, file);
        if (read == -1) {
            free(line);
            free_map(map->grid);
            return -1;
        }
        
        // Check for newline at end
        if (line[read - 1] == '\n')
            read--;
        else {
            free(line);
            free_map(map->grid);
            return -1;
        }
        
        // Copy line without newline
        map->grid[i] = ft_substr(line, 0, read);
        if (!(map->grid[i])) {
            free(line);
            free_map(map->grid);
            return -1;
        }
        
        // Set width from first line, validate others match
        if (i == 0)
            map->width = read;
        else {
            if (map->width != read) {
                free(line);
                free_map(map->grid);
                return -1;
            }
        }
    }
    
    // Validate only allowed characters
    if (element_control(map->grid, elements->empty, elements->obstacle) == -1) {
        free(line);
        free_map(map->grid);
        return -1;
    }
    
    free(line);
    return 0;
}
```

**Purpose:** Load and validate the entire map from file  
**Process:**
1. Allocate memory for grid
2. Skip newline after header
3. Read each line with getline
4. Validate line endings
5. Store lines without newlines
6. Validate consistent width
7. Validate characters

**Returns:** 0 on success, -1 on error  
**Memory:** Cleans up on all error paths

---

### 6. find_min()

```c
int find_min(int n1, int n2, int n3)
{
    int min = n1;
    
    if (n2 < min)
        min = n2;
    if (n3 < min)
        min = n3;
    
    return min;
}
```

**Purpose:** Find minimum of three integers  
**Used in:** DP algorithm to calculate square size

---

### 7. find_big_square()

```c
void find_big_square(t_map* map, t_square* square, t_elements* elements)
{
    // Initialize DP matrix
    int matrix[map->height][map->width];
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++)
            matrix[i][j] = 0;
    }
    
    // Fill DP matrix and track maximum
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            if (map->grid[i][j] == elements->obstacle) {
                matrix[i][j] = 0;
            } else if (i == 0 || j == 0) {
                matrix[i][j] = 1;
            } else {
                int min = find_min(matrix[i-1][j], 
                                   matrix[i-1][j-1], 
                                   matrix[i][j-1]);
                matrix[i][j] = min + 1;
            }
            
            // Update maximum square found
            if (matrix[i][j] > square->size) {
                square->size = matrix[i][j];
                square->i = i - matrix[i][j] + 1;  // Top-left row
                square->j = j - matrix[i][j] + 1;  // Top-left col
            }
        }
    }
}
```

**Purpose:** Find the largest square using dynamic programming  
**Algorithm:** Maximal Square DP
- `matrix[i][j]` = side length of largest square ending at (i,j)
- Recurrence: `min(top, left, diagonal) + 1`

**Result:** Updates `square` structure with:
- `size`: Side length of largest square
- `i`, `j`: Top-left corner coordinates

**Key Insight:** By using `>` (not `>=`), we automatically prefer top-left squares when sizes are equal.

---

### 8. print_filled_square()

```c
void print_filled_square(t_map* map, t_square* square, t_elements* elements)
{
    // Fill the square with the full character
    for (int i = square->i; i < square->i + square->size; i++) {
        for (int j = square->j; j < square->j + square->size; j++) {
            if ((i < map->height) && (j < map->width))
                map->grid[i][j] = elements->full;
        }
    }
    
    // Print the resulting map
    for (int i = 0; i < map->height; i++) {
        fprintf(stdout, "%s\n", map->grid[i]);
    }
}
```

**Purpose:** Fill and print the solution  
**Process:**
1. Modify the grid to fill the square
2. Print the entire grid to stdout

**Bounds Check:** Ensures we don't write outside the grid

---

### 9. execute_bsq()

```c
int execute_bsq(FILE* file)
{
    t_elements elements;
    if (loadElements(file, &elements) == -1)
        return -1;
    
    t_map map;
    if (loadMap(file, &map, &elements) == -1)
        return -1;
    
    t_square square;
    square.size = 0;
    square.i = 0;
    square.j = 0;
    
    find_big_square(&map, &square, &elements);
    print_filled_square(&map, &square, &elements);
    
    free_map(map.grid);
    return 0;
}
```

**Purpose:** Main execution flow  
**Steps:**
1. Parse header
2. Load map
3. Find largest square
4. Fill and print result
5. Clean up memory

**Returns:** 0 on success, -1 on error

---

### 10. convert_file_pointer()

```c
int convert_file_pointer(char* name)
{
    FILE* file = fopen(name, "r");
    if (!file)
        return -1;
    
    int ret = execute_bsq(file);
    fclose(file);
    
    return ret;
}
```

**Purpose:** Open file and execute BSQ  
**Process:**
1. Open file in read mode
2. Execute algorithm
3. Close file
4. Return result

---

### 11. main()

```c
int main(int argc, char* argv[])
{
    if (argc == 1) {
        // No arguments: read from stdin
        if (execute_bsq(stdin) == -1)
            fprintf(stderr, "map error\n");
    } else if (argc == 2) {
        // Single file argument
        if (convert_file_pointer(argv[1]) == -1)
            fprintf(stderr, "map error\n");
    } else {
        // Multiple file arguments
        int i = 1;
        while (i < argc) {
            if (convert_file_pointer(argv[i]) == -1)
                fprintf(stderr, "map error\n");
            i++;
            if (i < argc)
                fprintf(stdout, "\n");
        }
    }
    return 0;
}
```

**Purpose:** Entry point and argument handling  
**Cases:**
1. **No arguments:** Read from stdin
2. **One argument:** Process that file
3. **Multiple arguments:** Process each, separate with newline

**Error Handling:** Print "map error" to stderr on failure

---

## Algorithm Details

### Dynamic Programming Recurrence

```
For each cell (i, j):
    
    If grid[i][j] is obstacle:
        dp[i][j] = 0
    
    Else if i == 0 or j == 0:
        dp[i][j] = 1
    
    Else:
        dp[i][j] = min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1
```

### Visual Example

```
Map:              DP Matrix:
. . . .           1 1 1 1
. . . .           1 2 2 2
. . . .           1 2 3 3
. . . .           1 2 3 4
```

At position (3, 3):
- Top: dp[2][3] = 3
- Left: dp[3][2] = 3
- Diagonal: dp[2][2] = 2
- min(3, 3, 2) + 1 = 3

Wait, the value should be 3, but in the matrix it shows 4. Let me recalculate:

Actually for position (3,3):
- Top: dp[2][3] = 3
- Left: dp[3][2] = 3  
- Diagonal: dp[2][2] = 3
- min(3, 3, 3) + 1 = 4 ✓

### Finding Top-Left Corner

Given bottom-right at (i, j) with size s:
```c
top_left_i = i - s + 1;
top_left_j = j - s + 1;
```

Example: If bottom-right is (3, 3) with size 4:
- top_left_i = 3 - 4 + 1 = 0
- top_left_j = 3 - 4 + 1 = 0

The square spans from (0,0) to (3,3), which is correct!

---

## Input/Output Examples

### Example 1: Basic
**Input:**
```
3 . o x
...
...
...
```

**Output:**
```
xxx
xxx
xxx
```

### Example 2: With Obstacles
**Input:**
```
5 . o x
.....
..o..
.....
.....
..o..
```

**Output:**
```
xx...
xxo..
xx...
xx...
..o..
```

### Example 3: Invalid - Duplicate Chars
**Input:**
```
3 . . x
...
...
...
```

**Output to stderr:**
```
map error
```

### Example 4: Multiple Files
**Command:**
```bash
./bsq map1.txt map2.txt
```

**Output:**
```
[result of map1]

[result of map2]
```

---

## Memory Management

### Allocation Points
1. **Map grid:** `malloc((height + 1) * sizeof(char*))`
2. **Each row:** `ft_substr()` allocates
3. **getline buffer:** Managed by getline

### Deallocation Points
1. **free_map():** Frees grid and all rows
2. **free(line):** After loading map
3. **On error:** All error paths must free

### Leak Prevention
```c
// Always check and free on error
if (error_condition) {
    free(line);
    free_map(map->grid);
    return -1;
}
```

---

## Error Handling

### Error Reporting
All errors print to **stderr**:
```c
fprintf(stderr, "map error\n");
```

### Error Conditions
1. File cannot be opened
2. Header parsing fails
3. Invalid characters in header
4. Inconsistent line widths
5. Invalid characters in map
6. malloc failure
7. Wrong number of lines

---

## Validation Checklist

### Header Validation
- [ ] Exactly 4 values parsed
- [ ] n_lines > 0
- [ ] All three chars are unique
- [ ] All chars are printable (32-126)

### Map Validation
- [ ] Exactly n_lines lines
- [ ] All lines same width
- [ ] Width > 0
- [ ] Each line ends with newline
- [ ] Only empty/obstacle chars in map

---

## Compilation & Execution

### Compile
```bash
gcc -Wall -Wextra -Werror bsq.c main.c -o bsq
```

### Run with stdin
```bash
cat map.txt | ./bsq
```

### Run with file
```bash
./bsq map.txt
```

### Run with multiple files
```bash
./bsq map1.txt map2.txt map3.txt
```

---

## Debugging Tips

### Print DP Matrix
```c
for (int i = 0; i < map->height; i++) {
    for (int j = 0; j < map->width; j++) {
        printf("%d ", matrix[i][j]);
    }
    printf("\n");
}
```

### Print Square Info
```c
printf("Square found: size=%d at (%d,%d)\n", 
       square.size, square.i, square.j);
```

### Verify Characters
```c
printf("Characters: empty='%c' obstacle='%c' full='%c'\n",
       elements.empty, elements.obstacle, elements.full);
```

---

## Performance

### Time Complexity
- Parsing: O(n) where n = total characters
- DP: O(h × w) where h = height, w = width
- Filling: O(s²) where s = square size
- **Total:** O(h × w)

### Space Complexity
- Map storage: O(h × w)
- DP matrix: O(h × w)
- **Total:** O(h × w)

---

## Common Bugs & Fixes

### Bug 1: Newline in Grid
**Problem:** Grid strings contain newlines  
**Fix:** Remove newline before storing:
```c
if (line[read - 1] == '\n')
    read--;
```

### Bug 2: Memory Leak on Error
**Problem:** Not freeing on error paths  
**Fix:** Always free before return:
```c
if (error) {
    free(line);
    free_map(map->grid);
    return -1;
}
```

### Bug 3: Wrong Square Position
**Problem:** Off-by-one in calculating top-left  
**Fix:** Remember to add 1:
```c
top_left = bottom_right - size + 1;  // Not just - size
```

### Bug 4: Uninitialized DP Matrix
**Problem:** DP array contains garbage values  
**Fix:** Initialize all cells to 0:
```c
for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
        matrix[i][j] = 0;
```

---

## Testing Commands

```bash
# Basic test
echo "3 . o x
...
...
..." | ./bsq

# Test with file
./bsq example_file

# Test multiple files
./bsq map1.txt map2.txt

# Test error handling
echo "3 . . x
..." | ./bsq 2>&1 | grep "map error"

# Memory leak check
valgrind --leak-check=full ./bsq map.txt
```

---

*This reference provides complete implementation details for the BSQ project.*
