#!/bin/bash
echo "Running smoke tests for shell"
echo "exit" | ./shell/my_shell || exit 1
echo "Shell smoke test passed."