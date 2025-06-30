#!/bin/bash




threads=(1 2 4 8 16 32)

for t in ${threads[@]}; do
    export OMP_NUM_THREADS=$THREADS
done

