#include <stdio.h>
#include "linear_matrix.h"
#include "mem_ops.h"
#include <mpi.h>
#include <string.h>
#include "mm_malloc.h"

#define ROOT 0

int char_to_int(char a)
{
	if ((a > 57) || (a < 48))
		return 0;
	return a - '0';
}

int string_to_int(char* str)
{
	int result = 0;
	int multiplier = 1;
	int len = (int)strlen(str);
	for (int i = len-1; i > -1; i--)
	{
		result += char_to_int(str[i]) * multiplier;
		multiplier *= 10;
	}
	return result;
}

int read_input_file(char* file_path, double* matrix1, double* matrix2, int n1, int n2, int n3)
{
	FILE* fd = fopen(file_path, "r");
	if (fd == NULL)
	{
		printf("fopen(%s) failed\n", file_path);
		return -1;
	}

	fscan_linear_matrix(fd, matrix1, n1, n2);
	fgetc(fd);
	fscan_linear_matrix(fd, matrix2, n2, n3);

	fclose(fd);
	return 0;
}

void create_grid(int procs_num, int n1, int n2, int n3)
{

}

int main(int argc, char* argv[])
{

	int procs_num = 0;
	int rank = 0;
	MPI_Status status;

	// Инициализация MPI. Указываем сколько процессов должно создаться.
	int rc = MPI_Init(&argc, &argv);
	if (rc != MPI_SUCCESS)
	{
		printf("Mpi Initialisation error\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}

	// Получение числа инициализированных процессов.
	MPI_Comm_size(MPI_COMM_WORLD, &procs_num);
	// Получение номера данного процесса.
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);


	if (rank == ROOT)
	{
		if (argc < 3)
		{
			printf("Few args\n");
			return 1;
		}

		// Чтение параметров матриц
		int n1 = string_to_int(argv[argc - 3]);
		printf("n1 = %d\n", n1);
		int n2 = string_to_int(argv[argc - 2]);
		printf("n2 = %d\n", n2);
		int n3 = string_to_int(argv[argc - 1]);
		printf("n3 = %d\n", n3);


		double* matrix1 = (double*)mallocs(n1 * n2 * sizeof(double));
		double* matrix2 = (double*)mallocs(n2 * n3 * sizeof(double));

		printf("malloced\n");

		read_input_file("input.txt", matrix1, matrix2, n1, n2, n3);

		printf("file read\n");

		print_array(matrix1, n1 * n2);
		printf("\n");
		print_array(matrix2, n2 * n3);

		double* res = mult_matrix(matrix1, matrix2, n1, n2, n3);

		print_array(res, n1 * n3);

		free(matrix1);
		free(matrix2);
	}

	MPI_Finalize();

	return 0;
}
