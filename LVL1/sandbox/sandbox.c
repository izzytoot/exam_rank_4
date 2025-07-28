#include <stdbool.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>

void alarm_handler(int sig)
{
	(void)sig;
}

int	sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t pid;
	int status;

	struct sigaction sa;
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
				printf("Nice function!\n");
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
		return(0);
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

void bad_ft_segf()
{
	char *str = NULL;
	str[2] = 'a';
}

void bad_ft_timeout()
{
	while(1)
		;
}

void bad_ft_sig()
{
	sleep(5);
}

int main()
{
	int r;

	r = sandbox(nice_ft, 5, true);
	perror("nice ft");
	printf("res is %d\n", r);
	r = sandbox(bad_ft_exit, 5, true);
	perror("bad ft exit");
	printf("res is %d\n", r);
	r = sandbox(bad_ft_segf, 5, true);
	perror("bad ft segfault");
	printf("res is %d\n", r);
	r = sandbox(bad_ft_timeout, 2, true);
	perror("bad ft timeout");
	printf("res is %d\n", r);
	r = sandbox(bad_ft_sig, 2, true);
	perror("bad ft signal");
	printf("res is %d\n", r);
	return (0);
}