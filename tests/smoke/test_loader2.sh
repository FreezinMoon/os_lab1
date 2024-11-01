#!/bin/bash
echo "Running smoke tests for loader2"
./loader2/regular/loader2_regular_opt 10|| exit 1
./loader2/regular/loader2_regular_nonopt 10|| exit 1
./loader2/parallel/loader2_parallel 10|| exit 1
echo "Loader2 smoke tests passed."