CFLAGS=-g -Wall

all: clean grind

grind: exe
	echo "Starting run ======="
	valgrind ./exe

run: exe
	./exe

clean:
	rm -rf exe

example: clean exe
	./exe mydb create
	./exe mydb s 6 person1 person1@gmail.com
	./exe mydb s 8 person2 person2@msn.com
	./exe mydb s 4 person3 person3@yahoo.com
	./exe mydb l
	echo "---------"
	./exe mydb d 8
	./exe mydb g 4
	./exe mydb s 11 person4 person4@terminal.com
	./exe mydb l
	rm mydb
