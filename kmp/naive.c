#include <stdio.h>
#include <string.h>

// man 3 strstr
char *_strstr(char *haystack, char *needle)
{
	int i, j;

	int hlen = strlen(haystack);
	int nlen = strlen(needle);

	for (i = 0; i < hlen; i++)
	{
		for (j = 0; j < nlen; j++)
		{
			if (haystack[i + j] != needle[j])
				break;
		}
		if (j == nlen)
			return haystack + i;
	}
	return NULL;
}

int main()
{
	char *str = "abdabacad";
	char *pattern = "abac";

	char *first = _strstr(str, pattern);
	if (first == NULL)
	{
		printf("Not Found");
	}
	else
	{
		printf("FOUND at: %ld\n", (first - str));
	}

	return 0;
}