CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe

run: exe
	./exe

exe: exe.o ex22.o

clean:
	rm -f exe *.o
