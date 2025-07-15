/* Assignment name  : ft_popen
Expected files   : ft_popen.c
Allowed functions: pipe, fork, dup2, execvp, close, exit
--------------------------------------------------------------------------------------

Write the following function:

int ft_popen(const char *file, char *const argv[], char type);

The function must launch the executable file with the arguments argv (using execvp).
If type is 'r' the function must return a file descriptor connected to the output of the command.
If type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

For example, the function could be used like that:

int main()
{
    int  fd;
    char *line;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    while ((line = get_next_line(fd)))
        ft_putstr(line);
    return (0);
}


int	main() {
	int	fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
	dup2(fd, 0);
	fd = ft_popen("grep", (char *const []){"grep", "c", NULL}, 'r');
	char	*line;
	while ((line = get_next_line(fd)))
		printf("%s", line);
}


Hints:
Do not leak file descriptors!
This exercise is inspired by the libc's popen(). */

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int    ft_popen(const char *file, char *const av[], char type)
{
    int fd[2]; //pipe - 0 for input and 1 for output
    int pid;

    if (!file || !av || (type != 'r' && type != 'w'))
        return (-1);
    if (pipe(fd) == -1)
        return (-1);
    pid = fork ();
    if (pid == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return (-1);
    }
    if (pid == 0) //childprocess
    {
        if (type == 'r') // Redirect stdout to pipe write end
        {
            if (dup2(fd[1], 1) == -1) //stdout is now fd[1]
                exit (1);
        }    
        if (type == 'w') // Redirect stdin to pipe read end
        {
            if (dup2(fd[0], 0) == -1) //stdin is now fd[0]
                exit (1);
        }
        close (fd[1]);
        close (fd[0]);
        execvp(file, av);
        exit (1);
    }  
    if (type == 'r')
    {
        close(fd[1]); // Close unused write end
        return (fd[0]); // Return read end to read from child output
    }
    if (type == 'w')
    {
        close(fd[0]); // Close unused read end
        return (fd[1]); // Return write end to read from child output
    }
}  

void    ft_putstr(char *line)
{
    if (!line)
        return ;
    while (line)
    {
        write(1, line, 1);
        line++;
    }
    return ;
}

char    *join_char(char *str, char c, int len)
{
    char *new_str;
    int i = 0;

    new_str = (char *)malloc(len + 2);
    if (!new_str)
        return (NULL);
    while (i < len)
    {
        new_str[i] = str[i];
        i++;
    }
    new_str[i++] = c;
    new_str[i] = '\0';
    return (new_str);
}

char *get_next_line(int fd)
{
    static char buffer[500];
    static int buffer_real_size = 0;
    static int buffer_index = 0;
    int i = 0;
    char *new_line;

    if (fd < 0)
        return (NULL);
    new_line = (char *)malloc(1);
    if (!new_line)
        return (NULL);
    new_line[0] = '\0';
    while (1)
    {
        if (buffer_real_size <= buffer_index)
        {
            buffer_real_size = read(fd, buffer, 500);
            buffer_index = 0;
            if (buffer_real_size <= 0)
            {
                if (i == 0)
                {
                    free(new_line);
                    return (NULL);
                }
            }
            break;
        }
        new_line = join_char(new_line, buffer[buffer_index++], i);
        if (!new_line)
            return (NULL);
        if (new_line[i++] == '\n')
            break;
    }
    return (new_line);
}

/*
int main()
{
    int  fd;
    char *line;

    fd = ft_popen("ls", (char *const []){"ls", NULL}, 'r');
    while ((line = get_next_line(fd)))
    {
        ft_putstr(line);
        free(line);
    }
    close (fd);
    return (0);
}
*/

int	main() {
	int	fd_ls = ft_popen("ls", (char *const []){"ls", NULL}, 'r');

	int fd_grep = ft_popen("grep", (char *const []){"grep", "a", NULL}, 'w');
	char	*line;
	while ((line = get_next_line(fd_ls)))
	{
        write (fd_grep, line, strlen(line));
        free (line);
    }
    close (fd_ls);
    close (fd_grep);
}
