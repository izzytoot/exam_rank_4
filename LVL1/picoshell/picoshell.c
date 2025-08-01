#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>

int picoshell(char **cmds[])
{
	pid_t pid;
	int fd[2] = {-1, -1};
	int status;
	int i = -1;
	int fd_std = 0;
	int res = 0;

	if (!cmds)
		return 1;
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
				if (fd_std > 0)
					close(fd_std);
				return 1;
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
			if (fd_std > 0)
				close(fd_std);
			return 1;
		}
		if (pid == 0)
		{
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) == -1)
					exit(1);
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
			}
			if (fd_std != 0)
			{
				if (dup2(fd_std, 0) == -1)
					exit(1);
				if (fd_std > 0)
					close(fd_std);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd[1] >= 0)
			close(fd[1]);
		if (fd_std > 0)
			close(fd_std);
		fd_std = fd[0];
	}
	if(waitpid(pid, &status, 0) == -1)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			res = 1;
		if (!WIFEXITED(status))
			res = 1;
	}
	if (fd[0] >= 0)
		close(fd[0]);
	if (fd[1] >= 0)
		close(fd[1]);
	if (fd_std > 0)
		close(fd_std);
	return res;
}

int main()
{
	char *cmd1[] = {"echo", "banana", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/u/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	int r = picoshell(cmds);
	if (r)
		perror("pico failure");
	else
		printf("pico success\n");
	exit(0);
}