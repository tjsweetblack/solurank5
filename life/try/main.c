#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int height;
int width;
int n_iteration;

int **create_map()
{
    int **tab = malloc(sizeof(int*) * height);
    for(int i = 0; i < height; i++)
    {
        tab[i] = calloc(width, sizeof(int));
    }
    return tab;
}
 void ft_print(int **tab)
 {
    for(int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if(tab[i][j] == 1)
                printf("O");
            if(tab[i][j] == 0)
                printf(" ");
            if(j == width - 1)
                printf("\n");
        }
    }
 }

  void ft_print_tab(int **tab)
 {
    for(int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if(tab[i][j] == 1)
                printf("1");
            if(tab[i][j] == 0)
                printf("0");
            if(j == width - 1)
                printf("\n");
        }
    }
 }

 int alive(int **tab, int i, int j)
 {
    if(i < 0 || j < 0)
        return 0;
    
    if(i > (height - 1) || j > (width - 1))
        return 0;

    if(tab[i][j] == 1)
        return 1;
    return 0;
 }

 int count_neighbours(int **tab, int i, int j)
 {
    int count = 0;

    count += alive(tab, i + 1, j);
    count += alive(tab, i - 1, j);
    count += alive(tab, i, j + 1);
    count += alive(tab, i, j - 1);
    count += alive(tab, i + 1, j - 1);
    count += alive(tab, i + 1, j + 1);
    count += alive(tab, i - 1, j + 1);
    count += alive(tab, i - 1, j - 1);

    return count;
 }

 void read_input(int **table)
 {
    int i = 0;
    int j = 0;
    char c = 0;
    int readed = 0;
    int pen = 0;
    while((readed = read(0, &c, 1)) > 0)
    {
        if(c == 'w' && i > 0)
            i--;
        if(c == 's' && i < height)
            i++;
        if(c == 'd' && j < width)
            j++;
        if(c == 'a' && j > 0)
            j--;
        if(c == 'x')
            pen = !pen;
        if(pen)
        table[i][j] = pen;
    }
 }

 void update_world(int **tab, int **temp)
 {
    for(int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int neibours = count_neighbours(tab, i, j);
            if(tab[i][j] == 1)
            {
                if(neibours == 2 || neibours == 3)
                    temp[i][j] = 1;
                if (neibours < 2)
                    temp[i][j] = 0;
            }
            if(tab[i][j] == 0)
            {
                if(neibours == 3)
                    temp[i][j] = 1;
                else
                    temp[i][j] = 0;
            }
        }      
    }
 }

 

 int main(int argc, char**argv)
 {
    if(argc != 4)
        return 1;

    height = atoi(argv[1]);
    width = atoi(argv[2]);
    n_iteration = atoi(argv[2]);
    int **tab = create_map();
    int **temp = create_map();

    if(height < 0 || width < 0 || n_iteration < 0)
        return 1;
    
    read_input(tab);
    ft_print_tab(tab);
    for(int i = 1; i <= n_iteration; i++)
    {
        update_world(tab, temp);
        int **swp = tab;
        tab = temp;
        temp = swp;
    }
    ft_print(tab);
    return 0;
 }