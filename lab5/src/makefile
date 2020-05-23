all : without_mutex with_mutex factorial deadlock

deadlock : deadlock.c
	gcc -o deadlock deadlock.c -pthread

factorial : factorial.c
	gcc -o factorial factorial.c -pthread -std=gnu99

without_mutex : without_mutex.c
	gcc -o without_mutex without_mutex.c -pthread

with_mutex : with_mutex.c
	gcc -o with_mutex with_mutex.c -pthread

clean :
	rm with_mutex without_mutex factorial deadlock