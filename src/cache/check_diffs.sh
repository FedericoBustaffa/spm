#!/bin/bash

c=$(find . -name "*.txt" -type f)

for file in *.txt; do
	if [ "$file" != "naive.txt" ]; then
		echo "$file"
		diff "naive.txt" "$file"
	fi
done