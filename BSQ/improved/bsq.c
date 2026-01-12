#include "bsq.h"

/*
 * parse_info - Parse and validate the first line of input
 * 
 * Expected format: "<rows> <empty> <obstacle> <full>"
 * 
 * Validations:
 * - Exactly 4 values must be read
 * - rows must be positive
 * - All three characters must be unique
 * - All characters must be printable (ASCII 32-126)
 * 
 * Returns: 0 on success, -1 on error
 */
int parse_info(FILE *file, t_info *info)
{
    if (fscanf(file, "%d %c %c %c", &info->rows, &info->empty, 
               &info->obstacle, &info->full) != 4)
        return -1;
    
    if (info->rows <= 0)
        return -1;
    
    if (info->empty == info->obstacle || 
        info->empty == info->full || 
        info->obstacle == info->full)
        return -1;
    
    if (info->empty < 32 || info->empty > 126 ||
        info->obstacle < 32 || info->obstacle > 126 ||
        info->full < 32 || info->full > 126)
        return -1;
    
    return 0;
}

/*
 * free_map - Free all memory allocated for the map
 */
void free_map(t_map *map)
{
    if (!map || !map->grid)
        return;
    
    for (int i = 0; i < map->height; i++) {
        if (map->grid[i])
            free(map->grid[i]);
    }
    free(map->grid);
    map->grid = NULL;
}

/*
 * validate_line - Check if a line contains only valid characters
 * 
 * Returns: 0 if valid, -1 if invalid character found
 */
static int validate_line(char *line, int len, t_info *info)
{
    for (int i = 0; i < len; i++) {
        if (line[i] != info->empty && line[i] != info->obstacle)
            return -1;
    }
    return 0;
}

/*
 * load_map - Read and validate the map from file
 * 
 * Process:
 * 1. Allocate memory for grid
 * 2. Skip the newline after header
 * 3. Read each line using getline
 * 4. Validate line endings and length
 * 5. Store lines without trailing newline
 * 6. Validate characters in each line
 * 
 * Returns: 0 on success, -1 on error
 */
int load_map(FILE *file, t_map *map, t_info *info)
{
    char *line = NULL;
    size_t buf_size = 0;
    ssize_t len;
    
    map->height = info->rows;
    map->width = 0;
    map->grid = malloc(map->height * sizeof(char *));
    if (!map->grid)
        return -1;
    
    /* Initialize pointers to NULL for safe cleanup */
    for (int i = 0; i < map->height; i++)
        map->grid[i] = NULL;
    
    /* Skip newline after header line */
    if (getline(&line, &buf_size, file) == -1) {
        free(line);
        return -1;
    }
    
    /* Read each map line */
    for (int i = 0; i < map->height; i++) {
        len = getline(&line, &buf_size, file);
        
        if (len == -1) {
            free(line);
            return -1;
        }
        
        /* Line must end with newline */
        if (len == 0 || line[len - 1] != '\n') {
            free(line);
            return -1;
        }
        
        /* Remove newline */
        len--;
        
        /* First line sets the width */
        if (i == 0) {
            if (len <= 0) {
                free(line);
                return -1;
            }
            map->width = len;
        }
        /* All other lines must match width */
        else if (len != map->width) {
            free(line);
            return -1;
        }
        
        /* Allocate and copy line without newline */
        map->grid[i] = malloc(len + 1);
        if (!map->grid[i]) {
            free(line);
            return -1;
        }
        
        memcpy(map->grid[i], line, len);
        map->grid[i][len] = '\0';
        
        /* Validate characters */
        if (validate_line(map->grid[i], len, info) == -1) {
            free(line);
            return -1;
        }
    }
    
    free(line);
    return 0;
}

/*
 * min3 - Return minimum of three integers
 */
static int min3(int a, int b, int c)
{
    int min = a;
    if (b < min) min = b;
    if (c < min) min = c;
    return min;
}

/*
 * find_largest_square - Use dynamic programming to find largest square
 * 
 * Algorithm: Maximal Square DP
 * 
 * For each cell (i,j):
 *   - If obstacle: dp[i][j] = 0
 *   - If first row/col: dp[i][j] = 1
 *   - Otherwise: dp[i][j] = min(top, left, diagonal) + 1
 * 
 * The dp[i][j] value represents the side length of the largest square
 * whose bottom-right corner is at position (i,j).
 * 
 * We track the maximum value found and its position to determine the
 * largest square in the map.
 */
void find_largest_square(t_map *map, t_square *square, t_info *info)
{
    /* Allocate DP matrix on stack (VLA) */
    int dp[map->height][map->width];
    
    /* Initialize square to size 0 */
    square->size = 0;
    square->row = 0;
    square->col = 0;
    
    /* Fill DP matrix */
    for (int i = 0; i < map->height; i++) {
        for (int j = 0; j < map->width; j++) {
            /* If obstacle, square size is 0 */
            if (map->grid[i][j] == info->obstacle) {
                dp[i][j] = 0;
            }
            /* First row or column, max square size is 1 */
            else if (i == 0 || j == 0) {
                dp[i][j] = 1;
            }
            /* Use recurrence relation */
            else {
                dp[i][j] = min3(dp[i-1][j], dp[i][j-1], dp[i-1][j-1]) + 1;
            }
            
            /* Update maximum if we found a larger square */
            /* Using > (not >=) ensures we prefer top-left squares */
            if (dp[i][j] > square->size) {
                square->size = dp[i][j];
                /* Calculate top-left corner from bottom-right */
                square->row = i - dp[i][j] + 1;
                square->col = j - dp[i][j] + 1;
            }
        }
    }
}

/*
 * fill_and_print - Fill the largest square and print the result
 * 
 * Modifies the map grid to fill the square with the 'full' character,
 * then prints the entire map to stdout.
 */
void fill_and_print(t_map *map, t_square *square, t_info *info)
{
    /* Fill the square */
    for (int i = square->row; i < square->row + square->size; i++) {
        for (int j = square->col; j < square->col + square->size; j++) {
            if (i < map->height && j < map->width)
                map->grid[i][j] = info->full;
        }
    }
    
    /* Print the result */
    for (int i = 0; i < map->height; i++) {
        printf("%s\n", map->grid[i]);
    }
}

/*
 * process_map - Main execution flow for processing a single map
 * 
 * Steps:
 * 1. Parse header information
 * 2. Load and validate map
 * 3. Find largest square using DP
 * 4. Fill square and print result
 * 5. Clean up memory
 * 
 * Returns: 0 on success, -1 on error
 */
int process_map(FILE *file)
{
    t_info info;
    t_map map;
    t_square square;
    
    /* Parse header */
    if (parse_info(file, &info) == -1)
        return -1;
    
    /* Load map */
    if (load_map(file, &map, &info) == -1) {
        free_map(&map);
        return -1;
    }
    
    /* Find largest square */
    find_largest_square(&map, &square, &info);
    
    /* Fill and print result */
    fill_and_print(&map, &square, &info);
    
    /* Clean up */
    free_map(&map);
    
    return 0;
}
