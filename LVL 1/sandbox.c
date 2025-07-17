#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>

void	alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction	sa;
	pid_t	pid;
	int status;
	int sig;

	sa.sa_handler = alarm_handler;
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}
	alarm(timeout);
	if (waitpid(pid, &status, 0) == -1)
	{
		if (errno == EINTR)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
			if (verbose)
				printf("Bad function: timed out after %d seconds\n", timeout);
			return (0);
		}
		return (-1);
	}
	if (WIFEXITED(status))
	{
		if (WEXITSTATUS(status) == 0)
		{
			if (verbose)
				printf("Nice function\n");
			return (1);
		}
		else
		{
			if (verbose)
				printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
			return (0);
		}
	}
	if (WIFSIGNALED(status))
	{
		sig = WTERMSIG(status);
		if (verbose)
			printf("Bad function: %s\n", strsignal(sig));
		return (0);
	}
	return (-1);
}

void	nice_ft(void)
{
	return ;
}

void bad_ft_exit(void)
{
	exit(1);
}

void bad_ft_segfault(void)
{
	char *str = NULL;
	str[2] = 'a';
}

void bad_ft_timout(void)
{
	while(1) 
	{

	}
}

void	bad_ft_sigkill(void)
{
	sleep(5);
}

int	main(void)
{
	int res;

	res = sandbox(nice_ft, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_exit, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_segfault, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_timout, 2, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_sigkill, 2, true);
	printf("res is %d\n", res);
}