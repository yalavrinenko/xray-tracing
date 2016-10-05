#!/bin/sh

for x in *.cpp; do gcc -g -fopenmp -fPIC -c -std=c++11 -w $x -o build/$x.o; done;g++ -g -fopenmp -shared -o ./build/libRaytrace.so ./build/*.o
rm build/*.o
