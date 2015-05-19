set -e
cd /tmp

# get and install apr-1.4.6

curl -L -O http://archive.apache.org/dist/apr/apr-1.4.6.tar.gz

tar -xzvf apr-1.4.6.tar.gz
cd apr-1.4.6

./configure
make
make install

cd /tmp
rm -rf apr-1.4.6 apr-1.4.6.tar.gz

# get and install apr-util-1.4.1

curl -L -O http://archive.apache.org/dist/apr/apr-util-1.4.1.tar.gz

tar -xzvf apr-util-1.4.1.tar.gz
cd apr-util-1.4.1

./configure --with-apr=/usr/local/apr # for some reason apr-util can't find it
make
make install

cd /tmp
rm -rf apr-util-1.4.1 apr-util-1.4.1.tar.gz
