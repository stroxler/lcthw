CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe my grind arguments

run: exe
	./exe my awesome run arguments

clean:
	rm -rf exe
