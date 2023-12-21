TARGET: example.exe
example.exe: example.o gc.o
	gcc example.o gc.o -o example.exe
example.o: example.c gc.h
	gcc -c example.c -o example.o
gc.o: gc.c gc.h
	gcc -c gc.c -o gc.o
clean:
	rm example.o gc.o example.exe