#!/usr/bin/env bash
ctest -j $(nproc) --rerun-failed --output-on-failure