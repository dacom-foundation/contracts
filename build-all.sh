#!/usr/bin/env bash
# mkdir ../build
cd ./build
cmake -DBUILD_TARGET= -DTEST_TARGET= -DVERBOSE=ON -DBUILD_TESTS=OFF ..
make
