//
// Created by Леонид Шайхутдинов on 03.04.2023.
//

#ifndef MPI_LAB_3_LINEAR_MATRIX_H
#define MPI_LAB_3_LINEAR_MATRIX_H

void scan_linear_matrix(double* matrix, int n, int k);
void print_array(double* array, int n);
double mult_linear_matrix_vectors(double* horizontal_vec, double* vertical_vec, int n);
double* mult_matrix(double* matrix1, double* matrix2, int n1, int n2, int n3);

#endif //MPI_LAB_3_LINEAR_MATRIX_H
