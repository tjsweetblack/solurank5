#include "unistd.h"
#include "stdio.h"


int **map;

void free_map(char **map)
{
    for(int i = 0; i < h; i++)
    {
            free(map[i])
    }
    free(map);
}

int create_map(int h, int w)
{
    map = malloc(sizeof(int) * h);
    if(!map)
    {
        free(map);
        return -1;
    }
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
            map[i] = calloc(sizeof(int) * w);
        if(!map[i])
            return -1;
    }
    return 0;
}

void place_in_map()
{
    int i = 0;
    int j = 0;
    char c;
    size_t red;
    while(red = read(STDIN, &c, 1) > 0)
    {
        if(c == 'w')
            i -= 1;
        if(c == 's')
            i += 1;
        if(c == 'a')
            j -= 1;
        if(c == 'd')
            j += 1;

        if(c == 'x')
        {
            if(map[i][j] == 0)
                map[i][j] = 1;
            if(map[i][j] == 1)
                map[i][j] = 0;
        }
    }
}

int count_neighboors(int i, int j)
{
    int count = 0;
    if(map[i-1][j] == 1)
        count++;
    if(map[i+1][j] == 1)
        count++;
    if(map[i][j-1] == 1)
        count++;
    if(map[i][j+1] == 1)
        count++;
    // all 4 diagnals
    if(map[i-1][j-1] == 1)
        count++;
    if(map[i-1][j+1] == 1)
        count++;
    if(map[i+1][j-1] == 1)
        count++;
    if(map[i+1][j+1] == 1)
        count++;
    
    return(count);
}

void die_or_stay(int i, int j)
{
    int count = count_neighboors(i, j);

    if(count >= 2)
        map[i][j] = 1;
    if(count < 2)
        map[i][j] = 0;
}

void live_or_stay(int i, int j)
{
    int count = count_neighboors(i, j);

    if(count >= 3)
        map[i][j] = 1;
    if(count < 3)
        map[i][j] = 0;
}

void solve_iteration(int iter)
{
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
             if((alive(map[i][j])) == 1)
                die_or_stay(i, j);
            if((alive(map[i][j])) == 0)
                live_or_stay(map[i][j]);
        }
    }
}

void print_ans()
{
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
             if(map[i][j] == 1)
                printf("0");
            if(map[i][j] == 1)
                printf(" ");
        }
    }
}

int main(int argc, char** argv)
{
    int h = atoi(argv[1]);
    int w = atoi(argv[2]);
    int n_iteration = atoi(argv[1]);

    if(h > 0 && w > 0 && n_iteration > 0)
        if(!(create_map(h, w)))
            return -1;
    if(!place_in_map())
    for(int i =  0; i <= n_iteration; i++)
    {
        solve_iteration(n_iteration);
    }
    print_ans();
    return(0);
}