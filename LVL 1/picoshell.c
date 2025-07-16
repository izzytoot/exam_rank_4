/* Assignment name:	picoshell
Expected files:		picoshell.c
Allowed functions:	close, fork, wait, exit, execvp, dup2, pipe
___________________________________________________________________

Write the following function:

int    picoshell(char **cmds[]);

The goal of this function is to execute a pipeline. It must execute each
commands of cmds and connect the output of one to the input of the
next command (just like a shell).

Cmds contains a null-terminated list of valid commands. Each rows
of cmds are an argv array directly usable for a call to execvp. The first
arguments of each command is the command name or path and can be passed
directly as the first argument of execvp.

If any error occur, The function must return 1 (you must of course
close all the open fds before). otherwise the function must wait all child
processes and return 0. You will find in this directory a file main.c which
contain something to help you test your function.


Examples: 
./picoshell /bin/ls "|" /usr/bin/grep picoshell
picoshell
./picoshell echo 'squalala' "|" cat "|" sed 's/a/b/g'
squblblb
*/

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int	picoshell(char **cmds[])
{
	int fd[2];
	int pid;
	int i = -1;
	int status;
	int fd_stdin = 0;
	int res = 0;

	if (!cmds)
		return (1);
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
				close(fd[1]);
			if (fd_stdin != 0)
				close(fd_stdin);
			return (1);
		}
		if (pid == 0)
		{
			if (fd_stdin != 0)
			{
				if(dup2(fd_stdin, 0) == -1)
					exit(1);
				close(fd_stdin);
			}
			if (fd[1] != -1)
			{
				if (dup2(fd[1], 1) == -1)
					exit(1);
				close(fd[0]);
				close(fd[1]);	
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		if (fd_stdin != 0)
			close(fd_stdin);
		if (fd[1] != -1)
			close(fd[1]);
		fd_stdin = fd[0];
	}
	while (wait(&status) > 0)
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
			res = 1;
		else if (!WIFEXITED(status))
			res = 1;
	}
	return (res);
}

int	main(void)
{
	char *cmd1[] = {"echo", "squalala", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/b/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	return (picoshell(cmds), 0);
}