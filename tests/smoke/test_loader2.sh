#!/bin/bash
echo "Running smoke tests for loader2"
./loader2/regular/loader2_regular_opt || exit 1
./loader2/regular/loader2_regular_nonopt || exit 1
./loader2/parallel/loader2_parallel || exit 1
echo "Loader2 smoke tests passed."