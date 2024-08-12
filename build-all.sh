#!/usr/bin/env bash
mkdir ../build
cd ../build
cmake -DBUILD_TARGET= -DTEST_TARGET= -DVERBOSE=ON -DIS_TESTNET=ON ..
make