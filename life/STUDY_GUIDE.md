# Game of Life - Study Guide

## Overview
This exercise implements **Conway's Game of Life**, a cellular automaton that simulates the evolution of a grid of cells based on simple rules. You'll combine drawing commands with simulation logic.

---

## Table of Contents
1. [Problem Statement](#problem-statement)
2. [Core Concepts](#core-concepts)
3. [Drawing System](#drawing-system)
4. [Game of Life Rules](#game-of-life-rules)
5. [Implementation Strategy](#implementation-strategy)
6. [Algorithm Details](#algorithm-details)
7. [Common Pitfalls](#common-pitfalls)
8. [Testing Approach](#testing-approach)

---

## Problem Statement

### Goal
1. Parse command-line arguments (width, height, iterations)
2. Read drawing commands from stdin to create initial board state
3. Simulate Conway's Game of Life for N iterations
4. Print the final board state

### Input Format
```bash
./life <width> <height> <iterations>
```

Drawing commands from stdin:
- `w` - move pen up
- `a` - move pen left
- `s` - move pen down
- `d` - move pen right
- `x` - toggle pen (lift/lower to start/stop drawing)

### Output
- Living cells: `O` (capital letter O)
- Dead cells: ` ` (space)
- Each row ends with newline

---

## Core Concepts

### 1. Conway's Game of Life

**What is it?**
A cellular automaton where cells evolve based on their neighbors.

**Rules:**
1. **Survival:** A living cell with 2 or 3 neighbors stays alive
2. **Death by isolation:** A living cell with < 2 neighbors dies
3. **Death by overcrowding:** A living cell with > 3 neighbors dies
4. **Birth:** A dead cell with exactly 3 neighbors becomes alive

**Visual Example:**
```
Generation 0:        Generation 1:
  O                      
  O          →         OOO
  O                      
```

### 2. The Drawing System

**Concept:** A "pen" that moves on the board and draws when lowered.

**Initial State:**
- Pen position: (0, 0) - top-left corner
- Pen status: lifted (not drawing)

**Commands:**
- `w`, `a`, `s`, `d` - move the pen
- `x` - toggle pen (if lifted → lowered, if lowered → lifted)
- When pen is lowered, mark current cell as alive

---

## Drawing System

### Coordinate System
```
(0,0) (1,0) (2,0) (3,0) (4,0)
(0,1) (1,1) (2,1) (3,1) (4,1)
(0,2) (1,2) (2,2) (3,2) (4,2)
  ↑     ↑
  x     y (note: in 2D array, typically [y][x])
```

### Movement Commands

| Command | Effect | Movement |
|---------|--------|----------|
| `w` | Up | y-- |
| `a` | Left | x-- |
| `s` | Down | y++ |
| `d` | Right | x++ |

### Drawing Logic

```c
int pen = 0;  // 0 = lifted, 1 = lowered
int x = 0, y = 0;

if (command == 'x')
    pen = !pen;  // Toggle

if (pen)
    grid[y][x] = 1;  // Mark as alive when pen is down
```

### Example: `sdxddssaaww`

```
Start: (0,0), pen up

s → move down to (0,1)
d → move right to (1,1)
x → pen down (start drawing)
d → move to (2,1), draw
d → move to (3,1), draw
s → move to (3,2), draw
s → move to (3,3), draw
a → move to (2,3), draw
a → move to (1,3), draw
w → move to (1,2), draw
w → move to (1,1), draw

Result:
     
 OOO
 O O
 OOO
```

---

## Game of Life Rules

### The Four Rules

#### Rule 1: Underpopulation
```
Any live cell with < 2 neighbors dies
```
**Example:**
```
Before:      After:
   O         
   O    →    (all die)
```

#### Rule 2: Survival
```
Any live cell with 2 or 3 neighbors survives
```
**Example:**
```
Before:      After:
 OO          OO
 OO     →    OO
(each has 3 neighbors, all survive)
```

#### Rule 3: Overpopulation
```
Any live cell with > 3 neighbors dies
```
**Example:**
```
Before:      After:
 OOO         O O
 OOO    →     O
 OOO         O O
(center has 8 neighbors, dies)
```

#### Rule 4: Reproduction
```
Any dead cell with exactly 3 neighbors becomes alive
```
**Example:**
```
Before:      After:
 O           
 O      →    OOO
 O           
(middle cell has 3 neighbors, becomes alive)
```

### Counting Neighbors

For a cell at (y, x), check all 8 surrounding cells:
```
[y-1][x-1]  [y-1][x]  [y-1][x+1]
[y][x-1]    [CELL]    [y][x+1]
[y+1][x-1]  [y+1][x]  [y+1][x+1]
```

**Important:** Cells outside the grid are considered dead (0 neighbors).

---

## Implementation Strategy

### Data Structures

#### Board Representation
```c
int **grid;  // 2D array: grid[height][width]
// 1 = alive, 0 = dead
```

#### Pen State
```c
int x = 0, y = 0;    // Position
int pen_down = 0;     // 0 = up, 1 = down
```

### Function Architecture

```
main()
  ├─ parse_arguments()
  ├─ allocate_grid()
  ├─ read_commands()     // Drawing phase
  ├─ simulate()          // Game of Life iterations
  │   └─ apply_rules()
  └─ print_grid()
```

---

## Algorithm Details

### Step 1: Parse Arguments

```c
int main(int argc, char **argv) {
    if (argc != 4)
        return 1;
    
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);
    int iterations = atoi(argv[3]);
    
    // Validate
    if (width <= 0 || height <= 0 || iterations < 0)
        return 1;
}
```

### Step 2: Allocate Grid

```c
int **allocate_grid(int height, int width) {
    int **grid = malloc(sizeof(int *) * height);
    for (int i = 0; i < height; i++) {
        grid[i] = calloc(width, sizeof(int));  // calloc initializes to 0
    }
    return grid;
}
```

**Why calloc?** Automatically initializes all cells to 0 (dead).

### Step 3: Read Drawing Commands

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

**Key Points:**
- Boundary checking prevents out-of-bounds access
- `read(0, &c, 1)` reads one character at a time from stdin
- Pen toggle happens before drawing check

### Step 4: Count Neighbors

```c
int is_alive(int **grid, int height, int width, int y, int x) {
    if (y < 0 || y >= height || x < 0 || x >= width)
        return 0;  // Outside grid = dead
    return grid[y][x];
}

int count_neighbors(int **grid, int height, int width, int y, int x) {
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

### Step 5: Apply Game of Life Rules

```c
void apply_rules(int **current, int **next, int height, int width) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int neighbors = count_neighbors(current, height, width, y, x);
            
            if (current[y][x] == 1) {  // Cell is alive
                if (neighbors == 2 || neighbors == 3)
                    next[y][x] = 1;  // Survives
                else
                    next[y][x] = 0;  // Dies
            } else {  // Cell is dead
                if (neighbors == 3)
                    next[y][x] = 1;  // Becomes alive
                else
                    next[y][x] = 0;  // Stays dead
            }
        }
    }
}
```

**Critical:** Don't modify the current grid during iteration! Use a temporary grid.

### Step 6: Simulate Iterations

```c
void simulate(int **grid, int height, int width, int iterations) {
    int **temp = allocate_grid(height, width);
    
    for (int i = 0; i < iterations; i++) {
        apply_rules(grid, temp, height, width);
        
        // Swap grids
        int **swap = grid;
        grid = temp;
        temp = swap;
    }
    
    // If odd iterations, result is in temp, need to copy back
    if (iterations % 2 == 1) {
        copy_grid(grid, temp, height, width);
    }
    
    free_grid(temp, height);
}
```

**Alternative:** Return the final grid pointer and track which is which.

### Step 7: Print Grid

```c
void print_grid(int **grid, int height, int width) {
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

---

## Common Pitfalls

### 1. Boundary Checking

❌ **Wrong:**
```c
if (c == 'w')
    y--;  // Can go negative!
```

✅ **Right:**
```c
if (c == 'w' && y > 0)
    y--;
```

### 2. Modifying Grid During Iteration

❌ **Wrong:**
```c
for (y = 0; y < height; y++) {
    for (x = 0; x < width; x++) {
        int n = count_neighbors(grid, height, width, y, x);
        grid[y][x] = apply_rule(grid[y][x], n);  // WRONG!
    }
}
```

**Problem:** Changes affect subsequent neighbor counts in the same iteration.

✅ **Right:**
```c
// Use two grids
apply_rules(current_grid, next_grid, height, width);
swap(current_grid, next_grid);
```

### 3. Drawing Before Pen Down

❌ **Wrong:**
```c
if (pen)
    grid[y][x] = 1;
if (c == 'x')
    pen = !pen;  // Toggle happens AFTER drawing check
```

✅ **Right:**
```c
if (c == 'x')
    pen = !pen;  // Toggle FIRST
if (pen)
    grid[y][x] = 1;  // Then draw
```

### 4. Off-by-One in Boundaries

❌ **Wrong:**
```c
if (y < height)  // Can access grid[height]!
    y++;
```

✅ **Right:**
```c
if (y < height - 1)  // Stops at height - 1
    y++;
```

### 5. Memory Leaks

❌ **Wrong:**
```c
int **grid = allocate_grid(height, width);
// ... use grid ...
return 0;  // Never freed!
```

✅ **Right:**
```c
int **grid = allocate_grid(height, width);
// ... use grid ...
free_grid(grid, height);
return 0;
```

### 6. Pen Drawing at Start Position

**Question:** Should the pen draw at (0, 0) when first lowered?

**Answer:** Yes! When pen is lowered, it draws at current position.

```c
// At (0,0)
if (c == 'x')
    pen = 1;
if (pen)
    grid[0][0] = 1;  // Should mark (0,0) as alive
```

---

## Testing Approach

### Test Cases

#### Test 1: Simple Drawing
```bash
echo 'dxss' | ./life 3 3 0
```
**Expected:**
```
O  
O  
O  
```

**Trace:**
- Start: (0,0), pen up
- `d`: move to (1,0)
- `x`: pen down
- `s`: move to (1,1), draw
- `s`: move to (1,2), draw

#### Test 2: Rectangle
```bash
echo 'sdxddssaaww' | ./life 5 5 0
```
**Expected:**
```
     
 OOO 
 O O 
 OOO 
     
```

#### Test 3: Oscillator (Blinker)
```bash
echo 'dxss' | ./life 3 3 1
```

**Generation 0:**
```
 O 
 O 
 O 
```

**Generation 1:**
```
   
OOO
   
```

**Why?** Each cell in middle row has 2 neighbors, so they survive. Cells at (0,1) and (2,1) each have 3 neighbors, so they become alive.

#### Test 4: Still Life (Block)
```bash
echo 'xddss' | ./life 4 4 5
```

**All Generations:**
```
OO  
OO  
    
    
```

Block is stable - each cell has 3 neighbors, survives forever.

#### Test 5: Zero Iterations
```bash
echo 'x' | ./life 1 1 0
```
**Expected:**
```
O
```
Just draws, no simulation.

#### Test 6: Empty Grid
```bash
echo '' | ./life 3 3 5
```
**Expected:**
```
   
   
   
```
No drawing, grid stays empty.

---

## Edge Cases

### 1. Pen Starts Lowered
```bash
echo 'xss' | ./life 2 2 0
```
Starting position (0,0) gets marked before moving.

### 2. Out-of-Bounds Movement
```bash
echo 'wwwwaaaaa' | ./life 2 2 0
```
Multiple movements beyond boundaries should be ignored.

### 3. Large Iterations
```bash
echo 'xss' | ./life 3 3 100
```
Should handle many iterations without issues.

### 4. Single Cell
```bash
echo 'x' | ./life 1 1 1
```
**Gen 0:** O (1 cell, alive)  
**Gen 1:** ` ` (0 neighbors, dies)

---

## Performance Considerations

### Time Complexity
- **Drawing:** O(C) where C = number of commands
- **One iteration:** O(H × W) where H = height, W = width
- **Total simulation:** O(I × H × W) where I = iterations
- **Overall:** O(C + I × H × W)

### Space Complexity
- **Grids:** 2 × (H × W) integers
- **Overall:** O(H × W)

### Optimization Tips
1. Use `calloc` to initialize grid to 0
2. Reuse two grids instead of allocating each iteration
3. Read commands in chunks if performance critical
4. Consider using bit arrays for large grids

---

## Debugging Strategies

### 1. Visualize Each Generation
```c
for (int i = 0; i <= iterations; i++) {
    printf("=== Generation %d ===\n", i);
    print_grid(grid, height, width);
    if (i < iterations)
        apply_rules(grid, temp, height, width);
}
```

### 2. Print Neighbor Counts
```c
for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
        int n = count_neighbors(grid, height, width, y, x);
        printf("%d ", n);
    }
    printf("\n");
}
```

### 3. Trace Drawing Commands
```c
printf("Command: %c, Pen: %d, Pos: (%d,%d)\n", c, pen, x, y);
```

### 4. Verify Grid Swap
```c
printf("Before swap: grid=%p, temp=%p\n", grid, temp);
// swap
printf("After swap: grid=%p, temp=%p\n", grid, temp);
```

---

## Key Takeaways

1. **Two-phase problem:** Drawing + Simulation
2. **Drawing system:** Pen moves with commands, draws when lowered
3. **Game of Life:** Four simple rules govern cell evolution
4. **Double buffering:** Use two grids to avoid modifying during iteration
5. **Boundary checking:** Critical for both drawing and neighbor counting
6. **Memory management:** Allocate and free both grids properly

---

## Study Checklist

- [ ] Understand Game of Life rules
- [ ] Know how the drawing system works
- [ ] Implement boundary-safe movement
- [ ] Count neighbors correctly (8 directions)
- [ ] Apply rules without modifying current grid
- [ ] Handle grid swapping properly
- [ ] Print output correctly (O for alive, space for dead)
- [ ] Test with provided examples
- [ ] Verify no memory leaks

---

## Additional Practice

1. Implement different initial patterns (glider, spaceship)
2. Add visualization with colors or different characters
3. Implement a "wrap-around" toroidal grid
4. Count total alive cells per generation
5. Detect stable states or cycles

Good luck with your Game of Life implementation! 🎮
