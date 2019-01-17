#!/bin/bash
apt-get update --yes
wget http://www.cmake.org/files/v3.10/cmake-3.10.1.tar.gz
tar -xvzf cmake-3.10.1.tar.gz
cd cmake-3.10.1/
./configure
make 
make install
apt-get install --yes libgtest-dev
cd /usr/src/gtest
cmake CMakeLists.txt
make
cp *.a /usr/lib
mkdir /usr/local/lib/googletest
ln -s /usr/lib/libgtest.a /usr/local/lib/googletest/libgtest.a
ln -s /usr/lib/libgtest_main.a /usr/local/lib/googletest/libgtest_main.a
apt-get update
apt-get install --yes ncurses-dev
apt install --yes libncurses5-dev libncursesw5-dev
apt-get install --yes valgrind
apt-get install --yes libpthread-stubs0-dev
