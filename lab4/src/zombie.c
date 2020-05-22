#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    int pid = fork();
    if (pid == 0) 
    {   // child
        exit(0); 
    } 
    else 
    {   // parent
        sleep(10);
        printf("%d was a zombie\n", pid);
    }
}