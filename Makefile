litepipe: litepipe.c client.o server.o protocol.o communicator.o
	gcc -o litepipe litepipe.c client.o server.o protocol.o communicator.o -lpthread

server: server.c communicator.o protocol.o
	gcc -c server.c protocol.o communicator.o -lpthread

protocol: protocol.c communicator.o
	gcc -c protocol.c

comm: communicator.c 
	gcc -c communicator.c -lpthread

client: client.c communicator.o
	gcc -c client.c communicator.o -lpthread

clean: communicator.o client.o server.o
	rm *.o