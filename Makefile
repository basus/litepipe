
CFLAGS = 

echo: echo.c server.o
	gcc $(CFLAGS) echo.c server.o -lpthread

server: server.c 
	gcc $(CFLAGS) -c -o server.o server.c -lpthread

clean:
	rm *.o
	./make_client_clean.sh
	rm litepipe
	rm litepipe_cl
	rm *~