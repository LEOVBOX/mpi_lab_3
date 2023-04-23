//
// Created by Леонид Шайхутдинов on 03.04.2023.
//

#include "mem_ops.h"
#include <stdio.h>

void print_array(double* array, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%lf ", array[i]);
	}
	printf("\n");
}


void scan_linear_matrix(double* matrix, int n, int k)
{
	for (int i = 0; i < n*k; i++)
	{
		scanf("%lf", &matrix[i]);
	}
}

void fscan_linear_matrix(FILE* fd, double* matrix, int n, int k)
{
	for (int i = 0; i < n*k; i++)
	{
		fscanf(fd, "%lf", &matrix[i]);
	}
}

void print_matrix(double* matrix, int rows, int cols)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			printf("%lf ", matrix[i * cols + j]);
		}
		printf("\n");
	}
}

void transpose_linear_matrix(double* matrix, int rows, int cols, double* result)
{
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			result[j * rows + i] = matrix[i * cols + j];
		}
	}
}

double mult_linear_matrix_vectors(double* horizontal_vec, double* vertical_vec, int n2)
{
	double result = 0;
	double matrix1_elem = 0;
	double matrix2_elem = 0;
	for (int i = 0; i < n2; i++)
	{
		matrix1_elem = *(horizontal_vec + i);
		matrix2_elem = *(vertical_vec + i);
		result += matrix1_elem * matrix2_elem;
	}
	return result;
}

double* mult_matrix(double* matrix_A, double* matrix_B, int N1, int N2, int N3)
{
	double* matrix_Res = (double*)mallocs(N1 * N3 * sizeof(double));
	for (int i = 0; i < N1; i++)
	{
		for (int j = 0; j < N3; j++)
		{
			int sum = 0;
			for (int k = 0; k < N2; k++)
			{
				sum += matrix_A[i * N2 + k] * matrix_B[j * N2 + k];
			}
			matrix_Res[i * N3 + j] = sum;
		}
	}
	return matrix_Res;
}


double* crt_default_matrix(int n, int k)
{
	double value = 1.0;
	double *A = (double*) malloc(n * k * sizeof(double));
	for (int i = 0; i < n * k; ++i) {
		A[i] = value;
		value += 1.0;
	}
	return A;
}