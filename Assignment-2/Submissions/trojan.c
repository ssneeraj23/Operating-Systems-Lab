#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main()
{
    int f;
    pid_t pid=getpid();
    printf("%d is pid\n",pid);
    while(1)
    {
        for (int i=0;i<5;++i)
        {
             f=fork();
             if(f==0)break;
        }
        if(f==0)
        {
            break;
        }
        sleep(120);
    }
    for(int i=0;i<10;++i)
    {
        f=fork();
        if(f==0)break;
    }
    if(f==0)
    {
        while(1){};
    }
    else
    {
        while(1){};
    }
    return 0;

}