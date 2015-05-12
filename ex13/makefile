CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe MybIglongargwithLOtsofVowels

run: exe
	./exe 'Here is an argument with spaces, treated as one token by bash'

clean:
	rm -rf exe
