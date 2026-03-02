#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int heigth; // vamos usar pra definir a altura da nossa matriz
int width; // vamos usar pra definir a largura da nossa matriz
int iteractios; // vamos usar pra definir o numero de geracoes que o nosso mundo vai viver


int **ft_alloc(void)
{
    int **table = malloc(sizeof(int *) * heigth);
    for (int i = 0; i < heigth; i++)
    {
        table[i] = calloc(width, sizeof(int));
    }
    return (table);
}

void    ft_print(int **table)
{
    for (int i = 0; i < heigth; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if (table[i][j])
                putchar('O');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}

void    read_rules(int **table)
{
    int y = 0;
    int x = 0;
    int readed = 0;
    int pen = 0;
    char chr = 0;

    while ((readed = read(0, &chr, 1)) > 0)
    {
        if (chr == 'w' && y > 0)
            y--;
        if (chr == 's' && y < heigth - 1)
            y++;
        if (chr == 'd' && x < width - 1)
            x++;
        if (chr == 'a' && x > 0)
            x--;
        if (chr == 'x')
            pen = !pen;
        if (pen)
            table[y][x] = pen;
        
    }
}

int alive(int **table, int y, int x)
{
    if (y < 0 || x < 0)
        return (0);
    if (y >= heigth || x >= width)
        return (0);
    if (table[y][x] == 1)
        return (1);
    return (0);
}

int count_neighboors(int **table, int y, int x)
{
    int count = 0;

    count += alive(table, y-1, x-1);
    count += alive(table, y, x-1);
    count += alive(table, y-1, x);

    count += alive(table, y+1, x+1);
    count += alive(table, y, x+1);
    count += alive(table, y+1, x);

    count += alive(table, y+1, x-1);
    count += alive(table, y-1, x+1);
    return (count);
}

void    update_world(int **table, int **tmp_table)
{
    for (int y = 0; y < heigth; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int neighboors = count_neighboors(table, y, x);
            if (table[y][x] == 1)
            {
                if (neighboors == 2 || neighboors == 3)
                    tmp_table[y][x] = 1;
                else
                    tmp_table[y][x] = 0;
            }
            else
            {
                if (neighboors == 3)
                    tmp_table[y][x] = 1;
                else
                    tmp_table[y][x] = 0;
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 4)
        return (1);
    width = atoi(argv[1]);
    heigth = atoi(argv[2]);
    iteractios = atoi(argv[3]);
    int **table = ft_alloc();
    int **tmp_table = ft_alloc();
    read_rules(table);
    for (int i = 0; i < iteractios; i++)
    {
        update_world(table, tmp_table);
        int **swp = table;
        table = tmp_table;
        tmp_table = swp;
    }
    ft_print(table);
    return (0);
}