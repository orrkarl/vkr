#!/bin/sh

./compile.sh
cd /home/ububuntu/git/Nraster/build
cd test
./NrasterTest --gtest_filter=$1
