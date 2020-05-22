#define _POSIX_SOURCE
#define _BSD_SOURCE
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <string.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

void kill_children(int sig)
{
    if (kill(0, SIGKILL)==0)
    {
        printf("Killed. \n");
    }
    else
    {
        printf ("Not killed. \n");
    }
}

int main(int argc, char **argv) 
{
    int seed = -1;
    int array_size = -1;
    int pnum = -1;
    int timeout = 0;
    bool by_files = false;

    while (true) {
        int current_optind = optind ? optind : 1;

        static struct option options[] = {{"seed", required_argument, 0, 0},
                                        {"array_size", required_argument, 0, 0},
                                        {"pnum", required_argument, 0, 0},
                                        {"timeout", required_argument, 0, 0},
                                        {"by_files", no_argument, 0, 'f'},
                                        {0, 0, 0, 0}};

        int option_index = 0;
        int c = getopt_long(argc, argv, "f", options, &option_index);

        if (c == -1) break;

        switch (c) {
        case 0:
            switch (option_index) {
            case 0:
                seed = atoi(optarg);
                if (seed <= 0) 
                {
                    printf("seed is a positive number\n");
                    return 1;
                }
                // error handling
                break;
            
            case 1:
                array_size = atoi(optarg);
                if (array_size <= 0)
                {
                    printf("array_size is a positive number\n");
                    return 1;
                }
                // error handling
                break;
            
            case 2:
                pnum = atoi(optarg);
                if (pnum <= 0)
                {
                    printf("pnum is a positive number\n");
                    return 1;
                }
                // error handling
                break;
            
            case 3:
                timeout = atoi(optarg);
                if (timeout < 0)
                {
                    printf ("timeout is not a negative number\n");
                    return 1;
                }
                break;  
            
            case 4:
                by_files = true;
                return 1;

            defalut:
                printf("Index %d is out of options\n", option_index);
            }
            break;
        case 'f':
            by_files = true;
            break;

        case '?':
            break;

        default:
            printf("getopt returned character code 0%o?\n", c);
        }
    }

    if (optind < argc) {
        printf("Has at least one no option argument\n");
        return 1;
    }

    if (seed == -1 || array_size == -1 || pnum == -1) {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" --timeout \"num\" --by_files (optional)\n",
            argv[0]);
        return 1;
    }

    int *array = malloc(sizeof(int) * array_size);
    GenerateArray(array, array_size, seed);
    int active_child_processes = 0;

    struct timeval start_time;
    gettimeofday(&start_time, NULL);

    int step = array_size / pnum;
    unsigned int begin, end;

    printf("Timeout: %d \n", timeout);
    if (timeout > 0)
    {
        alarm(timeout);
        signal(SIGALRM, kill_children);
    }

    //for (int i=0; i<array_size; i++) printf("%d ", array[i]);

    int pipe_fd[2];
    if (pipe(pipe_fd) < 0)
    {
        printf("Cannot open pipe\n");
        return 1;
    }

    for (int i = 0; i < pnum; i++) {
        printf("Process #%d\n", i);

        pid_t child_pid = fork();
        if (child_pid >= 0) 
        {
            // successful fork
            active_child_processes += 1;
            
            if (child_pid == 0) 
            {
                // child process
                // parallel somehow

                if (active_child_processes < pnum)
                {
                    begin = step * (active_child_processes - 1);
                    end = step * active_child_processes;
                }
                else
                {
                    begin = step * (pnum - 1);
                    end = array_size;
                }

                struct MinMax min_max_child = GetMinMax(array, begin, end);

                if (by_files) 
                {
                    // use files here
                    FILE *fp;
                    char fname[32];

                    sprintf(fname, "%d", active_child_processes);
                    
                    if ((fp = fopen(fname, "w")) == NULL)
                    {
                        printf("Cannot open file %s\n", fname);
                        exit(1);
                    }

                    fprintf(fp, "%d %d\n", min_max_child.min, min_max_child.max);

                    if (fclose(fp) != 0)
                    {
                        printf("Cannot close file %s\n", fname);
                        exit(1);
                    }
                } 
                else 
                {
                    // use pipe here
                    close(pipe_fd[0]);
/*
                    char min[16], max[16];
                    sprintf(min, "%d", min_max_child.min);
                    sprintf(max, "%d", min_max_child.max);

                    write(pipe_fd[1], max, strnlen(max, 16));
                    write(pipe_fd[1], " ", 1);
                    write(pipe_fd[1], min, strnlen(min, 16));
                    write(pipe_fd[1], "\n", 1);
*/
                    write(pipe_fd[1],&min_max_child,sizeof(struct MinMax));
                    close(pipe_fd[1]);
                    exit(1);
                }
                
                return 0;
            }

        } else {
        printf("Fork failed!\n");
        return 1;
        }
    }

    while (active_child_processes > 0) 
    {
        wait(NULL);
        active_child_processes -= 1;
    }

    struct MinMax min_max;
    min_max.min = INT_MAX;
    min_max.max = INT_MIN;

    for (int i = 0; i < pnum; i++) 
    {
        int min = INT_MAX;
        int max = INT_MIN;

        if (by_files) 
        {
            // read from files
            FILE *fp;
            char fname[32];
            sprintf(fname, "%d", i+1);
            
            if ((fp = fopen(fname, "r")) == NULL)
            {
                printf("Cannot open file %s\n", fname);
                return 1;
            }

            fscanf(fp, "%d %d", &min, &max);
            printf("File: %s, min: %d, max: %d\n", fname, min, max); //DEBUG
            
            if (fclose(fp) != 0)
            {
                printf("Cannot close file %s\n", fname);
                return 1;
            }

            if (remove(fname) != 0)
            {
                printf("Cannot delete file %s\n", fname);
                return 1;
            }
        } 
        else 
        {
            // read from pipes
            close(pipe_fd[1]);

            struct MinMax read_min_max;
            read(pipe_fd[0], &read_min_max, sizeof(struct MinMax));
            
            min = read_min_max.min;
            max = read_min_max.max;
            printf("i: %d, min: %d, max: %d\n", i+1, min, max);
        }

        if (min < min_max.min) min_max.min = min;
        if (max > min_max.max) min_max.max = max;
    }

    struct timeval finish_time;
    gettimeofday(&finish_time, NULL);

    double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
    elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

    free(array);

    printf("Min: %d\n", min_max.min);
    printf("Max: %d\n", min_max.max);
    printf("Elapsed time: %fms\n", elapsed_time);
    fflush(NULL);
    return 0;
}