#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char	**map;
int	lines, cols;
char	vazio, obst, cheio;

int	min(int a, int b, int c)
{
	if (a < b && a < c)
		return (a);
	if (c < b)
		return (c);
	return (b);
}

void	erro(void)
{
	fprintf(stderr, "map error\n");
}

int	ler(FILE *f)
{
	int	ret = fscanf(f, "%d %c %c %c", &lines, &vazio, &obst, &cheio);
	if (ret != 4 || lines <= 0 || (vazio == obst || vazio == cheio || obst == cheio))
		return (1);
	map = malloc(lines * sizeof(char *));
	if (!map) return (1);
	
	char	*buf = NULL;
	size_t	cap = 0;
	ssize_t	len = 0;
	ssize_t	rlen = 0;
	getline(&buf, &cap, f);
	free(buf);
	for (int i = 0; i < lines; i++)
	{
		buf = NULL;
		cap = 0;
		len = getline(&buf, &cap, f);
		if (len == -1 || (len != rlen && i > 0))
			return (1);
		rlen = len;
		if (buf[len - 1] == '\n')
			buf[len - 1] = 0;
		if (i == 0)
			cols = len - 1;
		if (cols <= 0)
			return 1;
		map[i] = buf;
	}
	return (0);
}

void	resolver(void)
{
	int	**dp = malloc(lines * sizeof(int *));
	int	size = 0, line = 0, col = 0;
	for (int i = 0; i < lines; i++)
		dp[i] = calloc(cols, sizeof(int));
	for (int i = 0; i < lines; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			if (map[i][j] == obst)
				dp[i][j] = 0;
			else if (i == 0 || j == 0)
				dp[i][j] = 1;
			else
				dp[i][j] = min(dp[i-1][j],dp[i][j-1],dp[i-1][j-1]) + 1;
			if (dp[i][j] > size)
			{
				size = dp[i][j];
				line = i - size + 1;
				col = j - size + 1;
			}
		}
	}
	for (int i = line; i < line + size; i++)
		for (int j = col; j < col + size; j++)
			map[i][j] = cheio;
	for (int i = 0;i < lines; i++)
		printf("%s\n", map[i]);
		
	for (int i = 0;i < lines; i++)
	{
		free(dp[i]);
		free(map[i]);
	}
	free(dp);
	free(map);
}

void	processar(FILE *f, int multiplos)
{
	if (!ler(f))
		resolver();
	else
		erro();
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
				erro();
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
