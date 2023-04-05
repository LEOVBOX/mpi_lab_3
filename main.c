#include <stdio.h>
#include "linear_matrix.h"
#include "mem_ops.h"


int main()
{
	printf("Введите n1, n2, n3 через пробел\n");
	int n1, n2, n3;
	scanf("%d %d %d", &n1, &n2, &n3);

	double* matrix1 = (double*)mallocs(n1 * n2 * sizeof(double));
	double* matrix2 = (double*)mallocs(n2 * n3 * sizeof(double));

	scan_linear_matrix(matrix1, n1, n2);
	print_array(matrix1, n1 * n2);
	printf("\n");

	scan_linear_matrix(matrix2, n2, n3);
	print_array(matrix2, n2 * n3);

	double* res = mult_matrix(matrix1, matrix2, n1, n2, n3);

	print_array(res, n1 * n3);

	return 0;
}
