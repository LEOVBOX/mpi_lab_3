#!/bin/bash
set -x
mpicc -std=gnu99 -Wpedantic -Wall -Werror main.c mem_ops.c parallel_matrix_mult.c my_math.c linear_matrix.c -o matrix_mult -lm
