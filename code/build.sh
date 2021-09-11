#!/bin/bash
# /bin/bash is the interpreter that understands the language we are
# using

mkdir -p ../build
pushd ../build
c++ ../code/sdl_manor.cpp -o manor -g `sdl2-config --cflags --libs` -lSDL2main -lSDL2 

# `sdl2-config --cflags --libs`
# backticks ` let bash know we want to
# run a program (sdl2-config) and include its results as part of the
# command.
# -o to name the output
# -g for debug options
popd
