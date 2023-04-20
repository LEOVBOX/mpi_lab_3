#include <stdio.h>
#include "linear_matrix.h"
#include "mem_ops.h"
#include <mpi.h>
#include <string.h>
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
	double start_time = MPI_Wtime();

	// Чтение параметров матриц
	int n1 = string_to_int(argv[argc - 3]);
	//printf("n1 = %d\n", n1);
	int n2 = string_to_int(argv[argc - 2]);
	//printf("n2 = %d\n", n2);
	int n3 = string_to_int(argv[argc - 1]);
	//printf("n3 = %d\n", n3);


	// Параметры решетки
	int world_size, size_x, size_y, rank_x, rank_y, grid_comm_rank;
	int coords[2];
	int dims[2] = {0, 0};

	// Создание решетки
	MPI_Comm grid_comm;
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	create_grid(procs_num, dims, &size_y, &size_x, &grid_comm);
	MPI_Comm_rank(grid_comm, &grid_comm_rank);
	int grid_comm_size = 0;
	MPI_Comm_size(grid_comm, &grid_comm_size);
	//printf("grid_comm_size = %d\n", grid_comm_size);
	MPI_Cart_coords(grid_comm, grid_comm_rank, 2, coords);

	//printf("rank %d coords: %d %d\n", rank, coords[X], coords[Y]);

	rank_x = coords[X];
	rank_y = coords[Y];

	// Создание коммуникаторов для пересылки по решетке
	MPI_Comm* col_comms = create_comms(grid_comm, dims, X);
	MPI_Comm* row_comms = create_comms(grid_comm, dims, Y);

	// Массивы содержащие количество строк(колонок) матриц для каждого процесса решетки
	int* matrix_A_summands = generate_summands(n1, dims[Y]);
	if (matrix_A_summands == NULL)
	{
		exit(-1);
	}
	int* matrix_B_summands = generate_summands(n3, dims[X]);
	if (matrix_B_summands == NULL)
	{
		exit(-1);
	}

	// DEBUG BEGIN
//	print_int_array(matrix_B_summands, dims[X]);
//	printf("\n");
	//printf("Summnds created\n");
	// DEBUG END


	// Матрицы для перемножения
	double* matrix_A = NULL;
	double* matrix_B = NULL;
	double* matrix_Res = NULL;
	double* tr_matrix_B = NULL;

	double* sub_matrix_A = (double*)mallocs(matrix_A_summands[rank_y] * n2 * sizeof(double));
	double* sub_matrix_B = (double*)mallocs(matrix_A_summands[rank_x] * n2 * sizeof(double));

	// Инициализация матриц
	if (rank == ROOT)
	{
		if (argc < 3)
		{
			printf("Few args\n");
			return 1;
		}

		matrix_A = reallocs(matrix_A, n1 * n2 * sizeof(double));
		matrix_B = reallocs(matrix_B, n2 * n3 * sizeof(double));

		//printf("malloced\n");
		matrix_A = crt_default_matrix(n1, n2);
		matrix_B = crt_default_matrix(n2, n3);
		matrix_Res = (double*)mallocs(sizeof(double) * n1 * n3);
		//read_input_file("input2.txt", matrix_A, matrix_B, n1, n2, n3);

		// DEBUG BEGIN
		/*printf("matrix_B:\n");
		print_matrix(matrix_B, n2, n3);
		printf("\n");


		printf("matrix_A:\n");
		print_matrix(matrix_A, n1, n2);
		printf("\n");*/
		// DEBUG END

		tr_matrix_B = (double*)mallocs(n3 * n2 * sizeof(double));
		transpose_linear_matrix(matrix_B, n2, n3, tr_matrix_B);
		free(matrix_B);

		/*printf("tr_matrix_B:\n");
		print_matrix(tr_matrix_B, n3, n2);

		printf("file read\n");*/
	}

	// Рассылка подматриц по процессам
	if (rank_x == 0)
	{
		matrix_partition(matrix_A, n2, matrix_A_summands, sub_matrix_A, rank_y, rank_x, dims[Y], col_comms);
	}

	if(rank_y == 0)
	{
		matrix_partition(tr_matrix_B, n2, matrix_B_summands, sub_matrix_B, rank_x, rank_y, dims[X], row_comms);
	}

	// Транслирование подматриц по столбцам (строкам) решетки
	data_broadcast(sub_matrix_A, sub_matrix_B, rank_y, rank_x, matrix_A_summands, matrix_B_summands, dims, n2, row_comms, col_comms);

	// DEBUG BEGIN

//	printf("(x = %d, y = %d) subm1:\n", rank_x, rank_y);
//	print_matrix(sub_matrix_A, matrix_A_summands[rank_y], n2);
//	printf("\n");
//
//	printf("(y = %d, x = %d) subm2:\n", rank_y, rank_x);
//	print_matrix(sub_matrix_B, matrix_B_summands[rank_x], n2);
//	printf("\n");

	// DEBUG END

	// Перемножение подматриц
	double* sub_matrix_Res = mult_matrix(sub_matrix_A, sub_matrix_B, matrix_A_summands[rank_y], n2, matrix_B_summands[rank_x]);

	// DEBUG BEGIN
//	printf("(x = %d, y = %d) result:\n", rank_x, rank_y);
//	print_matrix(sub_matrix_Res, matrix_A_summands[rank_y], matrix_B_summands[rank_y]);
//	printf("\n");
	// DEBUG END

	// Сбор матрицы
	collect_res_matrix(matrix_A_summands[rank_y], matrix_B_summands[rank_x], n3, rank, procs_num, matrix_Res, sub_matrix_Res,
			dims, grid_comm);

	//printf("Collect success\n %d", rank);




	free(matrix_Res);
	free(matrix_A_summands);
	free(sub_matrix_A);
	free(sub_matrix_B);
	free(col_comms);
	free(row_comms);
	double end_time = MPI_Wtime();

	if (rank == ROOT)
	{
		//print_matrix(matrix_Res, n1, n3);
		free(matrix_A);
		free(tr_matrix_B);
		printf("TIME: %lf\n", end_time - start_time);
	}

	MPI_Finalize();



	return 0;
}
