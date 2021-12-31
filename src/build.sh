#!/bin/bash

mkdir -p ../build
pushd ../build
cc ../src/sdl_manor.c -o manor -g `sdl2-config --cflags --libs` -lSDL2main -lSDL2

# `sdl2-config --cflags --libs`
# backticks ` let bash know we want to
# run a program (sdl2-config) and include its results as part of the
# command.
# -o to name the output
# -g for debug options
popd
