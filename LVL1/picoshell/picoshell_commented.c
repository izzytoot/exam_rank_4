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

#include <unistd.h> //for close, fork, execvp, dup2 and pipe
#include <stdlib.h> //for exit
#include <sys/wait.h> //for wait

int    picoshell(char **cmds[])
{
	int fd[2]; //pipe fd's - fd[0] for read end and fd[1] for write end
	pid_t pid; //childprocess pid through fork
	int	i = -1; //to iterate through cmds
	int res = 0; //standard res is 0
	int status = 0; 
	int fd_stdin = 0; //stdin to return to after each loop

	fd[0] = -1;
	fd[1] = -1;
	while (cmds[++i]) //looping over each cmd
	{
		if (cmds[i + 1]) //if not last cmd
		{
			if (pipe(fd) == -1) //perform pipe. 
			{
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
				if (fd_stdin > 0)
					close(fd_stdin);
				return (1); //if error return
			}
		}
		else //identify last loop
		{
			fd[0] = -1;
			fd[1] = -1;
		}
		pid = fork(); //create new child process
		if (pid == -1) //if error, close fds and return
		{
			if (fd[0] >= 0) //check if open
				close(fd[0]);
			if (fd[1] >= 0) //check if open
				close(fd[1]);
			if (fd_stdin > 0) //check if changed
				close(fd_stdin);
			return (1);
		}
		if (pid == 0) //child process
		{
			if (fd_stdin != 0) //if there has been previous cmd
			{
				if (dup2(fd_stdin, 0) == -1) //stdin becomes previous cmd's output
					exit(1);
				if (fd_stdin > 0)
				close (fd_stdin);
			}
			if (fd[1] != -1) //if not last cmd
			{
				if (dup2(fd[1], 1) == -1) //stdout becomes pipe's write end (fd[1])
					exit(1);
				if (fd[0] >= 0)
					close(fd[0]);
				if (fd[1] >= 0)
					close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]); //exec cmd and args
			exit(1); //in case execvp fails
		}
		if (fd_stdin > 0) //if prev cmd
			close(fd_stdin); // we don't need stdin from prev cmd anymore
		if (fd[1] >= 0) //if write end open
			close(fd[1]); //close it
		fd_stdin = fd[0]; //save the read end for the next cmd
	}
	if (waitpid(pid, &status, NULL) == -1) //while all children are finishing we wait
	{
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) //if it exited with a non-0 status (error)
			res = 1;
		else if (!WIFEXITED(status)) //if it didn't exit normally
			res = 1;
	}
	if (fd[0] >= 0)
		close(fd[0]);
	if (fd[1] >= 0)
		close(fd[1]);
	if (fd_stdin > 0)
		close(fd_stdin);
	return (res); //0 if success, 1 if error
}

int main(void)
{
	/*
	char *cmd1[] = {"ls", NULL};
	char *cmd2[] = {"grep", "picoshell", NULL};
	char **cmds[] = {cmd1, cmd2, NULL};
	*/
	char *cmd1[] = {"echo", "squalala", NULL};
	char *cmd2[] = {"cat", NULL};
	char *cmd3[] = {"sed", "s/a/b/g", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	return (picoshell(cmds));
}