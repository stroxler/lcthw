# ?= in a makefile defines if not already part of ENV
PREFIX?=/usr/local
# -I{path/to/dir} says look here for .h files
CFLAGS=-g -Wall -I${PREFIX}/apr/include/apr-1 -I${PREFIX}/apr/include/apr-util-1

# -L{/path/to/dir} says look here for .so files
# -l{libname} says try to link to a file named {libname}lib.so inside one
#   of the link files.
# Note that on some systems, -L and -l aren't enough; you also have to modify
# the system by modifying /etc/ld.conf.so.d and running ldconfig.
# ...
# I don't know what -pthread does. I googled a while and found nothing.
# ...
# the make doesn't actually work on my ubuntu machine. See the notes file I
# added for discussion of what does work.
# ...
# The -pthread option, which you can grep for in the gcc man, says to compile
# with thread support. I don't know if it's necessary; I successfully compiled
# and it seemed to be working without this flag on my machine.
LDFLAGS=-L${PREFIX}/apr/lib -lapr-1 -laprutil-1 -pthread

all: clean devpkg

devpkg: bstrlib.o shell.o commands.o db.o

# if DESTDIR isn't defined in your env, it will evaluate to ""
#
# read the install man page. The first command below ensures that the bin
# directory exists; the second one drops the devpkg executable into it.
install: all
	install -d $(DESTDIR)/$(PREFIX)/bin
	install devpkg $(DESTDIR)/$(PREFIX)/bin

clean:
	rm -rf *.o
	rm -rf devpkg
	rm -rf *.dSYM  # I'm not sure what this is...
