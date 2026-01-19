#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int h;
int w;
int interations;

void free_map(char **map)
{
    for(int i = 0; i < h; i++)
    {
            free(map[i]);
    }
    free(map);
}


int main()
{

}