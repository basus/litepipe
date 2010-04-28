
CFLAGS = -g

litepipe: litepipe.c client.o server.o protocol.o communicator.o
	gcc $(CFLAGS) -o litepipe litepipe.c client.o server.o protocol.o communicator.o -lpthread

server: server.c communicator.o protocol.o
	gcc $(CFLAGS) -c server.c protocol.o communicator.o -lpthread

protocol: protocol.c communicator.o
	gcc $(CFLAGS) -c protocol.c

comm: communicator.c 
	gcc $(CFLAGS) -std=c99 -c communicator.c -lpthread

communicator.o: communicator.c
	gcc $(CFLAGS) -std=c99 -c -o communicator.o communicator.c -lpthread

client: client.c communicator.o
	gcc $(CFLAGS) -c client.c communicator.o -lpthread

clean:
	rm *.o *~
	cd litepipe_client
	make clean
	rm bin/*
	cd ..