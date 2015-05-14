CFLAGS=-g -Wall

grind: clean exe
	echo "Starting run ======="
	valgrind ./exe 4 1 7 3 2 0 8

exe: object.o

preprocess:
	gcc -E exe.c

clean:
	rm -f exe object.o
