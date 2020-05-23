#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <getopt.h>
#include <stdbool.h>
#include <pthread.h>

uint64_t factorial = 1; // используется всеми потоками
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

struct FactArgs 
{
    uint32_t begin;
    uint32_t end;
    uint32_t modulo;
};

void Fact(const struct FactArgs *args) 
{
    uint64_t fact_part = 1;
    for(uint32_t i = args->begin; i <= args->end; i++)
    {
        fact_part *= (i % args->modulo);
    }
    pthread_mutex_lock(&mut);
    factorial *= fact_part;
    pthread_mutex_unlock(&mut);
}

void *ThreadFact(void *args) 
{   
    struct FactArgs *fact_args = (struct FactArgs *)args;
    Fact(fact_args);
    return 0;
}

int main(int argc, char **argv) {
    uint32_t k = -1;
    uint32_t pnum = -1;
    uint32_t mod = -1;

    static struct option options[] = 	{{"k", required_argument, 0, 0},
                                        {"pnum", required_argument, 0, 0},
                                        {"mod", required_argument, 0, 0},
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
					k = atoi(optarg);
					if(k < 1)
                    {
                        printf("k is a positive number\n");
                        return 1;
                    }
					break;
				case 1:
					pnum = atoi(optarg);
					if(pnum < 1)
                    {
                        printf("pnum is a positive number\n");
                        return 1;
                    }
					break;
				case 2:
					mod = atoi(optarg);
					if(mod < 1)
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

    if (k == -1 || pnum == -1 || mod == -1) 
    {
        printf("Usage: %s --k \"num\" --pnum \"num\" --mod \"num\" \n", argv[0]);
        return 1;
    }

    pthread_t threads[pnum];

    struct timeval time_start;
    gettimeofday(&time_start, NULL);
  
    struct FactArgs args[pnum];
    uint32_t step = k/pnum;
    for (int i = 0; i < pnum; i++)
    {
        args[i].modulo = mod;
        args[i].begin = i * step + 1;
        if (i == pnum - 1)
        {
            args[i].end = k;
        }
        else 
        {
            args[i].end = (i + 1) * step;
        }
    }

    for (uint32_t i = 0; i < pnum; i++) 
    {
        if (pthread_create(&threads[i], NULL, ThreadFact, (void *)&args[i])) 
        {
            printf("Error: pthread_create failed!\n");
            return 1;
        }
    }

    for (int i = 0; i < pnum; i++) 
    {
        if (pthread_join(threads[i], NULL) != 0)
        {
            printf("Error: pthread_join failed!\n");
            return 1;
        } 
    }

    struct timeval time_stop;
    gettimeofday(&time_stop, NULL);
    double elapsed_time = (time_stop.tv_sec - time_start.tv_sec) * 1000.0;
    elapsed_time += (time_stop.tv_usec - time_start.tv_usec) / 1000.0;

    printf("Factorial: %lu\nElapsed time: %f ms\n", factorial, elapsed_time);
    return 0;
}
