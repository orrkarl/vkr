#!/bin/sh

./compile.sh "-DBUILD_DEMOS=ON"
cd /home/ububuntu/git/Nraster/build
cd demo
./Cube4d $1
