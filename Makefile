server: server.c communicator.o client.o
	gcc -o server server.c communicator.o client.o -lpthread

comm: communicator.c 
	gcc -c communicator.c -lpthread

client: client.c communicator.o
	gcc -c client.c communicator.o -lpthread

clean: communicator.o client.o server.o
	rm *.o