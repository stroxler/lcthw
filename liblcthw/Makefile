# define OPTLIBS and OPTFLAGS as needed; in particular, add
# -I flags to OPTCFLAGS, -L flags to OPTLDFLAGS, and -l flags
# to OPTLDIBS.
#
# The CC, CFLAGS, LDFLAGS, and LDLIBS are all called "implicit variables"
CC=gcc
CFLAGS=-g -O2 -Wall -Wextra -Isrc -rdynamic -DNDEBUG $(OPTCFLAGS)
LDFLAGS=$(OPTLDFLAGS)
LDLIBS=-ldl $(OPTLDLIB)
PREFIX?=/usr/local

SOURCES=$(wildcard src/*.c src/**/*.c)
OBJECTS=$(patsubst %.c,%.o,$(SOURCES))

TEST_SRC=$(wildcard tests/*_tests.c)
TESTS=$(patsubst %.c,%,$(TEST_SRC))

MY_LIB=lcthw
TARGET=build/lib$(MY_LIB).a
SO_TARGET=$(patsubst %.a,%.so,$(TARGET))

# the target build
all: clean check $(TARGET) $(SO_TARGET) tests

# this looks weird, but I guess if you want to redefine a variable in a make
# target, then you do it by just adding an extra line(?)
dev: CFLAGS=-g -Wall -Isrc -Wall -Wextra $(OPTFLAGS)
dev: all

# -fPIC makes 'position independent code', which affects linkage somehow.
# In a makefile, the $@ refers to the make target (so in this case, $(TARGET))
# ar is the archive command, rcs are options. You can grep for them in the man.
# the ranlib is supposedly no longer necessary, but it finalizes some stuff.
#
# Note that it appears you don't need library flags to create a static library
# that depends on other static libraries. A .a file doesn't involve linking,
# it's actually just a thin wrapper around a bunch of .o files. If you have
# any dependencies on other static libraries, it appears that they simply
# need to be documented, and then your end users need to make sure to include
# all the dependencies when they link their executables ('users' here includes
# us in testing!)
$(TARGET): CFLAGS += -fPIC
$(TARGET): build $(OBJECTS)
	ar rcs $@ $(OBJECTS)
	ranlib $@

$(SO_TARGET): $(TARGET) $(OBJECTS)
	$(CC) -shared -o $@ $(OBJECTS)

# I *think* that @mkdir is a platform-independent mkdir -p
build: 
	@mkdir -p build
	@mkdir -p bin

.PHONY: tests
#tests: CFLAGS += $(TARGET)   .... this doesn't seem to do anything
# for tests...
#   force static linking
#   add build to the -L flags, and lctw to -l flags
tests: CFLAGS += -static
tests: LDFLAGS += -L./build
tests: LDLIBS += -l$(MY_LIB)
tests: $(TESTS)
	sh ./tests/runtests.sh


valgrind:
	VALGRIND="valgrind --log-file=/tmp/valgrind-%p.log" $(MAKE)

# The Cleaner
clean:
	rm -rf build $(OBJECTS) $(TESTS)
	rm -f tests/tests.log
	find . -name "*.gc*" -exec rm {} \;
	rm -rf `find . -name "*.dSYM" -print`

# The Install
install:
	install -d $(DESTDIR)/$(PREFIX)/lib/
	install $(TARGET) $(DESTDIR)/$(PREFIX)/lib/

# The Checker
BADFUNCS='[^_.>a-zA-Z0-9](str(n?cpy|n?cat|xfrm|n?dup|str|pbrk|tok|_)|stpn?cpy|a?sn?printf|byte_)'
check:
	@echo Files with potentially dangerous functions.
	@egrep $(BADFUNCS) $(SOURCES) || true
