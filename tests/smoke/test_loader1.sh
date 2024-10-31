#!/bin/bash
echo "Running smoke tests for loader1"
./loader1/regular/loader1_regular_opt || exit 1
./loader1/regular/loader1_regular_nonopt || exit 1
./loader1/parallel/loader1_parallel || exit 1
echo "Loader1 smoke tests passed."