#!/bin/sh

echo "cleaning up config files"
rm -f configure
rm -f aclocal.m4
rm -f Makefile.in Makefile
rm -f */Makefile */Makefile.in

echo "running aclocal..."
aclocal
echo "running autoheader..."
autoheader
echo "running autoconf..."
autoconf
echo "running automake..."
automake

./configure --enable-debug
make
