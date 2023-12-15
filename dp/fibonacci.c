#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

int *a;

long fib(int n)
{
	if (n == 0 || n == 1)
		return 1;
	return fib(n - 1) + fib(n - 2);
}

long fib_mem(int n)
{
	if (a[n] > 0)
		return a[n];
	a[n] = fib_mem(n - 1) + fib_mem(n - 2);
	return a[n];
}

long fib_bottom_to_up(int n)
{
	long r1, r2, r;
	int i;

	r1 = 0;
	r2 = 1;
	for (i = 0; i < n; i++)
	{
		r = r1 + r2;
		r1 = r2;
		r2 = r;
	}
	return r;
}

int main(int argc, char *argv[])
{
	if (argc != 2)
		printf("Usage: ./fibonacci NUMBER");
	int n = atoi(argv[1]);

	int pid_done_num;
	pid_t p1, p2, p3, p;
	time_t start_time, end_time;
	int status;

	pid_done_num = 0;
	start_time = time(NULL);

	if ((p1 = fork()) == 0)
	{
		printf("basic fib(%d) = %ld\n", n, fib(n));
		_exit(0);
	}

	if ((p2 = fork()) == 0)
	{
		a = (int *)malloc(sizeof(int) * n);
		memset(a, 0, n);
		a[0] = a[1] = 1;
		printf("array fib_mem(%d) = %ld\n", n, fib_mem(n));
		_exit(0);
	}

	if ((p3 = fork()) == 0)
	{
		printf("bottom2up fib_bottom_to_up(%d) = %ld\n", n, fib_bottom_to_up(n));
		_exit(0);
	}

	while (1)
	{
		if (pid_done_num == 3)
			break;
		p = waitpid(-1, &status, WNOHANG);
		if (p == p1 || p == p2 || p == p3)
		{
			end_time = time(NULL);
			printf("%s DONE: %lds\n", p == p1 ? "basic" : (p == p2 ? "array" : "bottom2up"), (end_time - start_time));
			pid_done_num++;
		}
	}

	return EXIT_SUCCESS;
}