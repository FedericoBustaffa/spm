#!/bin/bash


for i in "128MB" "256MB" "512MB" "1GB" "2GB" "3GB"; do
    ./sequential.out $1 $i | grep -i "time"
done
