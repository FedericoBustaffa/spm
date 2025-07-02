#!/bin/bash

for s in "static" "dynamic"; do
    echo "${s} schedule"
    for i in 1 2 4 8 16 32; do
        export OMP_NUM_THREADS=$i
        export OMP_SCHEDULE=$s
        ./run.out $1
    done
done
