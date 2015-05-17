CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe 4 1 7 3 2 0 8


run: exe
	./exe -5 7 9 -3 -3 6 -3

clean:
	rm -rf exe
