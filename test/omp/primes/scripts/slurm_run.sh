#!/bin/bash


mkdir -p logs

threads=(1 2 4 8 16 32)

for t in ${threads[@]}; do
    export OMP_NUM_THREADS=$t
    for i in {1..10}; do
        echo "repetition $i with $t threads"
        ./run.out $1 > ./logs/output_t${t}_r${i}.log
    done
done

