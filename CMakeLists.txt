cmake_minimum_required(VERSION 3.24)
project(mpi_lab_3 C)

set(CMAKE_C_STANDARD 11)

add_executable(mpi_lab_3 main.c linear_matrix.c mem_ops.c)
