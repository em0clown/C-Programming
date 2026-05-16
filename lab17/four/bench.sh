#!/bin/bash
gcc matrix_bench.c -o matrix_bench -lpthread -O3
echo "Threads,Size,Time_ms" > results.csv

for t in 1 2 4 8 16 32 64 128
do
    for size in 500 1000 1500 2000
    do
        ./matrix_bench $size $t >> results.csv
    done
done
echo "Готово! Данные в results.csv"