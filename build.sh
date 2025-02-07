#!/bin/bash

if [ -z "$1" ]; then
  echo "Usage: $0 <contract>"
  exit 1
fi

contract="$1"

cd ./build
cmake -DBUILD_TARGET="$contract" -DTEST_TARGET= -DVERBOSE=ON -DBUILD_TESTS=OFF .. && make
