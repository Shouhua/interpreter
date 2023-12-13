#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define array_len(arr) (sizeof(arr) / sizeof(arr[0]))

int *make_table(const char *p)
{
	int table_size = strlen(p);
	int *t = malloc(table_size * sizeof(int));
	memset(t, 0, table_size);

	int i, j, len, max_prefix;
	i = 1;
	j = 0;
	len = strlen(p);
	t[0] = max_prefix = 0;
	while (i < len)
	{
		if (p[i] == p[j])
		{
			max_prefix++;
			t[i] = max_prefix;
			i++;
			j++;
		}
		else
		{
			if (i == 1)
			{
				t[i] = 0;
				i++;
				continue;
			}
			if (j == 0)
			{
				i++;
				j++;
				t[i] = 0;
			}
			else
			{
				max_prefix = j = t[j - 1];
			}
		}
	}
	return t;
}

int search(const char *s, const char *p, const int *t)
{
	size_t i, j, len;
	j = 0;
	len = strlen(s);
	for (i = 0; i < len; i++)
	{
		if (s[i] == p[j])
		{
			j++;
			if (j == strlen(p))
				return i - j + 1;
		}
		else
		{
			j = t[j];
		}
	}
	return -1;
}

int main()
{
	size_t i, j;
	// char *pattern = "abacabab";
	char *pattern = "ababc";
	char *str = "ababdababc";
	int *t;

	t = make_table(pattern);

	printf("next table: ");
	for (i = 0; i < strlen(pattern); i++)
	{
		printf("%d ", t[i]);
	}
	printf("\n");

	printf("matched index: %d\n", search(str, pattern, t));

	char *test_pattern[] = {
		"ababcabaa",
		"001201231",
		"ababc",
		"00120",
		"abacabab",
		"00101232",
		"abababca",
		"00123401"};
	for (i = 0; i < array_len(test_pattern); i = i + 2)
	{
		t = make_table(test_pattern[i]);
		for (j = 0; j < strlen(test_pattern[i]); j++)
		{
			if (t[j] != (test_pattern[i + 1][j] - '0'))
			{
				printf("[TEST] %s->%s FAILED\n", test_pattern[i], test_pattern[i + 1]);
				break;
			}
		}
		printf("[TEST] %s->%s OK\n", test_pattern[i], test_pattern[i + 1]);
	}
}