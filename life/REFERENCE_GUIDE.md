# Game of Life - Reference Guide

Complete implementation reference for Conway's Game of Life with drawing commands.

---

## Program Structure

```
main.c        - Complete implementation
```

---

## Headers Required

```c
#include <unistd.h>   // read()
#include <stdlib.h>   // malloc, calloc, realloc, free, atoi
```

---

## Global Variables (Optional)

```c
int width;        // Grid width
int height;       // Grid height
int iterations;   // Number of generations to simulate
```

**Note:** Can also pass as parameters instead of globals.

---

## Function Reference

### 1. allocate_grid()

```c
int **allocate_grid(int height, int width)
{
    int **grid = malloc(sizeof(int *) * height);
    for (int i = 0; i < height; i++) {
        grid[i] = calloc(width, sizeof(int));
    }
    return grid;
}
```

**Purpose:** Allocate and initialize a 2D grid  
**Parameters:**
- `height`: Number of rows
- `width`: Number of columns  
**Returns:** Pointer to 2D array, all cells initialized to 0  
**Memory:** Caller must free with `free_grid()`

---

### 2. free_grid()

```c
void free_grid(int **grid, int height)
{
    for (int i = 0; i < height; i++) {
        free(grid[i]);
    }
    free(grid);
}
```

**Purpose:** Free a 2D grid  
**Parameters:**
- `grid`: The grid to free
- `height`: Number of rows  
**Important:** Must free each row before freeing the array of pointers

---

### 3. read_commands()

```c
void read_commands(int **grid, int height, int width)
{
    int x = 0, y = 0;
    int pen = 0;
    char c;
    
    while (read(0, &c, 1) > 0) {
        if (c == 'w' && y > 0)
            y--;
        else if (c == 's' && y < height - 1)
            y++;
        else if (c == 'd' && x < width - 1)
            x++;
        else if (c == 'a' && x > 0)
            x--;
        else if (c == 'x')
            pen = !pen;
        
        if (pen)
            grid[y][x] = 1;
    }
}
```

