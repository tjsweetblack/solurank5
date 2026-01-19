#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

char **map;
int height;
int width;
char empty, obs, full;


void error()
{
    fprintf(stderr, "map error\n");
}

int min(int a, int b, int c)
{
    int min = a;
    if(b < a)
        min = b;
    if(c < b)
        min = c;
    if(a < c)
        min = a;
    return min;
}

int read_file(FILE* file)
{
    int readed = fscanf(file, "%d %c %c %c", &height, &empty, &obs, &full);
    map = malloc(sizeof(char*)*height);
    size_t cap = 0;
    char *line2 = NULL;
    
    getline(&line2, &cap, file);
    //free(line);
    
    for(int i = 0; i < height; i++)
    {
        char *line = NULL;
        size_t len = getline(&line, &cap, file);
        if (len == -1)
        return 1;
        
        map[i] = line;
        width = len;
    }
    for (int i = 0; i < height; i++)
    {
        //printf("%s",map[i]);
    }
    
    return 0;
}

int solve()
{
    int size = 0;
    int col = 0;
    int row = 0;
    int **dp = malloc(sizeof(int*)*height);
    for(int i = 0; i < height; i++)
        dp[i] = calloc(width, sizeof(int));
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            if(map[i][j] == obs)
                dp[i][j] = 0;
            if(i == 0 || j == 0)
                dp[i][j] = 1;
            else if(map[i][j] == empty)
                dp[i][j] = min(dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]) + 1;
            
            if(dp[i][j] > size)
            {
                size = dp[i][j];
                col = i - size + 1;
                row = j - size + 1;
            }
        }
    }
    for(int i = col; i < col + size; i++)
        for(int j = row; j < row + size; j++)
            map[i][j] = full;
    
    for(int i = 0; i < height; i++)
        printf("%s", map[i]);

    // for (int i = 0;i < height; i++)
	// {
	// 	free(dp[i]);
	// 	free(map[i]);
	// }
	// free(dp);
	// free(map);
    return 0;

}

void	processar(FILE *f, int multiplos)
{
	if (!read_file(f))
		solve();
	else
		error();
	if (multiplos)
		printf("\n");
}

int	main(int ac, char **av)
{
	FILE	*f;

	if (ac == 1)
	{
		processar(stdin, 0);
	}
	else
	{
		for (int i = 1; i < ac; i++)
		{
			f = fopen(av[i], "r");
			if (!f)
			{
				error();
				if (ac > 2)
					printf("\n");
				continue ;
			}
			processar(f, ac > 2);
			fclose(f);
		}
	}
	return (0);
}
