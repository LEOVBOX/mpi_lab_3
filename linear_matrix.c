//
// Created by Леонид Шайхутдинов on 03.04.2023.
//

#include "mem_ops.h"
#include <stdio.h>

void scan_linear_matrix(double* matrix, int n, int k)
{
	double cur_num;
	for (int i = 0; i < n*k; i++)
	{
		scanf("%lf", &matrix[i]);
	}
}

double mult_linear_matrix_vectors(double* horizontal_vec, double* vertical_vec, int n2, int n3)
{
	double result = 0;
	double matrix1_elem = 0;
	double matrix2_elem = 0;
	for (int i = 0; i < n2; i++)
	{
		matrix1_elem = *(horizontal_vec + i);
		matrix2_elem = *(vertical_vec + i * n3);
		result += matrix1_elem * matrix2_elem;
	}
	return result;
}

double* mult_matrix(double* matrix1, double* matrix2, int n1, int n2, int n3)
{
	double* res_matrix = (double*)mallocs(sizeof(double) * n1 * n3);
	double cur_cell = 0;
	int cur_cell_ind = 0;
	for (int row = 0; row < n1 * n2; row += n2)
	{
		for (int col = 0; col < n3; col++)
		{
			cur_cell = mult_linear_matrix_vectors(&matrix1[row], &matrix2[col], n2, n3);
			res_matrix[cur_cell_ind] = cur_cell;
			cur_cell_ind++;
		}
	}
	return res_matrix;
}

void print_array(double* array, int n)
{
	for (int i = 0; i < n; i++)
	{
		printf("%lf ", array[i]);
	}
	printf("\n");
}