# BSQ Algorithm Visualization

Visual examples to help understand how the BSQ algorithm works.

---

## Example 1: Simple 4×4 Map (No Obstacles)

### Input Map
```
4 . o x
....
....
....
....
```

### Step-by-Step DP Matrix Building

**Initial State:**
```
DP Matrix (all zeros):
0 0 0 0
0 0 0 0
0 0 0 0
0 0 0 0
```

**After Processing Row 0 (first row = 1):**
```
DP Matrix:
1 1 1 1
0 0 0 0
0 0 0 0
0 0 0 0
```

**After Processing Row 1:**
```
DP Matrix:
1 1 1 1
1 2 2 2   ← dp[1][1] = min(dp[0][1], dp[1][0], dp[0][0]) + 1 = min(1,1,1) + 1 = 2
0 0 0 0
0 0 0 0
```

**After Processing Row 2:**
```
DP Matrix:
1 1 1 1
1 2 2 2
1 2 3 3   ← dp[2][2] = min(dp[1][2], dp[2][1], dp[1][1]) + 1 = min(2,2,2) + 1 = 3
0 0 0 0
```

**Final DP Matrix:**
```
DP Matrix:
1 1 1 1
1 2 2 2
1 2 3 3
1 2 3 4   ← dp[3][3] = min(dp[2][3], dp[3][2], dp[2][2]) + 1 = min(3,3,3) + 1 = 4
```

**Maximum:** dp[3][3] = 4 (a 4×4 square)

**Position Calculation:**
- Bottom-right: (3, 3)
- Top-left: (3 - 4 + 1, 3 - 4 + 1) = (0, 0)

### Output
```
xxxx
xxxx
xxxx
xxxx
```

---

## Example 2: Map with Obstacle

### Input Map
```
4 . o x
....
.o..
....
....
```

### DP Matrix Building

**Row 0 (first row = 1):**
```
Map:        DP:
. . . .     1 1 1 1
. o . .     0 0 0 0
. . . .     0 0 0 0
. . . .     0 0 0 0
```

**Row 1 (obstacle at [1][1]):**
```
Map:        DP:
. . . .     1 1 1 1
. o . .     1 0 1 1   ← obstacle = 0, then continues
. . . .     0 0 0 0
. . . .     0 0 0 0
```

**Row 2:**
```
Map:        DP:
. . . .     1 1 1 1
. o . .     1 0 1 1
. . . .     1 1 1 2   ← dp[2][3] = min(1,1,1) + 1 = 2
. . . .     0 0 0 0
```

**Final:**
```
Map:        DP:
. . . .     1 1 1 1
. o . .     1 0 1 1
. . . .     1 1 1 2
. . . .     1 1 1 2
```

**Maximum:** dp[2][3] or dp[3][3] = 2 (a 2×2 square)

**First occurrence:** (2, 3) with bottom-right at (3, 3)
- Top-left: (3 - 2 + 1, 3 - 2 + 1) = (2, 2)

### Output
```
....
.o..
..xx
..xx
```

---

## Example 3: The Given Example

### Input
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

### Visualization (showing key DP values)

Let's trace a specific region where the 7×7 square forms:

**Region around rows 0-6, columns 5-11:**
```
Map (portion):          DP Matrix (portion):
. . . . . . .           1 1 1 1 1 1 1
. . . . . . .           1 2 2 2 2 2 2
. . . o . . .           1 2 3 3 0 1 2
. . . . . . .           1 2 3 4 1 2 3
. . . . . . .           1 2 3 4 2 3 4
. . . . . . o           1 2 3 4 3 4 5
. . . . . . .           1 2 3 4 4 5 6
```

Actually, let me show the exact region where the 7×7 forms:

**Rows 0-6, columns 5-11:**
```
Position:  5 6 7 8 9 0 1
Row 0:     . . . . . . .
Row 1:     . . . . . . .
Row 2:     . . . o . . .  ← obstacle at column 8
Row 3:     . . . . . . .
Row 4:     . . . . . . .
Row 5:     . . . . . . o  ← obstacle at column 11
Row 6:     . . . . . . .
```

The DP algorithm finds that a 7×7 square can fit from rows 0-6, columns 5-11 (avoiding obstacles).

### Output (showing the filled square)
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

## Algorithm Step-by-Step

### For Each Cell (i, j):

```
┌─────────────────────────────┐
│ Is grid[i][j] an obstacle?  │
└──────────┬──────────────────┘
           │
    ┌──────┴──────┐
    │ YES         │ NO
    ▼             ▼
dp[i][j] = 0   Is i=0 or j=0?
                   │
            ┌──────┴──────┐
            │ YES         │ NO
            ▼             ▼
        dp[i][j] = 1   Calculate:
                       dp[i][j] = min(
                           dp[i-1][j],    ← above
                           dp[i][j-1],    ← left
                           dp[i-1][j-1]   ← diagonal
                       ) + 1
```

