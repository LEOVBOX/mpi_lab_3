//
// Created by Леонид Шайхутдинов on 03.04.2023.
//

#ifndef MPI_LAB_3_LINEAR_MATRIX_H
#define MPI_LAB_3_LINEAR_MATRIX_H

#include <stdio.h>

void scan_linear_matrix(double* matrix, int n, int k);
void print_array(double* array, int n);
double mult_linear_matrix_vectors(double* horizontal_vec, double* vertical_vec, int n);
double* mult_matrix(double* matrix_A, double* matrix_B, int N1, int N2, int N3);
void fscan_linear_matrix(FILE* fd, double* matrix, int n, int k);
void transpose_linear_matrix(double* matrix, int rows, int cols, double* result);
void print_matrix(double* matrix, int rows, int cols);
double* crt_default_matrix(int n, int k);


#endif //MPI_LAB_3_LINEAR_MATRIX_H
