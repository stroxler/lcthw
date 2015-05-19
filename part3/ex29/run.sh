set -x

# clean up any old build
rm -f libex29.o libex29.so exe

# compile from .c to .o, then .o to .so
#  (note that Zed made a comment about -fPIC, and indeed we need it on ubuntu)
cc -c libex29.c -o libex29.o -fPIC
cc -shared -o libex29.so libex29.o

# compile exe from .c to executable.
# note the -ldl: we are linking to the dynamic linker library
cc -Wall -g -DNDEBUG exe.c -ldl -o exe

# give it a bad arg list, library, then a bad function
./exe ./libex29.so print_a_message
echo $?
./exe ./libdoesntexist print_a_message "Hello there"
echo $?
./exe ./libex29.so func_doesnt_exist "Hello there"
echo $?

# generate an error on purpose
./exe ./libex29.so i_fail_on_purpose "Hello there"
echo $?

# give it some normal inputs
./exe ./libex29.so print_a_message "Hello There"
echo $?
./exe ./libex29.so uppercase "Hello There"
echo $?
./exe ./libex29.so lowercase "Hello There"
echo $?
