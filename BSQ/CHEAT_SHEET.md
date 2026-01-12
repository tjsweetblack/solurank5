# BSQ Cheat Sheet

Quick reference for the **BSQ** (Biggest Square) program.

---

## Problem

Find the largest square on a map without obstacles and fill it.

**Input Format:**
```
<n_lines> <empty_char> <obstacle_char> <full_char>
<map_line_1>
<map_line_2>
...
```

**Output:** Map with largest square filled.

---

## Data Structures

```c
typedef struct s_elements {
    int n_lines;
    char empty;
    char obstacle;
    char full;
} t_elements;

typedef struct s_map {
    char** grid;
    int width;
    int height;
} t_map;

typedef struct s_square {
    int size;    // Side length
    int i;       // Top-left row
    int j;       // Top-left col
} t_square;
```

---

## Core Algorithm: Dynamic Programming

### DP Recurrence

```c
For cell (i, j):
    if (grid[i][j] == obstacle):
        dp[i][j] = 0
    else if (i == 0 || j == 0):
        dp[i][j] = 1
    else:
        dp[i][j] = min(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1
```

### Find Maximum
```c
if (dp[i][j] > max_size) {
    max_size = dp[i][j];
    max_i = i;  // Bottom-right row
    max_j = j;  // Bottom-right col
}
```

### Calculate Top-Left
```c
top_left_i = max_i - max_size + 1;
top_left_j = max_j - max_size + 1;
```

---

## Essential Functions

### 1. Parse Header
```c
int loadElements(FILE* file, t_elements* elements) {
    if (fscanf(file, "%d %c %c %c", &n_lines, &empty, &obstacle, &full) != 4)
        return -1;
    
    // Validate: n_lines > 0, unique chars, printable chars
    if (n_lines <= 0)
        return -1;
    if (empty == obstacle || empty == full || obstacle == full)
        return -1;
    if (empty < 32 || empty > 126)
        return -1;
    
    return 0;
}
```

### 2. Load Map
```c
int loadMap(FILE* file, t_map* map, t_elements* elements) {
    map->grid = malloc((height + 1) * sizeof(char*));
    
    char* line = NULL;
    size_t len = 0;
    
    // Skip newline after header
    getline(&line, &len, file);
    
    for (int i = 0; i < height; i++) {
        int read = getline(&line, &len, file);
        
        // Remove newline
        if (line[read - 1] == '\n')
            read--;
        
        // Store line
        map->grid[i] = ft_substr(line, 0, read);
        
        // Validate width consistency
        if (i == 0)
            map->width = read;
        else if (map->width != read)
            return -1;
    }
    
    free(line);
    return 0;
}
```

### 3. Find Biggest Square
```c
void find_big_square(t_map* map, t_square* square, t_elements* elements) {
    int dp[height][width];
    
    // Initialize
    for (int i = 0; i < height; i++)
        for (int j = 0; j < width; j++)
            dp[i][j] = 0;
    
    // Fill DP matrix
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
            
            if (dp[i][j] > square->size) {
                square->size = dp[i][j];
                square->i = i - dp[i][j] + 1;
                square->j = j - dp[i][j] + 1;
            }
        }
    }
}
```

### 4. Fill & Print
```c
void print_filled_square(t_map* map, t_square* square, t_elements* elements) {
    // Fill square
    for (int i = square->i; i < square->i + square->size; i++) {
        for (int j = square->j; j < square->j + square->size; j++) {
            map->grid[i][j] = elements->full;
        }
    }
    
    // Print result
    for (int i = 0; i < map->height; i++) {
        printf("%s\n", map->grid[i]);
    }
}
```

---

## Helper Functions

### Substring
```c
char* ft_substr(char* arr, int start, int len) {
    char* str = malloc(len + 1);
    if (!str) return NULL;
    
    int i = 0, j = 0;
    while (arr[i]) {
        if (i >= start && j < len) {
            str[j++] = arr[i];
        }
        i++;
    }
    str[j] = '\0';
    return str;
}
```

### Find Minimum
```c
int find_min(int a, int b, int c) {
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}
```

### Free Map
```c
void free_map(char** grid) {
    if (!grid) return;
    
    for (int i = 0; grid[i]; i++)
        free(grid[i]);
    free(grid);
}
```

---

## Main Flow

