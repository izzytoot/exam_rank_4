/* 
Assignment name  : ft_popen
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
This exercise is inspired by the libc's popen(). 
*/

#include <unistd.h>
#include <stdlib.h>

int    ft_popen(const char *file, char *const av[], char type)
{
    int fd[2]; //pipe - fd[0] for input/read and fd[1] for output/write
    int pid; //to fork
 
    if (!file || !av || (type != 'r' && type != 'w')) //parse
        return (-1);
    if (pipe(fd) == -1) //perform pipe with fd[0] being read end and fd[1] being write end. if error...
        return (-1);
    pid = fork(); // fork child
    if (pid == -1) // if error...
    {
        close(fd[0]); 
        close(fd[1]);
        return (-1);
    }
    if (pid == 0) //childprocess
    {
        if (type == 'r') // child will write into pipe so parent can read
        {
            if (dup2(fd[1], 1) == -1) // stdout is now redirected to fd[1]
                exit (1); // if error
        }    
        if (type == 'w') // child will read from pipe (written from parent)
        {
            if (dup2(fd[0], 0) == -1) //stdin is now redirected to fd[0]
                exit (1); // if error
        }
        close (fd[1]); // close fd's so there aren't leaks
        close (fd[0]); // close fd's so there aren't leaks
        execvp(file, av); // execute file and av into/from fd[1]/fd[0]
        exit (1); // in case execvp fails, exit
    }  
    if (type == 'r') // we want to read from the child's output
    {
        close(fd[1]); // Close unused write end
        return (fd[0]); // Return read end to read from child output
    }
    else // we want to write to the child's input
    {
        close(fd[0]); // Close unused read end
        return (fd[1]); // Return write end to write to child's input
    }
}  

int main ()
{
    int fd_ls;
    int fd_grep;
    char buffer[1024];
    int bytes_read;

    fd_ls = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
    fd_grep = ft_popen("grep", (char *const[]){"grep", "a", NULL}, 'w');
    if (fd_ls == -1 || fd_grep == -1)
        return (1);
    while ((bytes_read = read(fd_ls, buffer, sizeof(buffer))) > 0)
        write(fd_grep, buffer, bytes_read);
    close(fd_grep);
    close(fd_ls);
    return (0);
}
