
CFLAGS = 

echo: echo.c server.o
	gcc $(CFLAGS) -o echopipe echo.c server.o -lpthread

server: server.c 
	gcc $(CFLAGS) -c -o server.o server.c -lpthread

clean:
	rm *.o
