CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe exe.c

run: exe
	./exe thisfiledoesntexist.junk

clean:
	rm -rf exe
