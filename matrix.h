//
// Created by Леонид Шайхутдинов on 22.02.2023.
//

#ifndef MPI_LAB1_MATRIX_H
#define MPI_LAB1_MATRIX_H

void scanf_array(double *array, int n);
void scanf_square_matrix(double **matrix, int n);
void print_array(double *array, int n);
void scanf_matrix(double** matrix, int n, int k);
void print_square_Matrix(double **matrix, int n);
void print_matrix(double** matrix, int n, int k);
double mult_vector_by_vector(double *row, double *column, int n);
double* vectors_sub(double *vector1, double const *vector2, int n);
double* mult_matrix_by_vector(double **matrix, double *vector, int n);
double* mult_vector_by_num(double *vector, double num, int n);
double **malloc_matrix(int n, int k);
double **malloc_square_matrix(int n);
double** mult_matrix_by_matrix(double** matrix1, int n, int k, double** matrix2);
void free_matrix(double** matrix);
double** transpose_matrix(double** matrix, int n, int k);
#endif //MPI_LAB1_MATRIX_H
