#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
cmake -B build -S . -DCMAKE_PREFIX_PATH="$(brew --prefix)"
cmake --build build -j
ctest --test-dir build --output-on-failure "$@"