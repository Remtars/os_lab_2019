CC=gcc
CFLAGS=-I.

all : parallel_min_max process_memory zombie parallel_sum

parallel_min_max : utils.o utils.h find_min_max.o find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c -std=gnu99

process_memory : process_memory.c
	$(CC) -o process_memory process_memory.c

zombie : zombie.c
	$(CC) -o zombie zombie.c

parallel_sum : utils.o utils.h sum.o parallel_sum.c 
	$(CC) -o parallel_sum sum.o utils.o parallel_sum.c -pthread -std=gnu99

sum.o : sum.h
	$(CC) -o sum.o -c sum.c -std=gnu99

utils.o : utils.h
	$(CC) -o utils.o -c utils.c -std=gnu99

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c -std=gnu99

clean :
	rm utils.o find_min_max.o sum.o parallel_min_max process_memory parallel_sum zombie
