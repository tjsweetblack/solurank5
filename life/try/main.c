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
void die_or_stay();

void solve_iteration(int iter)
{
    for(int i = 0; i < h; i++)
    {
        for(int j = 0; j < w; j++)
        {
             if((alive(map[i][j])) == 1)
                die_or_stay(map[i][j]);
            if((alive(map[i][j])) == 0)
                live_or_stay(map[i][j]);
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

}