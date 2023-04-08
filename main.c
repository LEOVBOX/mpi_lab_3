#include <stdio.h>
#include "linear_matrix.h"
#include "mem_ops.h"
#include <mpi.h>
#include <string.h>
#include <math.h>
#include "my_math.h"
#include "parallel_matrix_mult.h"

#define ROOT 0
#define X 1
#define Y 0

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

int main(int argc, char* argv[])
{

	int procs_num = 0;
	int rank = -1;
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


	// Чтение параметров матриц
	int n1 = string_to_int(argv[argc - 3]);
	//printf("n1 = %d\n", n1);
	int n2 = string_to_int(argv[argc - 2]);
	//printf("n2 = %d\n", n2);
	int n3 = string_to_int(argv[argc - 1]);
	//printf("n3 = %d\n", n3);


	// Параметры решетки
	int size, size_x, size_y, rank_x, rank_y, comm2d_rank;
	int prev_x, prev_y, next_x, next_y;
	int coords[2];
	int dims[2] = {0, 0};

	// Создание решетки
	MPI_Comm grid_comm2d;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	create_grid(procs_num, dims, &size_y, &size_x, &grid_comm2d);
	MPI_Comm_rank(grid_comm2d, &comm2d_rank);
	int grid_comm_size = 0;
	MPI_Comm_size(grid_comm2d, &grid_comm_size);
	//printf("grid_comm_size = %d\n", grid_comm_size);
	MPI_Cart_coords(grid_comm2d, comm2d_rank, 2, coords);

	printf("rank %d coords: %d %d\n", rank, coords[X], coords[Y]);

	rank_x = coords[X];
	rank_y = coords[Y];

	// Создание коммуникаторов для пересылки по решетке
	MPI_Comm* comms_x = create_comms(grid_comm2d, dims, X);
	MPI_Comm* comms_y = create_comms(grid_comm2d, dims, Y);

	// Массивы содержащие количество строк(колонок) матриц для каждого процесса решетки
	int* matrix1_summands;
	int* matrix2_summands;
	matrix1_summands = generate_summands(n1, dims[Y]);
	matrix2_summands = generate_summands(n2, dims[X]);
	printf("Summnds created\n");


	// Матрицы для перемножения
	double* matrix1 = NULL;
	double* matrix2 = NULL;

	double* sub_matrix1 = (double*)mallocs(matrix1_summands[rank_y] * n2 * sizeof(double));
	double* sub_matrix2 = (double*)mallocs(matrix1_summands[rank_x] * n2 * sizeof(double));

	if (rank == ROOT)
	{
		if (argc < 3)
		{
			printf("Few args\n");
			return 1;
		}

		matrix1 = reallocs(matrix1, n1 * n2 * sizeof(double));
		matrix2 = reallocs(matrix2, n2 * n3 * sizeof(double));

		printf("malloced\n");

		read_input_file("input.txt", matrix1, matrix2, n1, n2, n3);

		printf("file read\n");
	}

	if (rank_x == 0)
		matrix_partition(matrix1, n1, n2, matrix1_summands, sub_matrix1, rank_y, rank_x, dims[Y], comms_x[0]);

	print_matrix(sub_matrix1, matrix1_summands[rank_y], n2);

	if (rank == ROOT)
	{
		free(matrix1);
		free(matrix2);
	}

	free(comms_x);
	free(comms_y);
	MPI_Finalize();

	return 0;
}
