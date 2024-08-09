#!/bin/bash

VERBOSE=""

if [[ $1 == "--verbose" ]]; then
  VERBOSE="--verbose"
fi

cd ../cpp/system/build/tests && ctest -j $(nproc) --output-on-failure $VERBOSE
