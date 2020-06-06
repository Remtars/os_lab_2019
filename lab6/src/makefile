all : client server

client : MultModulo.o client.c 
	gcc -o client MultModulo.o client.c -lpthread -std=gnu99

server : MultModulo.o server.c
	gcc -o server MultModulo.o server.c -lpthread -std=gnu99

MultModulo.o : MultModulo.h MultModulo.c
	gcc -o MultModulo.o -c MultModulo.c

clean :
	rm client server MultModulo.o