#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
int ft_popen(const char *file, char *const argv[], char type)
{
	int fd[2];
	__pid_t pid;
	
	if (!file || !argv || (type != 'r' && type != 'w'))
		return (-1);
	if (pipe(fd) == -1)
		return (-1);
	pid = fork();
	if (pid == -1)
	{
		perror("pid:");
		close(fd[0]);
		close(fd[1]);
		return (-1);
	}
	if (pid == 0)
	{
		if (type == 'r')
		{
			if (dup2(fd[1], STDOUT_FILENO) == -1)
				exit(1);
		}
		else
		{
			if (dup2(fd[0], STDIN_FILENO) == -1)
				exit(1);
		}
		close(fd[0]);
		close(fd[1]);
		execvp(file, argv);
		exit(1);
	}
	if (type == 'r')
	{
		close(fd[1]);
		return (fd[0]);
	}
	else
	{
		close(fd[0]);
		return (fd[1]);
	}
}

int main()
{
	int fd_ls = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
	int fd_grep = ft_popen("grep", (char *const[]){"grep", "o", NULL}, 'w');
	char buffer[1024];
	int bytes;

	if (fd_ls == -1 || fd_grep == -1)
	{
		perror("popen:");
		return (1);
	}
	while ((bytes = read(fd_ls, buffer, 1024)) > 0)
		write(fd_grep, buffer, bytes);
	close(fd_grep);
	close(fd_ls);
	return (0);
}