```c
int execute_bsq(FILE* file) {
    t_elements elements;
    if (loadElements(file, &elements) == -1)
        return -1;
    
    t_map map;
    if (loadMap(file, &map, &elements) == -1)
        return -1;
    
    t_square square = {0, 0, 0};
    find_big_square(&map, &square, &elements);
    print_filled_square(&map, &square, &elements);
    
    free_map(map.grid);
    return 0;
}

int main(int argc, char* argv[]) {
    if (argc == 1) {
        if (execute_bsq(stdin) == -1)
            fprintf(stderr, "map error\n");
    } else {
        for (int i = 1; i < argc; i++) {
            FILE* file = fopen(argv[i], "r");
            if (!file || execute_bsq(file) == -1)
                fprintf(stderr, "map error\n");
            if (file) fclose(file);
            if (i < argc - 1)
                printf("\n");
        }
    }
    return 0;
}
```

---

## Validation Checklist

### Header
- [ ] fscanf returns 4
- [ ] n_lines > 0
- [ ] All 3 chars unique
- [ ] All chars printable (32-126)

### Map
- [ ] All lines same width
- [ ] Width > 0
- [ ] Lines end with newline
- [ ] Only empty/obstacle chars
- [ ] Exactly n_lines lines

---

## Common Mistakes

❌ **Forgetting +1 in top-left calculation**
```c
top_left = bottom_right - size;  // WRONG
top_left = bottom_right - size + 1;  // CORRECT
```

❌ **Not freeing on error paths**
```c
if (error) return -1;  // MEMORY LEAK!

if (error) {
    free(line);
    free_map(grid);
    return -1;  // CORRECT
}
```

❌ **Including newline in grid**
```c
grid[i] = strdup(line);  // WRONG (has \n)

if (line[read-1] == '\n') read--;
grid[i] = ft_substr(line, 0, read);  // CORRECT
```

❌ **Not initializing DP matrix**
```c
int dp[h][w];  // Uninitialized!

int dp[h][w];
for (int i = 0; i < h; i++)
    for (int j = 0; j < w; j++)
        dp[i][j] = 0;  // CORRECT
```

---

## Quick Examples

### Example 1
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

### Example 2
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

### Example 3 - Error
```
Input:
3 . . x
...

Output to stderr:
map error
```

---

## Testing

```bash
# From stdin
cat map.txt | ./bsq

# From file
./bsq map.txt

# Multiple files
./bsq map1.txt map2.txt

# Check leaks
valgrind ./bsq map.txt
```

---

## Algorithm Visualization

```
Map:        DP Matrix:    Result:
. . . .     1 1 1 1       x x x x
. . . .     1 2 2 2       x x x x
. . . .     1 2 3 3       x x x x
. . . .     1 2 3 4       x x x x
```

DP value at (3,3) = 4 means:
- Size: 4×4 square
- Bottom-right: (3, 3)
- Top-left: (3-4+1, 3-4+1) = (0, 0)

---

## Key Points

✅ **Use DP** for O(n) solution  
✅ **Validate thoroughly** (most code is validation)  
✅ **Free on all error paths**  
✅ **Handle stdin and files**  
✅ **Print errors to stderr**  
✅ **Separate multiple outputs with newline**  
✅ **Prefer top-left square** (use `>` not `>=`)

---

## Time & Space

- **Time:** O(height × width)
- **Space:** O(height × width)

---

## Error Output

All errors go to **stderr**:
```c
fprintf(stderr, "map error\n");
```

---

## File I/O Functions

```c
// Read formatted input
fscanf(file, "%d %c %c %c", &n, &c1, &c2, &c3);

// Read line (dynamic allocation)
getline(&line, &len, file);

// Open file
FILE* file = fopen("file.txt", "r");

// Close file
fclose(file);
```

---

## Memory Functions

```c
// Allocate
char** grid = malloc(n * sizeof(char*));
char* str = malloc(len + 1);

// Free
free(ptr);

// Always check malloc return
if (!ptr) return -1;
```

---

## Quick Implementation Steps

1. Parse header with fscanf
2. Validate header values
3. Allocate grid
4. Read lines with getline
5. Remove newlines from lines
6. Validate consistent width
7. Build DP matrix
8. Track maximum square
9. Fill square in grid
10. Print result
11. Free memory

---

**Remember:** The DP algorithm is the key to solving this efficiently!

🎯 **Good luck!**
