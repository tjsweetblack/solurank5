#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int h;
int w;
int n_iteration;

int **ft_alloc()
{
    int **tab = malloc(sizeof(int *) * h);
    for (int i = 0; i < h; i++)
    {
        tab[i] = calloc(w, sizeof(int));
    }
    return tab;
}

void read_input(int **tab)
{
    int readed = 0;
    char c = 0;
    int i = 0;
    int j = 0;
    int pen = 0;
    while((readed = read(0, &c, 1) > 0))
    {
        if(c == 'w' && i > 0)
            i--;
        if(c == 's' && i < h - 1)
            i++;
        if(c == 'a' && j > 0)
            j--;
        if(c == 'd' && j < w - 1)
            j++;
        if(c == 'x')
            pen = !pen;
        if(pen)
            tab[i][j] = pen;
    }
}

int alive(int **tab, int i, int j)
{
    if(i < 0 || j < 0)
        return 0;
    if(i >= h || j >= w)
        return 0;
    if(tab[i][j] == 1)
        return 1;
    return 0;
}

int count_neibours(int **tab, int i, int j)
{
    int count = 0;

    count += alive(tab, i - 1, j - 1);
    count += alive(tab, i - 1, j);
    count += alive(tab, i - 1, j + 1);
    count += alive(tab, i, j - 1);
    count += alive(tab, i, j + 1);
    count += alive(tab, i + 1, j - 1);
    count += alive(tab, i + 1, j);
    count += alive(tab, i + 1, j + 1);
    return count;
}

void update_map(int **tab, int **temp)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            int count = count_neibours(tab, i, j);
            if(tab[i][j] == 1)
            {
                if(count == 2 || count == 3)
                    temp[i][j] = 1;
                else
                    temp[i][j] = 0;
            }
            else if(tab[i][j] == 0)
            {
                if(count == 3)
                    temp[i][j] = 1;
                else
                    temp[i][j] = 0;
            }
        }
        
    }
    
}

void print_map(int **tab)
{
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if(tab[i][j] == 1)
                putchar('O');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}

void free_tab(int **tab)
{
    for (int i = 0; i < h; i++)
    {
        free(tab[i]);
    }
    free(tab);
}

int main(int argc, char **argv)
{
    if(argc != 4)
        return 1;
    w = atoi(argv[1]);
    h = atoi(argv[2]);
    n_iteration = atoi(argv[3]);
    int **tab = ft_alloc();
    int **temp = ft_alloc();
    read_input(tab);
    for(int i = 0; i < n_iteration; i++)
    {
        update_map(tab, temp);
        int **swp = tab;
        tab = temp;
        temp = swp;
    }
    print_map(tab);
    free_tab(tab);
    free_tab(temp);
    return 0;
}