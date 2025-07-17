#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int    picoshell(char **cmds[])
{
	int fd[2];
	pid_t pid;
	int status;
	int res = 0;
	int i = -1;
	int var_stdin = 0;

	while(cmds[++i])
	{
		if (cmds[i + 1])
		{
			if (pipe(fd) == -1)
				return (1);
		}
		else
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork();
		if (pid == -1)
		{
			if (fd[0] != -1)
				close(fd[0]);
			if (fd[1] != -1)
				close (fd[1]);
			if (var_stdin != 0)
				close (var_stdin);
			return (1);
		}
		if (pid == 0)
		{
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) == -1)
					exit(1);
				close(fd[0]);
				close(fd[1]);
			}
			if (var_stdin != 0)
			{
				if (dup2(var_stdin, 0) == -1)
					exit(1);
				close(var_stdin);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd[1] != -1)
			close(fd[1]);
		if (var_stdin != 0)
			close(var_stdin);
		var_stdin = fd[0];	
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && (WEXITSTATUS(status) != 0))
			res = 1;
		else if (!WIFEXITED(status))
			res = 1;
	}
	return (res);
}

int	main(void)
{
	char *cmd1[] = {"echo", "banana", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/u/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	int res;

	res = picoshell(cmds);
	if (res)
		printf("Picoshell failed\n");
	else
		printf("Picoshell succeeded\n");
	return (0);
}