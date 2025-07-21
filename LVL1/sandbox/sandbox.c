#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>
#include <signal.h>

void	alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction sa;
	sa.sa_handler = alarm_handler;
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, 0) < 0)
		return (-1);

	int status;
	pid_t pid;
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
		if (verbose)
			printf("Bad function: %s\n", strsignal(WTERMSIG(status)));
		return (0);
	}
	return (-1);
}

void nice_ft()
{
	return ;
}

void bad_ft_exit()
{
	exit(1);
}

void bad_ft_segfault()
{
	char *str = NULL;
	str[2] = 'a';
}

void bad_ft_timeout()
{
	while(1)
		;
}

void bad_ft_sigkill()
{
	sleep(5);
}

int main()
{
	int res;

	res = sandbox(nice_ft, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_exit, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_segfault, 5, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_timeout, 2, true);
	printf("res is %d\n", res);
	res = sandbox(bad_ft_sigkill, 2, true);
	printf("res is %d\n", res);
}