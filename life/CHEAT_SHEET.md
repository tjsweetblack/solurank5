# Game of Life - Cheat Sheet

Quick reference for implementing Conway's Game of Life with drawing commands.

---

## Program Structure

```c
#include <unistd.h>   // read()
#include <stdlib.h>   // malloc, calloc, free, atoi

int main(int argc, char **argv);
```

---

## Essential Functions

### Allocate Grid
```c
int **allocate_grid(int height, int width) {
    int **grid = malloc(sizeof(int *) * height);
    for (int i = 0; i < height; i++)
        grid[i] = calloc(width, sizeof(int));  // calloc = init to 0
    return grid;
}
```

### Free Grid
```c
void free_grid(int **grid, int height) {
    for (int i = 0; i < height; i++)
        free(grid[i]);
    free(grid);
}
```

### Read Drawing Commands
```c
void read_commands(int **grid, int height, int width) {
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
```

### Check if Cell is Alive
```c
int is_alive(int **grid, int height, int width, int y, int x) {
    if (y < 0 || y >= height || x < 0 || x >= width)
        return 0;
    return grid[y][x];
}
```

### Count Neighbors
```c
int count_neighbors(int **grid, int height, int width, int y, int x) {
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
```

### Apply Game of Life Rules
```c
void apply_rules(int **current, int **next, int height, int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int n = count_neighbors(current, height, width, y, x);
            
            if (current[y][x] == 1) {  // Alive
                next[y][x] = (n == 2 || n == 3) ? 1 : 0;
            } else {  // Dead
                next[y][x] = (n == 3) ? 1 : 0;
            }
        }
    }
}
```

### Print Grid
```c
void print_grid(int **grid, int height, int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            putchar(grid[y][x] ? 'O' : ' ');
        }
        putchar('\n');
    }
}
```

### Main Function
```c
int main(int argc, char **argv) {
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

## Game of Life Rules

| State | Neighbors | Next State |
|-------|-----------|------------|
| Alive | 0-1 | Dead (underpopulation) |
| Alive | 2-3 | Alive (survival) |
| Alive | 4-8 | Dead (overpopulation) |
| Dead | 3 | Alive (reproduction) |
| Dead | other | Dead |

**In code:**
```c
if (alive) {
    next = (n == 2 || n == 3) ? 1 : 0;
} else {
    next = (n == 3) ? 1 : 0;
}
```

---

## Drawing Commands

| Command | Action | Movement |
|---------|--------|----------|
| `w` | Up | y-- |
| `s` | Down | y++ |
| `a` | Left | x-- |
| `d` | Right | x++ |
| `x` | Toggle pen | pen = !pen |

**Start:** Position (0,0), pen lifted (0)  
**Drawing:** When pen is down (1), mark cell as alive

---

## Neighbor Positions

```
For cell at (y, x):

(y-1,x-1)  (y-1,x)  (y-1,x+1)
(y,x-1)    [CELL]   (y,x+1)
(y+1,x-1)  (y+1,x)  (y+1,x+1)

All 8 surrounding cells
```

---

## Common Patterns

### Grid Swapping
```c
for (int i = 0; i < iterations; i++) {
    apply_rules(grid, temp, height, width);
    int **swap = grid;
    grid = temp;
    temp = swap;
}
```

### Boundary Check
```c
if (command == 'w' && y > 0)
    y--;
```

### Pen Toggle
```c
if (command == 'x')
    pen = !pen;
```

---

## Common Mistakes

❌ **Modifying grid while iterating**
```c
grid[y][x] = new_value;  // WRONG - affects neighbor counts
```

✅ **Use double buffering**
```c
apply_rules(current, next, h, w);
swap(current, next);
```

❌ **No boundary check**
```c
if (c == 'w')
    y--;  // Can go negative!
```

✅ **Check boundaries**
```c
if (c == 'w' && y > 0)
    y--;
```

❌ **Drawing before toggling pen**
```c
if (pen) grid[y][x] = 1;
if (c == 'x') pen = !pen;  // Too late!
```

✅ **Toggle first, then draw**
```c
if (c == 'x') pen = !pen;
if (pen) grid[y][x] = 1;
```

❌ **Off-by-one in bounds**
```c
if (y < height)  // Can access grid[height]!
    y++;
```

✅ **Correct bounds**
```c
if (y < height - 1)
    y++;
```

---

## Testing Examples

### Test 1: Vertical Line
```bash
echo 'dxss' | ./life 3 3 0
```
**Output:**
```
 O 
 O 
 O 
```

### Test 2: Blinker (Oscillator)
```bash
echo 'dxss' | ./life 3 3 1
```
**Gen 0:** Vertical line  
**Gen 1:** Horizontal line
```
   
OOO
   
```

### Test 3: Rectangle
```bash
echo 'sdxddssaaww' | ./life 5 5 0
```
**Output:**
```
     
 OOO 
 O O 
 OOO 
     
```

### Test 4: Still Life (Block)
```bash
echo 'xddss' | ./life 4 4 5
```
**All generations:** 2×2 block (stable)
```
OO  
OO  
    
    
```

---

## Quick Algorithm

### Phase 1: Drawing
```
1. Start at (0,0), pen up
2. Read commands one by one
3. Process movement (w/a/s/d)
4. Process pen toggle (x)
5. If pen down, mark current cell
```

### Phase 2: Simulation
```
For each iteration:
    For each cell:
        Count neighbors
        Apply rules → write to temp grid
    Swap grid and temp
