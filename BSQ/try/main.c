#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int h;
int w;
char **map;
char emp, obs, full;

void errorr()
{
    fprintf(stderr, "map error\n");
}

int min(int a, int b, int c)
{
    int min = a;
    if(min > b)
        min = b;
    if(min > c)
        min = c;
    return min;
}

void solve()
{
    int **dp = malloc(sizeof(int *) * h);
    if(!dp)
    {
        errorr();
        return ;
    }
    for (int i = 0; i < h; i++)
    {
        dp[i] = calloc(w, sizeof(int));
        if(!dp[i])
        {
            errorr();
            return ;
        }
    }
    int size = 0;
    int y = 0;
    int x = 0;

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if(map[i][j] != emp && map[i][j] != obs)
            {
                errorr();
                return ;
            }
            if(map[i][j] == obs)
                dp[i][j] = 0;
            else if(i == 0 || j == 0)
                dp[i][j] = 1;
            else
                dp[i][j] = min(dp[i - 1][j], dp[i - 1][j - 1], dp[i][j - 1]) + 1;
            
            if(size < dp[i][j])
            {
                size = dp[i][j];
                y = i - size + 1;
                x = j - size + 1;
            }
        }
        
    }

    for(int i = y; i < y + size; i++)
        for(int j = x; j < x + size; j++)
            map[i][j] = full;

    for(int i = 0; i < h; i++)
        fprintf(stdout, "%s\n", map[i]);

    for (int i = 0; i < h; i++)
    {
        free(dp[i]);
        free(map[i]);
    }
    free(dp);
    free(map);
    
}

int read_file(FILE *file)
{
    int readed = fscanf(file, "%d %c %c %c", &h, &emp, &obs, &full);
    if(readed != 4 || h <= 0 || emp == obs || obs == full || emp == full)
    {
        return 1;
    }   
    map = malloc(sizeof(char *) * h);
    if(!map)
    {
        return 1;
    }
    size_t cap = 0;
    int readed2 = 0;
    int readed3 = 0;
    char *line = NULL;
    getline(&line, &cap, file);
    free(line);
    for (int i = 0; i < h; i++)
    {
        char *line2 = NULL;
        readed2 = getline(&line2, &cap, file);
        if(i == 0)
            readed3 = readed2;
        if(readed2 != readed3 || readed2 <= 0)
            return 1;
        if(line2[readed3 - 1] == '\n')
            line2[readed3 - 1] = '\0';
        else
             return 1;
        map[i] = line2;
    }
    w = readed3 - 1;
    if(w <= 0)
        return 1;
     return 0;
}

void process(FILE *file, int multi)
{
    if(!read_file(file))
        solve();
    else
        errorr();

    if(multi)
        fprintf(stdout, "\n");
}
int main(int argc, char **argv)
{
    if(argc == 1)
    {
        process(stdin, 0);
    }
    else
    {
        for(int i = 1; i < argc; i++)
        {
            FILE *file = fopen(argv[i], "r");
            if(!file)
            {
                errorr();
                if(argc > 2)
                    fprintf(stdout, "\n");
                continue;
            }
            process(file, argc > 2);
            fclose(file);
        }
    }
}