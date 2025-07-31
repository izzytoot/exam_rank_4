#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <errno.h>

int	picoshell(char **cmds[])
{
	int fd[2];
	__pid_t pid;
	int status;
	int i = -1;
	int res = 0;
	int fdstd = 0;

	fd[0] = -1;
	fd[1] = -1;
	if (!cmds)
		return (1);
	while(cmds[++i])
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) == -1)
			{
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
				if (fdstd > 0)
					close(fdstd);
				return (1);
			}
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid == -1)
		{
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
				if (fdstd > 0)
					close(fdstd);
			return (1);
		}
		if (pid == 0)
		{
			if (fd[1] != -1)
			{
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit(1);
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
			}
			if (fdstd != 0)
			{
				if (dup2(fdstd, STDIN_FILENO) == -1)
					exit(1);
				if (fdstd > 0)
					close(fdstd);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd[1] >= 0)
			close(fd[1]);
		if (fdstd > 0)
			close(fdstd);
		fdstd = fd[0];
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
		{
			res = 1;
			break;
		}
		if (!WIFEXITED(status))
		{
			res = 1;
			break;
		}
	}
	if (fd[0] >= 0)
		close(fd[0]);
	if (fd[1] >= 0)
		close(fd[1]);
	if (fdstd > 0)
		close(fdstd);
	return (res);
}

int main()
{
	char *cmd1[] = {"echo", "banana", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"cmd", "s/a/u/g", NULL};
	// char *cmd4[] = {NULL};
	// char *cmd5[] = {"ola", "nofile", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	int res;

	res = picoshell(cmds);
	if (res)
		perror("Pico failure");
	else
		printf("pico success\n");
	return (0);
}