#!/bin/sh

echo "cleaning up configure and aclocal.m4"
rm -f configure
rm -f aclocal.m4

echo "running aclocal..."
aclocal
echo "running autoconf..."
autoconf
echo "running automake..."
automake

./configure --enable-debug
make
