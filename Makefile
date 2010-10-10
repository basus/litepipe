
CFLAGS = 

echo: echo.c litepipe.o
	gcc $(CFLAGS) -o echopipe echo.c litepipe.o -lpthread

litepipe: litepipe.c 
	gcc $(CFLAGS) -c -o litepipe.o litepipe.c -lpthread

clean:
	rm *.o
