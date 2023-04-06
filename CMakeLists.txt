cmake_minimum_required(VERSION 3.24)
project(mpi_lab_3 C)

set(CMAKE_C_STANDARD 11)
include_directories(/opt/homebrew/Cellar/mpich/4.1/include)
set(CMAKE_C_COMPILER "/opt/homebrew/bin/mpicc")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/mpic++")

add_executable(mpi_lab_3 main.c linear_matrix.c mem_ops.c)