**Purpose:** Read drawing commands from stdin and populate grid  
**Commands:**
- `w`: Move up (y--)
- `a`: Move left (x--)
- `s`: Move down (y++)
- `d`: Move right (x++)
- `x`: Toggle pen (draw/don't draw)

**Initial State:**
- Position: (0, 0) - top-left
- Pen: lifted (0)

**Drawing:** When pen is down (1), mark current cell as alive

**Boundary Checking:** Prevents movement outside grid

---

### 4. is_alive()

```c
int is_alive(int **grid, int height, int width, int y, int x)
{
    if (y < 0 || y >= height || x < 0 || x >= width)
        return 0;
    return grid[y][x];
}
```

**Purpose:** Safely check if a cell is alive, treating out-of-bounds as dead  
**Parameters:**
- `grid`: The grid
- `height`, `width`: Grid dimensions
- `y`, `x`: Cell coordinates  
**Returns:** 1 if alive, 0 if dead or out of bounds

---

### 5. count_neighbors()

```c
int count_neighbors(int **grid, int height, int width, int y, int x)
{
    int count = 0;
    
    count += is_alive(grid, height, width, y-1, x-1);  // Top-left
    count += is_alive(grid, height, width, y-1, x);    // Top
    count += is_alive(grid, height, width, y-1, x+1);  // Top-right
    count += is_alive(grid, height, width, y, x-1);    // Left
    count += is_alive(grid, height, width, y, x+1);    // Right
    count += is_alive(grid, height, width, y+1, x-1);  // Bottom-left
    count += is_alive(grid, height, width, y+1, x);    // Bottom
    count += is_alive(grid, height, width, y+1, x+1);  // Bottom-right
    
    return count;
}
```

**Purpose:** Count living neighbors of a cell  
**Parameters:**
- `grid`: The grid
- `height`, `width`: Grid dimensions
- `y`, `x`: Cell coordinates  
**Returns:** Number of living neighbors (0-8)

**Neighbors:** All 8 cells surrounding the given cell  
**Edge handling:** Cells outside grid count as dead (via `is_alive()`)

---

### 6. apply_rules()

```c
void apply_rules(int **current, int **next, int height, int width)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int neighbors = count_neighbors(current, height, width, y, x);
            
            if (current[y][x] == 1) {
                // Cell is alive
                if (neighbors == 2 || neighbors == 3)
                    next[y][x] = 1;  // Survives
                else
                    next[y][x] = 0;  // Dies
            } else {
                // Cell is dead
                if (neighbors == 3)
                    next[y][x] = 1;  // Birth
                else
                    next[y][x] = 0;  // Stays dead
            }
        }
    }
}
```

**Purpose:** Apply Game of Life rules to generate next generation  
**Parameters:**
- `current`: Current generation grid
- `next`: Next generation grid (output)
- `height`, `width`: Grid dimensions

**Rules:**
1. Living cell with 2-3 neighbors → survives
2. Living cell with < 2 or > 3 neighbors → dies
3. Dead cell with exactly 3 neighbors → becomes alive
4. Dead cell otherwise → stays dead

**Critical:** Never modifies `current` grid, only writes to `next`

---

### 7. copy_grid()

```c
void copy_grid(int **dest, int **src, int height, int width)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            dest[y][x] = src[y][x];
        }
    }
}
```

**Purpose:** Copy one grid to another  
**Parameters:**
- `dest`: Destination grid
- `src`: Source grid
- `height`, `width`: Grid dimensions

**Used for:** Finalizing result after odd number of iterations with swapping

---

### 8. simulate()

```c
void simulate(int **grid, int **temp, int height, int width, int iterations)
{
    for (int i = 0; i < iterations; i++) {
        apply_rules(grid, temp, height, width);
        
        // Swap grids
        int **swap = grid;
        grid = temp;
        temp = swap;
    }
}
```

**Purpose:** Run Game of Life simulation for N iterations  
**Parameters:**
- `grid`: Initial grid (will be swapped)
- `temp`: Temporary grid for double buffering
- `height`, `width`: Grid dimensions
- `iterations`: Number of generations to simulate

**Algorithm:**
1. Apply rules to generate next generation in `temp`
2. Swap `grid` and `temp` pointers
3. Repeat

**Important:** After function, result may be in either `grid` or `temp` depending on iterations

**Alternative Implementation:**
```c
int **simulate(int **grid, int height, int width, int iterations)
{
    int **temp = allocate_grid(height, width);
    int **current = grid;
    
    for (int i = 0; i < iterations; i++) {
        apply_rules(current, temp, height, width);
        int **swap = current;
        current = temp;
        temp = swap;
    }
    
    // Copy result back to grid if needed
    if (iterations % 2 == 1) {
        copy_grid(grid, current, height, width);
        free_grid(temp, height);
        return grid;
    }
    
    free_grid(temp, height);
    return current;
}
```

---

### 9. print_grid()

```c
void print_grid(int **grid, int height, int width)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x])
                putchar('O');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}
```

**Purpose:** Print the grid to stdout  
**Output:**
- Alive cell: `O` (capital letter O)
- Dead cell: ` ` (space)
- Row separator: `\n`

**Format:** Exactly as specified - no extra formatting

---

### 10. main()

```c
int main(int argc, char **argv)
{
    if (argc != 4)
        return 1;
    
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int iterations = atoi(argv[3]);
    
    // Allocate grids
    int **grid = allocate_grid(height, width);
    int **temp = allocate_grid(height, width);
    
    // Phase 1: Drawing
    read_commands(grid, height, width);
    
    // Phase 2: Simulation
    for (int i = 0; i < iterations; i++) {
        apply_rules(grid, temp, height, width);
        int **swap = grid;
        grid = temp;
        temp = swap;
    }
    
    // Phase 3: Output
    print_grid(grid, height, width);
    
    // Cleanup
    free_grid(grid, height);
    free_grid(temp, height);
    
    return 0;
}
```

**Purpose:** Entry point  
**Arguments:**
1. `argv[1]`: width
2. `argv[2]`: height
3. `argv[3]`: iterations

**Flow:**
1. Validate arguments
2. Parse dimensions and iterations
3. Allocate grids
4. Read drawing commands
5. Simulate generations
6. Print final state
7. Free memory

---

## Complete Implementation Example

```c
#include <unistd.h>
#include <stdlib.h>

int **allocate_grid(int height, int width)
{
    int **grid = malloc(sizeof(int *) * height);
    for (int i = 0; i < height; i++)
        grid[i] = calloc(width, sizeof(int));
    return grid;
}

void free_grid(int **grid, int height)
{
    for (int i = 0; i < height; i++)
        free(grid[i]);
    free(grid);
}

void read_commands(int **grid, int height, int width)
{
    int x = 0, y = 0;
    int pen = 0;
    char c;
    
    while (read(0, &c, 1) > 0) {
        if (c == 'w' && y > 0)          y--;
        else if (c == 's' && y < height - 1) y++;
        else if (c == 'd' && x < width - 1)  x++;
        else if (c == 'a' && x > 0)          x--;
        else if (c == 'x')              pen = !pen;
        
        if (pen)
            grid[y][x] = 1;
    }
}

int is_alive(int **grid, int height, int width, int y, int x)
{
    if (y < 0 || y >= height || x < 0 || x >= width)
        return 0;
    return grid[y][x];
}

int count_neighbors(int **grid, int height, int width, int y, int x)
{
    int count = 0;
    count += is_alive(grid, height, width, y-1, x-1);
    count += is_alive(grid, height, width, y-1, x);
    count += is_alive(grid, height, width, y-1, x+1);
    count += is_alive(grid, height, width, y, x-1);
    count += is_alive(grid, height, width, y, x+1);
    count += is_alive(grid, height, width, y+1, x-1);
    count += is_alive(grid, height, width, y+1, x);
    count += is_alive(grid, height, width, y+1, x+1);
    return count;
}

void apply_rules(int **current, int **next, int height, int width)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int neighbors = count_neighbors(current, height, width, y, x);
            
            if (current[y][x] == 1) {
                if (neighbors == 2 || neighbors == 3)
                    next[y][x] = 1;
                else
                    next[y][x] = 0;
            } else {
                if (neighbors == 3)
                    next[y][x] = 1;
                else
                    next[y][x] = 0;
            }
        }
    }
}

void print_grid(int **grid, int height, int width)
{
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            if (grid[y][x])
                putchar('O');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
        return 1;
    
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int iterations = atoi(argv[3]);
    
    int **grid = allocate_grid(height, width);
    int **temp = allocate_grid(height, width);
    
    read_commands(grid, height, width);
    
    for (int i = 0; i < iterations; i++) {
        apply_rules(grid, temp, height, width);
        int **swap = grid;
        grid = temp;
        temp = swap;
    }
    
    print_grid(grid, height, width);
    
    free_grid(grid, height);
    free_grid(temp, height);
    
    return 0;
}
```

---

## Game of Life Rules Reference

| Current State | Neighbors | Next State | Rule Name |
|---------------|-----------|------------|-----------|
| Alive | 0-1 | Dead | Underpopulation |
| Alive | 2-3 | Alive | Survival |
| Alive | 4-8 | Dead | Overpopulation |
| Dead | 0-2 | Dead | Stays dead |
| Dead | 3 | Alive | Reproduction |
| Dead | 4-8 | Dead | Stays dead |

---

## Drawing Commands Reference

| Command | Action | Effect on (x, y) | Pen State |
|---------|--------|------------------|-----------|
| `w` | Move up | (x, y-1) | Unchanged |
| `a` | Move left | (x-1, y) | Unchanged |
| `s` | Move down | (x, y+1) | Unchanged |
| `d` | Move right | (x+1, y) | Unchanged |
| `x` | Toggle pen | Unchanged | Flips |

**Drawing:** If pen is down after processing command, mark current cell as alive.

---

## Examples Walkthrough

### Example 1: Vertical Line
```bash
echo 'dxss' | ./life 3 3 0
```

**Trace:**
```
Initial: (0,0), pen=0

d: move to (1,0), pen=0
x: toggle pen, pen=1, mark (1,0) as alive
s: move to (1,1), pen=1, mark (1,1) as alive
s: move to (1,2), pen=1, mark (1,2) as alive
```

**Result:**
```
 O 
 O 
 O 
```

### Example 2: Blinker Oscillator
```bash
echo 'dxss' | ./life 3 3 1
```

**Generation 0:**
```
 O     Neighbors:
 O  →  0 2 0
 O     0 2 0
       0 2 0
```

**Generation 1:**
```
       Neighbors:
OOO →  1 1 1
       3 2 3
       1 1 1
```

**Rule Application:**
- (1,0): alive, 2 neighbors → survives
- (1,1): alive, 2 neighbors → survives
- (1,2): alive, 2 neighbors → survives
- (0,1): dead, 3 neighbors → becomes alive
- (2,1): dead, 3 neighbors → becomes alive

---

## Memory Management

### Allocation
```c
// Allocate rows
grid = malloc(sizeof(int *) * height);

// Allocate columns for each row
for (int i = 0; i < height; i++)
    grid[i] = calloc(width, sizeof(int));
```

### Deallocation
```c
// Free each row
for (int i = 0; i < height; i++)
    free(grid[i]);

// Free row array
free(grid);
```

**Important:** Must free in reverse order - rows first, then row array.

---

## Performance

### Time Complexity
- **Drawing:** O(C) where C = number of commands
- **One iteration:** O(H × W) where H, W = grid dimensions
- **N iterations:** O(N × H × W)
- **Total:** O(C + N × H × W)

### Space Complexity
- **Two grids:** 2 × H × W integers
- **Total:** O(H × W)

---

## Common Patterns

### Pattern 1: Grid Swapping
```c
for (int i = 0; i < iterations; i++) {
    apply_rules(current, next, h, w);
    int **tmp = current;
    current = next;
    next = tmp;
}
```

### Pattern 2: Safe Boundary Check
```c
if (y < 0 || y >= height || x < 0 || x >= width)
    return 0;  // Out of bounds
```

### Pattern 3: Pen Toggle
```c
if (command == 'x')
    pen = !pen;  // Flip between 0 and 1
```

---

## Debugging Tips

### Print Each Generation
```c
for (int i = 0; i <= iterations; i++) {
    printf("Gen %d:\n", i);
    print_grid(grid, height, width);
    printf("\n");
    if (i < iterations)
        apply_rules(grid, temp, height, width);
}
```

### Print Neighbor Counts
```c
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        printf("%d ", count_neighbors(grid, height, width, y, x));
    }
    printf("\n");
}
```

### Trace Drawing
```c
printf("Cmd: %c, Pos: (%d,%d), Pen: %d\n", cmd, x, y, pen);
```

---

## Testing Commands

```bash
# Compile
gcc -Wall -Wextra -Werror main.c -o life

# Test 1: Simple line
echo 'dxss' | ./life 3 3 0 | cat -e

# Test 2: Blinker
echo 'dxss' | ./life 3 3 1 | cat -e

# Test 3: Rectangle
echo 'sdxddssaaww' | ./life 5 5 0 | cat -e

# Test 4: Complex pattern
echo 'sdxssdswdxddddsxaadwxwdxwaa' | ./life 10 6 0 | cat -e
```

---

*This reference provides complete implementation details for the Game of Life project.*
