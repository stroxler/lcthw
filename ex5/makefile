CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe

run: exe
	./exe

clean:
	rm -rf exe
