#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int picoshell(char **cmds[])
{
	int fd[2];
	pid_t pid;
	int status;
	int res = 0;
	int std_in = 0;
	int i = -1;

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
			return (1);
		if (pid == 0)
		{
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) == -1)
					exit(1);
				close(fd[0]);
				close(fd[1]);
			}
			if (std_in != 0)
			{
				if (dup2(std_in, 0) == -1)
					exit(1);
				close(std_in);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd[1] != -1)
			close(fd[1]);
		if (std_in != 0)
			close(std_in);
		std_in = fd[0];
	}
	while(wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			res = 1;
		else if (!WIFEXITED(status))
			res = 1;
	}
	return (0);
}

int main()
{
	char *cmd1[] = {"echo", "banana", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/u/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	int res = picoshell(cmds);

	if (res)
		printf("Pico failed :(\n");
	else
		printf("Pico succeeded :)\n");
	return (0);
}