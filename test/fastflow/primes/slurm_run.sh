#!/bin/bash


for i in 1 2 4 8 16 32; do
    srun ./run.out $1 $i
done
