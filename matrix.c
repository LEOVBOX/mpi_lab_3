//
// Created by Леонид Шайхутдинов on 16.02.2023.
//

#include <stdio.h>
#include "mm_malloc.h"


void scanf_array(double *array, int n)
{
	for (int i = 0; i < n; i++)
	{
		scanf("%lf", &array[i]);
	}
}

void scanf_square_matrix(double** matrix, int n)
{
	for (int i = 0; i < n; i++)
	{
		scanf_array(matrix[i], n);
	}
}

void scanf_matrix(double** matrix, int n, int k)
{
	for (int i = 0; i < n; i++)
	{
		scanf_array(matrix[i], k);
	}
}

void print_array(double *array, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%f ", array[i]);
	}
	printf("\n");
}

void print_square_Matrix(double **matrix, int n)
{
	for (int i = 0; i < n; i++)
	{
		print_array(matrix[i], n);
	}
	printf("\n");
}

void print_matrix(double** matrix, int n, int k)
{
	for (int i = 0; i < n; i++)
	{
		print_array(matrix[i], k);
	}
}

double mult_vector_by_vector(double *row, double *column, int n)
{
	double result = 0;
	for (int i = 0; i < n; i++)
	{
		result += (row[i] * column[i]);
	}
	return result;
}

double* mult_matrix_by_vector(double **matrix, double *vector, int n)
{
	double* result = (double*)malloc(sizeof(double) * n);
	for (int i = 0; i < n; i++)
	{
		result[i] = mult_vector_by_vector(matrix[i], vector, n);
	}
	return result;
}



double* vectors_sub(double *vector1, double const *vector2, int n)
{
	double* result = vector1;
	for (int i = 0; i < n; i++)
	{
		result[i] = vector1[i] - vector2[i];
	}
	return result;
}

double* mult_vector_by_num(double *vector, double num, int n)
{
	double* result = vector;
	for (int i = 0; i < n; i++)
	{
		result[i] = vector[i] * num;
	}
	return result;
}

double **malloc_matrix(int n, int k)
{
	double** matrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++)
	{
		matrix[i] = (double*)malloc(sizeof(double*) * k);
	}
	return matrix;
}

double **malloc_square_matrix(int n)
{
	double** matrix = (double**)malloc(sizeof(double*) * n);
	for (int i = 0; i < n; i++)
	{
		matrix[i] = (double*)malloc(sizeof(double) * n);
	}
	return matrix;
}

/* Multiplication matrix1 by matrix2,
 * matrix2 is already transposed
 * n1 = n2, k1 = k2
*/
double** mult_matrix_by_matrix(double** matrix1, int n, int k, double** matrix2)
{
	double** result_matrix = malloc_matrix(n, n);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < n; j++)
		{
			result_matrix[i][j] = mult_vector_by_vector(matrix1[i], matrix2[j], k);
		}

	}
	return result_matrix;
}

double** transpose_matrix(double** matrix, int n, int k)
{
	double** result_matrix = malloc_matrix(k, n);
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < k; j++)
		{
			result_matrix[j][i] = matrix[i][j];
		}
	}
	return result_matrix;
}

void free_matrix(double** matrix, int n)
{
	for (int i = 0; i < n; i++)
	{
		free(matrix[i]);
	}
	free(matrix);
}

//double* paralleMultlMatrixByVector()