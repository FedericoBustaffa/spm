#!/bin/bash


# run all simulations and save results
make -j 2>&1 | grep softmax_auto.cpp
for j in {128,256,512,1024,2048,4096,8192,16384}; do
    for i in {0..49}; do
        ./softmax_plain.out $j 1 1>> plain_times_$j.txt 2>> plain_res_$j.txt
        # ./softmax_auto.out $j 1 1>> auto_times_$j.txt 2>> auto_res_$j.txt
        ./softmax_avx.out $j 1 1>> avx_times_$j.txt 2>> avx_res_$j.txt
    done
done

RED="\e[31m"
GREEN="\e[32m"
RESET="\e[0m"

# check that every program compute the same result
function compare_results() {
    diff $1 $2 > /dev/null 2>&1
    if [[ $? != 0 ]]; then
        echo -e "${RED}$1 - $2: failed${RESET}"
    else
        echo -e "${GREEN}$1 - $2: passed${RESET}"
    fi
}

for j in {128,256,512,1024,2048,4096,8192,16384}; do
    # compare_results plain_res_$j.txt auto_res_$j.txt
    compare_results plain_res_$j.txt avx_res_$j.txt
    # compare_results auto_res_$j.txt avx_res_$j.txt
    echo "---------------"
done

# aggregate time files
python aggregate.py *_times_*.txt
# mv results/auto.csv "results/$1.csv"

# delete all sparse files
rm *.txt
