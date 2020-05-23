#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <getopt.h>
#include <stdbool.h>
#include <pthread.h>

#include "utils.h"
#include "sum.h"

void *ThreadSum(void *args) 
{
    struct SumArgs *sum_args = (struct SumArgs *)args;
    return (void *)(size_t)Sum(sum_args);
}

int main(int argc, char **argv) {
    uint32_t threads_num = -1;
    uint32_t array_size = -1;
    uint32_t seed = -1;

    static struct option options[] = 	{{"seed", required_argument, 0, 0},
                                        {"array_size", required_argument, 0, 0},
                                        {"threads_num", required_argument, 0, 0},
                                        {0, 0, 0, 0}};

    int option_index = 0;
	
    while(true)
	{
	    int c = getopt_long(argc, argv, "f", options, &option_index);
		
        if (c == -1) break;
		
        switch(c)
		{
		case 0:
			switch(option_index)
			{
				case 0:
					seed = atoi(optarg);
					if(seed < 0)
                    {
                        printf("seed is not a negative number\n");
                        return 1;
                    }
					break;
				case 1:
					array_size = atoi(optarg);
					if(array_size < 1)
                    {
                        printf("array_size is a positive number\n");
                        return 1;
                    }
					break;
				case 2:
					threads_num = atoi(optarg);
					if(threads_num < 1)
					{
                        printf("threads_num is a positive number\n");
                        return 1;
                    }
					break;
                default:
			        printf("Index %d is out of options\n", option_index);
			}
			break;
		default:
            printf("getopt returned character code 0%o?\n", c);
		}
	}

    if (seed == -1 || array_size == -1 || threads_num == -1) 
    {
        printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n", argv[0]);
        return 1;
    }

    pthread_t threads[threads_num];

    unsigned int modulo = 1000;
    int *array = malloc(sizeof(int) * array_size);
    GenerateArrayModulo(array, array_size, seed, modulo);
    PrintArray(array, array_size);

    struct timeval time_start;
    gettimeofday(&time_start, NULL);
  
    struct SumArgs args[threads_num];
    int step = array_size/threads_num;
    for (int i = 0; i < threads_num; i++)
    {
        args[i].array = array;
        args[i].begin = i * step;
        if (i == threads_num - 1)
        {
            args[i].end = array_size;
        }
        else 
        {
            args[i].end = (i + 1) * step;
        }
    }

    for (uint32_t i = 0; i < threads_num; i++) 
    {
        if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) 
        {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    int total_sum = 0;
    for (uint32_t i = 0; i < threads_num; i++) 
    {
        int sum = 0;
        pthread_join(threads[i], (void **)&sum);
        total_sum += sum;
    }

    struct timeval time_stop;
    gettimeofday(&time_stop, NULL);
    double elapsed_time = (time_stop.tv_sec - time_start.tv_sec) * 1000.0;
    elapsed_time += (time_stop.tv_usec - time_start.tv_usec) / 1000.0;

    free(array);
    printf("Sum: %d\nElapsed time: %f ms\n", total_sum, elapsed_time);
    return 0;
}
