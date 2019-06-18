#!/bin/sh

./compile.sh "-DBUILD_DEMOS=ON"
cd /home/ububuntu/git/Nraster/build
cd demo
./Triangle3d
