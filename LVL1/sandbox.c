#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>

void alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	struct sigaction sa;
	sa.sa_handler = alarm_handler;
	sa.sa_flags = 0;
	if (sigaction(SIGALRM, &sa, NULL) < 0)
		return (-1);

	pid_t pid;
	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		f();
		exit(0);
	}

	int status;
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
				printf("Nice function!\n");
			return (1);
		}
		else
		{
			if (verbose)
				printf("Bad function: exited with code %d\n", WEXITSTATUS(status));
			return (0);
		}
		return (-1);
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

void badf_exit()
{
	exit(2);
}

void badf_segf()
{
	char *str = NULL;
	str[2] = 'a';
}

void badf_timout()
{
	while(1)
		;
}

void badf_sig()
{
	sleep(5);
}

int main()
{
	int res;
	res = sandbox(nice_ft, 5, true);
	printf("Res is %d\n", res);
	res = sandbox(badf_exit, 5, true);
	printf("Res is %d\n", res);
	res = sandbox(badf_segf, 5, true);
	printf("Res is %d\n", res);
	res = sandbox(badf_timout, 2, true);
	printf("Res is %d\n", res);
	res = sandbox(badf_sig, 2, true);
	printf("Res is %d\n", res);
	return (0);
}