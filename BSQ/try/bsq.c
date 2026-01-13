#include "bsq.h"

int loadElements(FILE* file, t_elements *element)
{
    int ret = fscanf(file, "%d %c %c %c", &(elements.n_lines), &(elements.empty), &(elements.obst), &(elements.full));
    if(ret != 4)
        return -1;

    if(elements->n_lines <= 0)
        return -1;
    if(elements->empty == elements->obstacle || elements->empty == elements-> full || elements->obstacle == elements->full)
        return -1;
    if(elements->empty < 32 || elements->empty > 126)
        return -1;
    if(elements->obstacle < 32 || elements->obstacle > 126)
        return -1;
    if(elements->full < 32 || elements->full > 126)
        return -1;

    return 0;
}

int loadMap(FILE* file, t_map *map, t_elements *element)
{
    
}

int execute_bsq(FILE* file)
{
    t_elements elements;
    if(loadElements(file, &elements) == -1)
        return (-1);
    t_map map;
    if (loadMap(file, &map, &elements) == -1)
        return (-1);

    t_square square;
    square.size = 0; square.i = 0; square.j = 0;
    find_big_square(&map, &square, &elements);
    print_filled_map(&map, &square, &elements);
    free_map(map.grid);
    return (0);
}

//int execute_bsq(FILE* file);
int convert_file_pointer(char* name)
{
    FILE* file = fopen(name, 'r');

    if(!file)
        return -1;
    int ret = 0;
    ret = execute_bsq(file);
    close(file);
    return ret;
}