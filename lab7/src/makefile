all : tcpclient tcpserver udpclient udpserver

tcpclient : tcpclient.c
	gcc -o tcpclient tcpclient.c -std=gnu99

tcpserver : tcpserver.c
	gcc -o tcpserver tcpserver.c -std=gnu99
	
udpclient : udpclient.c
	gcc -o udpclient udpclient.c -std=gnu99

udpserver : udpserver.c
	gcc -o udpserver udpserver.c -std=gnu99

clean :
	rm -f tcpclient tcpserver udpclient udpserver