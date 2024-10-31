#!/bin/bash
echo "Running smoke tests for shell"
./shell/shell || exit 1
echo "Shell smoke test passed."