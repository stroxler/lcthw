set -x

make clean exe

echo 'Expect too few...'
./exe

if [ $? -eq 0 ]; then
    echo "FAIL"
fi

echo 'Expect too many...'
./exe lots and lots of args

if [ $? -eq 0 ]; then
    echo "FAIL"
fi


echo 'Expect "my" and "args" echoed back...'
./exe my args

if [ $? -ne 0 ]; then
    echo "FAIL"
fi
