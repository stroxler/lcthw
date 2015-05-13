# this code actually doesn't run because there's a glibc dependency
# issue; I'm too lazy to solve it so for now just run
#  apt-get install -y valgrind

# set -e makes it so an error in your script will stop executuion
# set -x makes your script echo commands (after substituting vars, if any)
set -e
set -x

# by default curl uses stdout. -O says to use an output file the same as
# the end of the url path. -L says to follow any 301 moved permanently
# redirects; curl -L -O is pretty similar to wget.
curl -L -O http://valgrind.org/downloads/valgrind-3.6.1.tar.bz2

# I'm pretty sure this is something you can always do with .tar.bz2 files
md5sum valgrind-3.6.1.tar.bz2

# -x = extract; -j = use bzip (z is gzip); -v = verbose;
# -f = use this file (for input, instead of stdin)
tar -xjvf valgrind-3.6.1.tar.bz2

# make.
#   - ./configure inspects your computer and sets env variables suitably
#   - make builds the code
#   - make install (needs sudo usually) installs the build to wherever your
#     system prefers.
cd valgrind-3.6.1
./configure
make
make install

# cleanup
cd ..
rm -rf valgrind-3.6.1
rm valgrind-3.6.1.tar.bz2
