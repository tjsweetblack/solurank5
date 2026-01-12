#ifndef BSQ_H
#define BSQ_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Structure to hold map metadata from first line */
typedef struct s_info {
    int rows;
    char empty;
    char obstacle;
    char full;
} t_info;

/* Structure to hold the map grid */
typedef struct s_map {
    char **grid;
    int width;
    int height;
} t_map;

/* Structure to hold square information */
typedef struct s_square {
    int size;
    int row;
    int col;
} t_square;

/* Core functions */
int     parse_info(FILE *file, t_info *info);
int     load_map(FILE *file, t_map *map, t_info *info);
void    find_largest_square(t_map *map, t_square *square, t_info *info);
void    fill_and_print(t_map *map, t_square *square, t_info *info);
void    free_map(t_map *map);

/* Main execution */
int     process_map(FILE *file);

#endif
