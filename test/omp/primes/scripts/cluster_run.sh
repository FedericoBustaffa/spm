#!/bin/bash

for s in "static" "dynamic"; do
    echo "${s} schedule"
    for i in 1 2 4 8 16 32; do
        export OMP_NUM_THREADS=$i
        export OMP_SCHEDULE=$s
        srun --nodes=1 --tasks-per-node=1 --cpus-per-task=$i \
            --time=00:05:00 ./run.out $1
    done
done
