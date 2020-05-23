#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

int res1, res2;

void *Function1()
{
    printf("Function 1\n");

    pthread_mutex_lock(&lock1);
    printf("Function 1 locked Lock 1\n");

    printf("Function 1 is using Resource 1\n");
    
    sleep(1);
    res1++;

    pthread_mutex_lock(&lock2);
    printf("Function 1 locked Lock 2\n");

    printf("Function 1 is using Resource 2\n");

    sleep(1);
    res2++;

    pthread_mutex_unlock(&lock2);
    printf("Function 1 unlocked Lock 2\n");

    pthread_mutex_unlock(&lock1);
    printf("Function 1 unlocked Lock 1\n");
       
    printf("Funciton 1 stopped\n");
    return 0;
} 

void *Function2()
{
    printf("Function 2\n");

    pthread_mutex_lock(&lock2);
    printf("Function 2 locked Lock 2\n");

    printf("Function 2 is using Resource 2\n");
    
    sleep(1);
    res2++;
    
    pthread_mutex_lock(&lock1);
    printf("Function 2 locked Lock 1\n");

    printf("Function 2 is using Resource 2\n");
    
    sleep(1);
    res1++;

    pthread_mutex_unlock(&lock1);
    printf("Function 2 unlocked Lock 1\n");

    pthread_mutex_unlock(&lock2);
    printf("Function 2 unlocked Lock 2\n");
       
    printf("Function 2 stopped\n");
    return 0;
} 
  
int main()
{
    pthread_t thread1,thread2;
    pthread_create(&thread1, NULL, Function1, NULL);
    pthread_create(&thread2, NULL, Function2, NULL);

    pthread_join(thread1,NULL);
	pthread_join(thread2,NULL);

    printf("End of programm\nResources: R1 = %d, R2 = %d", res1, res2); // этого не случится, как и всех анлоков
}