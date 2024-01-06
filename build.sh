#!/bin/bash
cmake CMakeLists.txt -B build -G Ninja
ninja -C build
cp -R assets/ build/