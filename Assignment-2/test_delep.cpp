#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <term.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/file.h>

int main()
{
    pid_t child;
    int stat_loc;
    child = fork();
    if (child == 0)
    {
        int outfd;
        char buf[2];
        buf[0] = 'a';
        buf[1] = '\0';
        
        // Creating the file temp.txt
        outfd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0666);
        
        // Obtaining lock for the file using flock function
        int ret = flock(outfd, LOCK_EX); 

        // Writing to the file "temp.txt" in an infinite loop
        while (1)
        {
            write(outfd, buf, 2);
        }
        exit(0);

    }
    else
    {
        // Parent doesn't wait for the child to return while the child never releases the file "temp.txt".
        return 0;
    }
}