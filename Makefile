
CFLAGS = 

echo: echo.c litepipe.o
	gcc $(CFLAGS) -o echopipe echo.c litepipe.o -lpthread

lp_static: litepipe.o
	ar rcs liblitepipe.a litepipe.o

lp_shared:
	gcc $(CFLAGS) -c -fPIC litepipe.c -o litepipe.o
	gcc -shared -Wl,-soname,liblitepipe.so.1 -o liblitepipe.so.1.0.1  litepipe.o

litepipe: litepipe.c 
	gcc $(CFLAGS) -c -o litepipe.o litepipe.c -lpthread

clean: litepipe.o
	rm *.o
	rm lib*
