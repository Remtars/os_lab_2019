CC=gcc
CFLAGS=-I.

all : sequential_min_max parallel_min_max exec_call

exec_call : exec_call.c utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o exec_call find_min_max.o utils.o exec_call.c $(CFLAGS)

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c -std=gnu99 $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c -std=gnu99 $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c -std=gnu99 $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c -std=gnu99 $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max exec_call
