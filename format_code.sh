#!/usr/bin/env bash

# This script formats all C and header files in the src/ directory using clang-format-19.
find src/ -type f \( -name '*.c' -o -name '*.h' \) | xargs clang-format-19 -i