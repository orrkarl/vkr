#!/bin/sh

cd /home/ububuntu/git/Nraster/build
./compile.sh
cd test
./NrasterTest --gtest_filter=$1
