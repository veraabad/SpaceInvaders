#!/usr/bin/env bash

if [ ! -d build ]
then
    mkdir -p build
else
    rm -rf build/*
fi

cd build

cmake ../

cp compile_commands.json ../compile_commands.json

make

./SpaceInvaders
