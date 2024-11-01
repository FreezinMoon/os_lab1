#!/bin/bash
echo "Running smoke tests for loader1"
./loader1/regular/loader1_regular_opt Makefile 5|| exit 1
./loader1/regular/loader1_regular_nonopt Makefile 5|| exit 1
./loader1/parallel/loader1_parallel Makefile 5 5|| exit 1
echo "Loader1 smoke tests passed."