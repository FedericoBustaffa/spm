#!/bin/bash


for i in 2048 4096 8192 16384 32768 65536; do
    srun ./sequential.out $1 $i
done