### Finding the Top-Left Corner

```
Given:
- Bottom-right position: (i, j)
- Square size: s = dp[i][j]

Calculate:
- Top-left row: i - s + 1
- Top-left col: j - s + 1

Example:
- Bottom-right: (6, 11)
- Size: 7
- Top-left: (6-7+1, 11-7+1) = (0, 5)
```

---

## Why the DP Recurrence Works

### Visual Explanation

For a cell to be the bottom-right of an n×n square, we need:
1. An (n-1)×(n-1) square above it
2. An (n-1)×(n-1) square to its left
3. An (n-1)×(n-1) square diagonally above-left

```
Visualization for dp[i][j]:

    j-1  j
i-1  D   T      D = diagonal
                T = top
  i  L   C      L = left
                C = current

For C to be bottom-right of size n:
- T must support size (n-1) above
- L must support size (n-1) to left
- D must support size (n-1) diagonally

Therefore: C = min(D, T, L) + 1
```

### Example: Building a 3×3 Square

```
Step 1: 1×1 squares (all empty cells)
. . .       1 1 1
. . .  →    1 1 1
. . .       1 1 1

Step 2: 2×2 squares
1 1 1       1 1 1
1 1 1  →    1 2 2
1 1 1       1 2 2

Step 3: 3×3 squares
1 1 1       1 1 1
1 2 2  →    1 2 2
1 2 2       1 2 3

The 3 at [2][2] means we can form a 3×3 square
with bottom-right at (2,2) and top-left at (0,0)
```

---

## Edge Cases Visualized

### Case 1: All Obstacles
```
Input:
2 . o x
oo
oo

DP Matrix:
0 0
0 0

Result: No square (size 0)
Output:
oo
oo
```

### Case 2: Single Empty Cell
```
Input:
2 . o x
o.
oo

DP Matrix:
0 1
0 0

Result: 1×1 square at (0,1)
Output:
ox
oo
```

### Case 3: One Row
```
Input:
1 . o x
.....

DP Matrix:
1 1 1 1 1

Result: 1×1 square (can only be 1 row high)
Output:
x....  (fills first cell, topmost leftmost)
```

### Case 4: One Column
```
Input:
5 . o x
.
.
.
.
.

DP Matrix:
1
1
1
1
1

Result: 1×1 square
Output:
x
.
.
.
.
```

---

## Tracking the Maximum

### Why Use `>` Not `>=`?

```
Consider this map:
. . . .
. . . .

DP Matrix:
1 1 1 1
1 2 2 2

Two 2×2 squares possible:
- Square A: rows 0-1, cols 0-1 (top-left at (0,0))
- Square B: rows 0-1, cols 1-2 (top-left at (0,1))
- Square C: rows 0-1, cols 2-3 (top-left at (0,2))

Using > (strictly greater):
- When we see dp[1][1] = 2, we record it
- When we see dp[1][2] = 2, we DON'T update (not greater)
- When we see dp[1][3] = 2, we DON'T update (not greater)

Result: We keep the FIRST occurrence (topmost, leftmost)
```

---

## Time Complexity Visualization

```
For an n×m grid:

Parse header:     O(1)
Load map:         O(n × m)
Build DP matrix:  O(n × m)   ← nested loops
Find maximum:     O(1)        ← done during DP
Fill square:      O(s²)       ← s ≤ min(n,m)
Print:            O(n × m)

Total: O(n × m)
```

---

## Space Complexity Visualization

```
Memory Usage:

Grid storage:     n × m characters
DP matrix:        n × m integers
Temporary vars:   O(1)

Total: O(n × m)

Note: DP matrix uses VLA (stack allocation) in improved version
```

---

## Common Mistakes Visualized

### Mistake 1: Wrong Top-Left Calculation

```
❌ WRONG:
top_left = bottom_right - size

Example: bottom_right = (3,3), size = 4
top_left = (3-4, 3-4) = (-1, -1)  ← Invalid!

✅ CORRECT:
top_left = bottom_right - size + 1

Example: bottom_right = (3,3), size = 4
top_left = (3-4+1, 3-4+1) = (0, 0)  ← Correct!
```

### Mistake 2: Not Initializing DP

```
❌ WRONG:
int dp[height][width];  // Uninitialized!
// dp contains garbage values

✅ CORRECT:
int dp[height][width];
for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
        dp[i][j] = 0;
```

---

## Summary

The DP algorithm efficiently finds the largest square by:

1. **Building up from smaller squares** (1×1 → 2×2 → 3×3 → ...)
2. **Using previous computations** (min of three neighbors + 1)
3. **Tracking the maximum** as we go
4. **Converting bottom-right to top-left** for filling

**Key Insight:** Each cell knows the largest square that ends there based on what its neighbors know!

---

*Use these visualizations to understand how the algorithm processes different map configurations.*
