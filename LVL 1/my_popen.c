/* Allowed functions: pipe, fork, dup2, execvp, close, exit

write the following function:

    int    ft_popen(const char file, charconst argv[], char type)

The function must launch the executable file with the arguments argv (using execvp).
If the type is 'r' the function must return a file descriptor connected to the output of the command.
If the type is 'w' the function must return a file descriptor connected to the input of the command.
In case of error or invalid parameter the function must return -1.

example:

int main() {
    int fd = ft_popen("ls", (char const[]){"ls", NULL}, 'r');

    charline;
    while(line = get_next_line(fd))
        ft_putstr(line);
}

Hint: Do not leak file descriptors! */

#include <unistd.h>
#include <stdlib.h>

int    ft_popen(const char file, char const *argv[], char type)
{
    int fd[2]; //pipe
    int pid;

    if (type != 'r' && type != 'w')
        return (-1);
    if (pipe(fd) == -1)
        return (-1);
    pid = fork ();
    if (pid == -1)
        return (-1);
    if (pid == 0) //childprocess
    {
        if (type == 'r') // Redirect stdout to pipe write end
            dup2(fd[1], 1); //stdout is now fd[1]
        if (type == 'w') // Redirect stdin to pipe read end
            dup2(fd[0], 0); //stdin is now fd[0]
        close (fd[1]);
        close (fd[0]);
        execvp(file, argv);
        exit (-1);
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

int main() 
{
    int fd = ft_popen("ls", (char const[]){"ls", NULL}, 'r');

    charline;
    while(line = get_next_line(fd))
        ft_putstr(line);
}
