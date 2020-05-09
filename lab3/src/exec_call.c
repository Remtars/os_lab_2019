#include <stdio.h>
#include <unistd.h>
int main(int argc, char **argv)
{
    pid_t child = fork();
    
    if(child == 0)
    {
        execl("sequential_min_max", " ", argv[1], argv[2], NULL);
    }
    else
    {
        return 1;
    }
}