```

### Phase 3: Output
```
For each row:
    For each column:
        Print 'O' if alive, ' ' if dead
    Print newline
```

---

## Memory Pattern

### Allocate
```c
grid = malloc(sizeof(int *) * height);
for (i = 0; i < height; i++)
    grid[i] = calloc(width, sizeof(int));
```

### Free
```c
for (i = 0; i < height; i++)
    free(grid[i]);
free(grid);
```

---

## Time & Space

**Time:** O(C + I × H × W)
- C = commands
- I = iterations
- H = height, W = width

**Space:** O(H × W)
- Two grids needed

---

## Compilation & Running

```bash
# Compile
gcc -Wall -Wextra -Werror main.c -o life

# Run
echo 'commands' | ./life width height iterations

# Example
echo 'dxss' | ./life 3 3 1

# With cat -e to see line endings
echo 'dxss' | ./life 3 3 0 | cat -e
```

---

## Debugging Tips

### Print Each Generation
```c
printf("Gen %d:\n", i);
print_grid(grid, height, width);
```

### Print Neighbor Counts
```c
for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++)
        printf("%d ", count_neighbors(grid, h, w, y, x));
    printf("\n");
}
```

### Trace Commands
```c
printf("Cmd: %c, Pos: (%d,%d), Pen: %d\n", c, x, y, pen);
```

---

## Quick Rules Summary

**Alive cell:**
- 0-1 neighbors → dies
- 2-3 neighbors → survives
- 4+ neighbors → dies

**Dead cell:**
- 3 neighbors → becomes alive
- otherwise → stays dead

---

## Classic Patterns

### Blinker (Period 2)
```
Gen 0:    Gen 1:
 O           
 O     →   OOO
 O           
```

### Block (Still Life)
```
OO
OO
(Never changes)
```

### Glider (Moves)
```
Gen 0:    Gen 1:    Gen 2:
 O         O        
  O       OO        OO
OOO       O O       O 
                     O
```

---

## Key Points

✅ **Use calloc** - initializes to 0  
✅ **Double buffering** - never modify current grid  
✅ **Boundary checks** - for movement and neighbors  
✅ **Pen toggle** - process before drawing  
✅ **Swap grids** - don't copy every iteration  
✅ **Free memory** - both grids at end  

---

## Function Checklist

- [ ] `allocate_grid(height, width)`
- [ ] `free_grid(grid, height)`
- [ ] `read_commands(grid, h, w)`
- [ ] `is_alive(grid, h, w, y, x)`
- [ ] `count_neighbors(grid, h, w, y, x)`
- [ ] `apply_rules(current, next, h, w)`
- [ ] `print_grid(grid, h, w)`
- [ ] `main(argc, argv)`

---

## Complete Minimal Code

```c
#include <unistd.h>
#include <stdlib.h>

int **alloc(int h, int w) {
    int **g = malloc(sizeof(int *) * h);
    for (int i = 0; i < h; i++)
        g[i] = calloc(w, sizeof(int));
    return g;
}

void free_grid(int **g, int h) {
    for (int i = 0; i < h; i++) free(g[i]);
    free(g);
}

void read_cmds(int **g, int h, int w) {
    int x = 0, y = 0, pen = 0;
    char c;
    while (read(0, &c, 1) > 0) {
        if (c == 'w' && y > 0) y--;
        else if (c == 's' && y < h-1) y++;
        else if (c == 'd' && x < w-1) x++;
        else if (c == 'a' && x > 0) x--;
        else if (c == 'x') pen = !pen;
        if (pen) g[y][x] = 1;
    }
}

int alive(int **g, int h, int w, int y, int x) {
    if (y < 0 || y >= h || x < 0 || x >= w) return 0;
    return g[y][x];
}

int neighbors(int **g, int h, int w, int y, int x) {
    return alive(g,h,w,y-1,x-1) + alive(g,h,w,y-1,x) + alive(g,h,w,y-1,x+1) +
           alive(g,h,w,y,x-1) + alive(g,h,w,y,x+1) +
           alive(g,h,w,y+1,x-1) + alive(g,h,w,y+1,x) + alive(g,h,w,y+1,x+1);
}

void rules(int **c, int **n, int h, int w) {
    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++) {
            int nb = neighbors(c, h, w, y, x);
            n[y][x] = (c[y][x]) ? (nb == 2 || nb == 3) : (nb == 3);
        }
}

void print(int **g, int h, int w) {
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++)
            putchar(g[y][x] ? 'O' : ' ');
        putchar('\n');
    }
}

int main(int argc, char **argv) {
    if (argc != 4) return 1;
    int w = atoi(argv[1]), h = atoi(argv[2]), it = atoi(argv[3]);
    int **g = alloc(h, w), **t = alloc(h, w);
    read_cmds(g, h, w);
    for (int i = 0; i < it; i++) {
        rules(g, t, h, w);
        int **s = g; g = t; t = s;
    }
    print(g, h, w);
    free_grid(g, h); free_grid(t, h);
    return 0;
}
```

---

**Remember:** Drawing first, then simulation, then output!

🎮 **Good luck!**
